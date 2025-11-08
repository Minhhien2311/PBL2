#include "AgentRevenueReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/Booking.h"

#include <QDate>
#include <QDateTime>
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
#include <QDebug> // Thêm thư viện này
#include <numeric> // Thêm thư viện này

AgentRevenueReportPage::AgentRevenueReportPage(AccountManager* am,
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

AgentRevenueReportPage::~AgentRevenueReportPage()
{
    // Qt sẽ tự động dọn dẹp
}

void AgentRevenueReportPage::setupUI()
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    // --- Nút làm mới ---
    auto* refreshLayout = new QHBoxLayout();
    refreshLayout->setAlignment(Qt::AlignLeft);

    auto* refreshBtn = new QPushButton("Làm mới", this);
    refreshLayout->addWidget(refreshBtn);
    refreshLayout->addStretch();

    layout->addLayout(refreshLayout);
    layout->addSpacing(10);

    // --- 1. Hàng thống kê tổng (3 hộp) ---
    auto* summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(20);

    // Tạo 3 ô doanh thu
    QFrame* box1 = createRevenueBox("TRONG NGÀY", "0 triệu VND");
    QFrame* box2 = createRevenueBox("TRONG TUẦN", "0 triệu VND");
    QFrame* box3 = createRevenueBox("TRONG THÁNG", "0 triệu VND");

    summaryLayout->addWidget(box1);
    summaryLayout->addWidget(box2);
    summaryLayout->addWidget(box3);
    layout->addLayout(summaryLayout);
    layout->addSpacing(20);

    // --- 2. Biểu đồ ---
    chart_ = new QChart();
    chart_->setTitle("Doanh thu theo tháng");
    chart_->legend()->setVisible(false);

    chartView_ = new QChartView(chart_, this);
    chartView_->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(chartView_);

    // Kết nối nút làm mới
    connect(refreshBtn, &QPushButton::clicked, this, &AgentRevenueReportPage::updateData);
}

QFrame* AgentRevenueReportPage::createRevenueBox(const QString& title, const QString& value)
{
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

    // Lưu con trỏ đến label giá trị để cập nhật sau
    if (title == "TRONG NGÀY") {
    dailyRevenueLabel_ = valueLabel;
    } else if (title == "TRONG TUẦN") {
    weeklyRevenueLabel_ = valueLabel;
    } else if (title == "TRONG THÁNG") {
    monthlyRevenueLabel_ = valueLabel;
    }

    return box;
}

void AgentRevenueReportPage::updateData()
{
    // Cập nhật dữ liệu thực từ ReportManager - cho agent hiện tại
    double todayRevenue = reportManager_->getAgentTodayRevenue();
    double weekRevenue = reportManager_->getAgentThisWeekRevenue();
    double monthRevenue = reportManager_->getAgentThisMonthRevenue();

    // Chuyển đổi sang triệu VND (giả sử đơn vị là VND)
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

// =================================================================
// === HÀM UPDATECHART ĐÃ ĐƯỢC TÁI CẤU TRÚC (REFACTOR) HOÀN TOÀN ===
// =================================================================
void AgentRevenueReportPage::updateChart()
{
    // Xóa hoàn toàn biểu đồ cũ và tạo mới
    if (chart_) {
    chart_->removeAllSeries();

    // Xóa tất cả các trục
    for (QAbstractAxis* axis : chart_->axes()) {
        chart_->removeAxis(axis);
    }
    } else {
    chart_ = new QChart();
    chartView_->setChart(chart_);
    }

    // Tạo series mới
    chartSeries_ = new QBarSeries();

    // --- TÁI CẤU TRÚC ---
    // 1. Lấy dữ liệu đã tính toán sẵn từ ReportManager
    int currentYear = QDate::currentDate().year();
    qDebug() << "=== Revenue Chart Data ===";
    qDebug() << "Fetching monthly revenue for year:" << currentYear;

    // Hàm này đã có sẵn trong file ReportManager.cpp bạn gửi!
    std::vector<double> monthlyRevenue = reportManager_->getAgentMonthlyRevenue(currentYear);

    // (Toàn bộ vòng lặp for, QDebug, parse ngày... đã được XÓA khỏi đây)
    // --- KẾT THÚC TÁI CẤU TRÚC ---

    // 2. Chỉ tập trung vào VẼ DỮ LIỆU
    auto* barSet = new QBarSet("Doanh thu");
    barSet->setColor(QColor(91, 134, 198)); // Màu xanh dương đẹp
    barSet->setBorderColor(Qt::darkBlue);

    QStringList categories;
    categories << "Tháng 1" << "Tháng 2" << "Tháng 3" << "Tháng 4" << "Tháng 5" << "Tháng 6" 
        << "Tháng 7" << "Tháng 8" << "Tháng 9" << "Tháng 10" << "Tháng 11" << "Tháng 12";

    double maxRevenueInMillion = 0.0;
    // Tính tổng doanh thu để kiểm tra xem có dữ liệu hay không
    double totalRevenue = std::accumulate(monthlyRevenue.begin(), monthlyRevenue.end(), 0.0);

    qDebug() << "=== Monthly Revenue (from ReportManager) ===";

    // Tìm max revenue (tính bằng triệu)
    for (int i = 0; i < 12; ++i) {
    double revenueInMillion = monthlyRevenue[i] / 1000000.0;
    if (revenueInMillion > maxRevenueInMillion) {
        maxRevenueInMillion = revenueInMillion;
    }
    qDebug() << categories[i] << ":" << revenueInMillion << "triệu VND";
    }

    double yAxisMax = 1.0; // Mặc định trục Y

    // Logic dữ liệu giả
    if (totalRevenue == 0.0) {
    qDebug() << "No revenue data, using dummy data for display";
    for (int i = 0; i < 12; ++i) {
        *barSet << 0.1; // Thêm 12 giá trị giả (0.1 triệu)
    }
    yAxisMax = 1.0; // Đặt trục Y là 1 triệu
    } else {
    qDebug() << "Using real revenue data";
    for (double revenue : monthlyRevenue) {
        *barSet << (revenue / 1000000.0); // Thêm 12 giá trị thật
    }
    yAxisMax = maxRevenueInMillion * 1.2; // Thêm 20% khoảng đệm
    }

    chartSeries_->append(barSet);
    chart_->addSeries(chartSeries_);

    // 3. Cấu hình trục
    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Tháng");
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries_->attachAxis(axisX);

    // Cấu hình trục Y
    auto* axisY = new QValueAxis();
    axisY->setRange(0, yAxisMax); // Sử dụng yAxisMax đã tính
    axisY->setLabelFormat("%.1f triệu VND");
    axisY->setTitleText("Doanh thu");
    chart_->addAxis(axisY, Qt::AlignLeft);
    chartSeries_->attachAxis(axisY);

    // 4. Cấu hình biểu đồ
    chart_->setTitle("Doanh thu theo Tháng - Năm " + QString::number(currentYear));
    chart_->setAnimationOptions(QChart::SeriesAnimations);
    chart_->legend()->setVisible(true);
    chart_->legend()->setAlignment(Qt::AlignBottom);

    // Đảm bảo biểu đồ được cập nhật
    chartView_->setChart(chart_);
    chartView_->update();

    qDebug() << "=== Revenue Chart Update Complete ===";
}