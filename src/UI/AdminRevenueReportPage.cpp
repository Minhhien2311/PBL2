#include "AdminRevenueReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "PageRefresher.h"

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
#include <QDebug>
#include <numeric>

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
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:16px; }"
        "QFrame#StatCard { background: white; border:1px solid #0E3B7C; }"
    );

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ========== TOP BAR ==========
    QWidget* topBar = new QWidget(this);
    auto* topBarLayout = new QVBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 20, 24, 16);
    topBarLayout->setSpacing(16);

    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);

    QLabel* pageTitle = new QLabel("Báo cáo doanh thu", topBar);
    pageTitle->setProperty("class", "PageTitle");
    headerRow->addWidget(pageTitle);
    headerRow->addStretch();

    QPushButton* refreshBtn = new QPushButton("🔄 Làm mới", topBar);
    refreshBtn->setStyleSheet(
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:6px; height:32px; padding:0 16px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }"
    );
    refreshBtn->setCursor(Qt::PointingHandCursor);
    headerRow->addWidget(refreshBtn);

    topBarLayout->addLayout(headerRow);

    QHBoxLayout* statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    QFrame* box1 = createRevenueBox("TRONG NGÀY", "0 triệu VND");
    QFrame* box2 = createRevenueBox("TRONG TUẦN", "0 triệu VND");
    QFrame* box3 = createRevenueBox("TRONG THÁNG", "0 triệu VND");

    statsLayout->addWidget(box1);
    statsLayout->addWidget(box2);
    statsLayout->addWidget(box3);

    topBarLayout->addLayout(statsLayout);
    mainLayout->addWidget(topBar);

    // ========== TIÊU ĐỀ BIỂU ĐỒ ==========
    QWidget* chartHeader = new QWidget(this);
    auto* chartHeaderLayout = new QHBoxLayout(chartHeader);
    chartHeaderLayout->setContentsMargins(24, 6, 24, 0);
    chartHeaderLayout->setSpacing(0);

    QLabel* chartTitle = new QLabel("Doanh thu theo tháng (Toàn hệ thống)", chartHeader);
    chartTitle->setProperty("class", "SectionTitle");
    chartHeaderLayout->addWidget(chartTitle);
    chartHeaderLayout->addStretch();

    mainLayout->addWidget(chartHeader);

    // ========== BIỂU ĐỒ TRONG KHUNG TRẮNG (KHÔNG CÓ SCROLL) ==========
    QWidget* chartContainer = new QWidget(this);
    auto* chartContainerLayout = new QVBoxLayout(chartContainer);
    chartContainerLayout->setContentsMargins(24, 6, 24, 24);

    QFrame* chartFrame = new QFrame(chartContainer);
    chartFrame->setStyleSheet(
        "QFrame { background: white; border: 1px solid #c2cfe2; border-radius: 8px; }"
    );
    auto* chartFrameLayout = new QVBoxLayout(chartFrame);
    chartFrameLayout->setContentsMargins(16, 16, 16, 16);

    chart_ = new QChart();
    chart_->setTitle("");
    chart_->setBackgroundVisible(false);

    chartView_ = new QChartView(chart_, chartFrame);
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setMinimumHeight(400);

    // === FIX: THÊM TRỰC TIẾP chartView_, KHÔNG QUA ScrollArea ===
    chartFrameLayout->addWidget(chartView_);
    chartContainerLayout->addWidget(chartFrame);
    
    mainLayout->addWidget(chartContainer, 1);

    connect(refreshBtn, &QPushButton::clicked, this, &AdminRevenueReportPage::refreshPage);
}

