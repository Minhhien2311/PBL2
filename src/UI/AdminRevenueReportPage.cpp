#include "AdminRevenueReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"

#include <QDate>     
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QChartView>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDebug> // Thêm thư viện
#include <numeric> // Thêm thư viện

AdminRevenueReportPage::AdminRevenueReportPage(AccountManager* am,
                        BookingManager* bm,
                        ReportManager* rm,
                        QWidget* parent)
    : QWidget(parent),
    accountManager_(am),
    bookingManager_(bm),
    reportManager_(rm),
    dailyRevenueLabel_(nullptr),
    weeklyRevenueLabel_(nullptr),
    monthlyRevenueLabel_(nullptr),
    chartView_(nullptr),
    chart_(nullptr),
    chartSeries_(nullptr)
{
    setupUI();
    updateData();
}

AdminRevenueReportPage::~AdminRevenueReportPage()
{
    // Qt sẽ tự động dọn dẹp
}

void AdminRevenueReportPage::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    // --- Nút làm mới ---
    // (Giữ nguyên)
    auto* refreshLayout = new QHBoxLayout();
    refreshLayout->setAlignment(Qt::AlignLeft);

    auto* refreshBtn = new QPushButton("Làm mới", this);
    refreshLayout->addWidget(refreshBtn);
    refreshLayout->addStretch();

    layout->addLayout(refreshLayout);
    layout->addSpacing(10);

    // --- 1. Hàng thống kê tổng (3 hộp) ---
    // (Giữ nguyên)
    auto* summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(20);

    QFrame* box1 = createRevenueBox("TRONG NGÀY", "0 triệu VND");
    QFrame* box2 = createRevenueBox("TRONG TUẦN", "0 triệu VND");
    QFrame* box3 = createRevenueBox("TRONG THÁNG", "0 triệu VND");

    summaryLayout->addWidget(box1);
    summaryLayout->addWidget(box2);
    summaryLayout->addWidget(box3);
    layout->addLayout(summaryLayout);
    layout->addSpacing(20);

    // --- 2. Biểu đồ ---
    // === SỬA: Chỉ tạo QChart và QChartView ===
    chart_ = new QChart();
    chart_->setTitle("Doanh thu theo tháng (Toàn hệ thống)");

    chartView_ = new QChartView(chart_, this);
    chartView_->setRenderHint(QPainter::Antialiasing);

    // (Đã xóa code tạo series và trục ở đây)

    layout->addWidget(chartView_);

    // Kết nối nút làm mới
    connect(refreshBtn, &QPushButton::clicked, this, &AdminRevenueReportPage::updateData);
}

QFrame* AdminRevenueReportPage::createRevenueBox(const QString& title, const QString& value)
{
    // (Giữ nguyên)
    auto* box = new QFrame(this);
    box->setFrameShape(QFrame::StyledPanel);
    box->setStyleSheet(
    "QFrame { background: white; border: 1px solid #C9D6EB; border-radius: 8px; padding: 10px; }"
    );

    auto* boxLayout = new QVBoxLayout(box);

    auto* titleLabel = new QLabel(title, box);
    titleLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #5B86C6; background: transparent; border: none;");

    auto* valueLabel = new QLabel(value, box);
    valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #123B7A; background: transparent; border: none;");

    boxLayout->addWidget(titleLabel);
    boxLayout->addWidget(valueLabel);

    // (Sửa lại key một chút cho khớp với UI)
    if (title == "TRONG NGÀY") {
    dailyRevenueLabel_ = valueLabel;
    } else if (title == "TRONG TUẦN") {
    weeklyRevenueLabel_ = valueLabel;
    } else if (title == "TRONG THÁNG") {
    monthlyRevenueLabel_ = valueLabel;
    }

    return box;
}

void AdminRevenueReportPage::updateData()
{
    // (Giữ nguyên)
    double todayRevenue = reportManager_->getTodayRevenue();
    double weekRevenue = reportManager_->getThisWeekRevenue();
    double monthRevenue = reportManager_->getThisMonthRevenue();

    // Chuyển đổi sang triệu VND
    todayRevenue /= 1000000;
    weekRevenue /= 1000000;
    monthRevenue /= 1000000;

    // Cập nhật các label
    if (dailyRevenueLabel_) {
    dailyRevenueLabel_->setText(QString::number(todayRevenue, 'f', 1) + " triệu VND");
    }
    if (weeklyRevenueLabel_) {
    weeklyRevenueLabel_->setText(QString::number(weekRevenue, 'f', 1) + " triệu VND");
    }
    if (monthlyRevenueLabel_) {
    monthlyRevenueLabel_->setText(QString::number(monthRevenue, 'f', 1) + " triệu VND");
    }

    updateChart();
}

void AdminRevenueReportPage::updateChart()
{
    // 1. Xóa dữ liệu cũ
    if (chart_) {
    chart_->removeAllSeries();
    for (QAbstractAxis* axis : chart_->axes()) {
        chart_->removeAxis(axis);
    }
    } else {
    chart_ = new QChart();
    chartView_->setChart(chart_);
    }

    // 2. Tạo series mới
    chartSeries_ = new QBarSeries();
     
    // 3. Lấy dữ liệu
    int currentYear = QDate::currentDate().year();
    qDebug() << "=== Admin Revenue Chart Data ===";
    qDebug() << "Fetching total monthly revenue for year:" << currentYear;

    // Lấy TỔNG doanh thu, không phải của riêng agent nào
    std::vector<double> monthlyRevenue = reportManager_->getMonthlyRevenue(currentYear);

    // 4. Vẽ dữ liệu
    auto* barSet = new QBarSet("Doanh thu");
    // === THÊM: Màu sắc ===
    barSet->setColor(QColor(91, 134, 198));
    barSet->setBorderColor(Qt::darkBlue);

    // === SỬA: Dùng Tiếng Việt ===
    QStringList categories;
    categories << "Tháng 1" << "Tháng 2" << "Tháng 3" << "Tháng 4" << "Tháng 5" << "Tháng 6" 
        << "Tháng 7" << "Tháng 8" << "Tháng 9" << "Tháng 10" << "Tháng 11" << "Tháng 12";

    double maxRevenueInMillion = 0.0;
    double totalRevenue = std::accumulate(monthlyRevenue.begin(), monthlyRevenue.end(), 0.0);

    qDebug() << "=== Monthly Revenue (from ReportManager) ===";
    for (int i = 0; i < 12; ++i) {
    double revenueInMillion = monthlyRevenue[i] / 1000000.0;
    if (revenueInMillion > maxRevenueInMillion) {
        maxRevenueInMillion = revenueInMillion;
    }
    qDebug() << categories[i] << ":" << revenueInMillion << "triệu VND";
    }

    double yAxisMax = 1.0; // Mặc định trục Y

    // 5. Xử lý dữ liệu giả
    if (totalRevenue == 0.0) {
    qDebug() << "No revenue data, using dummy data for display";
    for (int i = 0; i < 12; ++i) {
        *barSet << 0.1; // Thêm 12 giá trị giả
    }
    yAxisMax = 1.0;
    } else {
    qDebug() << "Using real revenue data";
    for (double revenue : monthlyRevenue) {
        *barSet << (revenue / 1000000.0); // Thêm 12 giá trị thật
    }
    yAxisMax = maxRevenueInMillion * 1.2; // Thêm 20% khoảng đệm
    }

    chartSeries_->append(barSet);
    chart_->addSeries(chartSeries_);

    // 6. Tạo trục X (MỚI)
    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Tháng");
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries_->attachAxis(axisX);

    // 7. Tạo trục Y (MỚI)
    auto* axisY = new QValueAxis();
    axisY->setRange(0, yAxisMax);
    axisY->setLabelFormat("%.1f"); // Hiển thị 1 chữ số thập phân
    axisY->setTitleText("Doanh thu (Triệu VND)");
    chart_->addAxis(axisY, Qt::AlignLeft);
    chartSeries_->attachAxis(axisY);

    // 8. Thêm hiệu ứng và legend
    chart_->setTitle("Doanh thu theo Tháng (Toàn hệ thống) - Năm " + QString::number(currentYear));
    chart_->setAnimationOptions(QChart::SeriesAnimations);
    chart_->legend()->setVisible(true);
    chart_->legend()->setAlignment(Qt::AlignBottom);

    chartView_->setChart(chart_);
    chartView_->update();
}