QFrame* AdminRevenueReportPage::createRevenueBox(const QString& title, const QString& value)
{
    // === KHUNG NGOÀI (GIỐNG DASHBOARDPAGE) ===
    QFrame* card = new QFrame(this);
    card->setObjectName("StatCard");
    card->setFixedHeight(90);  // ← Chiều cao giống DashboardPage
    card->setStyleSheet(
        "QFrame#StatCard {"
        "   background: white;"
        "   border: 1px solid #0E3B7C;"
        "}"
    );

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    // === HEADER XANH ĐẬM (GIỐNG DASHBOARDPAGE) ===
    QWidget* head = new QWidget(card);
    head->setStyleSheet("background:#0E3B7C; color:white;");
    head->setFixedHeight(28);
    
    auto* headLayout = new QHBoxLayout(head);
    headLayout->setContentsMargins(10, 0, 10, 0);
    
    QLabel* titleLabel = new QLabel(title, head);
    titleLabel->setStyleSheet("font-size:13px; font-weight:600; color:white;");
    titleLabel->setAlignment(Qt::AlignCenter);
    headLayout->addWidget(titleLabel);

    // === BODY TRẮNG (GIỐNG DASHBOARDPAGE) ===
    QWidget* body = new QWidget(card);
    auto* bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(10, 6, 10, 6);

    QLabel* valueLabel = new QLabel(value, body);
    valueLabel->setStyleSheet("color:#0E3B7C; font-size:20px; font-weight:700;");
    valueLabel->setAlignment(Qt::AlignCenter);
    bodyLayout->addWidget(valueLabel, 0, Qt::AlignCenter);

    // === LƯU TRỮ POINTER (để updateData() cập nhật sau) ===
    if (title == "TRONG NGÀY") {
        dailyRevenueLabel_ = valueLabel;
    } else if (title == "TRONG TUẦN") {
        weeklyRevenueLabel_ = valueLabel;
    } else if (title == "TRONG THÁNG") {
        monthlyRevenueLabel_ = valueLabel;
    }

    cardLayout->addWidget(head);
    cardLayout->addWidget(body);

    return card;
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

    // 6. Tạo trục X (Không có tiêu đề ở giữa)
    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    // ← XÓA: axisX->setTitleText("Tháng");  // Không dùng title ở giữa nữa
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries_->attachAxis(axisX);

    // 7. Tạo trục Y (Không có tiêu đề ở giữa)
    auto* axisY = new QValueAxis();
    axisY->setRange(0, yAxisMax);
    // Cách 2: Chỉ số, riêng đơn vị
    axisY->setLabelFormat("%.1f");
    axisY->setTitleText("Triệu VND");  // ← Title sẽ hiển thị ở giữa trục Y
    QFont titleFont;
    titleFont.setPointSize(10);
    titleFont.setBold(true);
    axisY->setTitleFont(titleFont);  // ← Font cho title
    // axisY->setTitleBrush(QBrush(QColor(18, 59, 122)));  // ← Màu xanh đậm (tùy chọn)
    // ← XÓA: axisY->setTitleText("Doanh thu (Triệu VND)");  // Không dùng title ở giữa
    chart_->addAxis(axisY, Qt::AlignLeft);
    chartSeries_->attachAxis(axisY);

    // 8. Thêm hiệu ứng và ẨN LEGEND (xóa ô "Doanh thu" dưới biểu đồ)
    chart_->setTitle("");  // Xóa title vì đã có tiêu đề riêng ở trên
    chart_->setAnimationOptions(QChart::SeriesAnimations);
    chart_->legend()->setVisible(false);  // ← ẨN LEGEND (ô doanh thu dưới biểu đồ)

    // === BỎ ĐƯỜNG KẺ DỌC (CHỈ GIỮ KẺ NGANG) ===
    chart_->setBackgroundVisible(true);
    chart_->setBackgroundBrush(Qt::white);
    
    // Tắt grid dọc cho trục X
    axisX->setGridLineVisible(false);  // ← Bỏ đường kẻ dọc
    
    // Giữ grid ngang cho trục Y
    axisY->setGridLineVisible(true);   // ← Giữ đường kẻ ngang
    axisY->setGridLineColor(QColor(220, 220, 220));  // Màu xám nhạt
    
    // === TĂNG PADDING CHO BIỂU ĐỒ (không sát viền) ===
    chart_->setMargins(QMargins(6, 20, 20, 20));
    chartView_->setChart(chart_);
    chartView_->update();
}

void AdminRevenueReportPage::refreshPage() {
    PageRefresher::executeRefresh([this]() {
        updateData();
    });
}