#include "AdminTicketsReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/AccountAgent.h"
#include "PageRefresher.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateEdit>
#include <QPushButton>
#include <QFrame>
#include <QChartView>
#include <QChart>
#include <QBarSeries>
#include <QHorizontalBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDebug>
#include <QScrollArea>

AdminTicketsReportPage::AdminTicketsReportPage(AccountManager* am,
                       BookingManager* bm,
                        ReportManager* rm,
                        QWidget* parent)
    : QWidget(parent),
    accountManager_(am),
    bookingManager_(bm),
    reportManager_(rm),
    startDate_(nullptr),
    endDate_(nullptr),
    refreshBtn_(nullptr),
    totalTicketsLabel_(nullptr),
    economyTicketsLabel_(nullptr),
    businessTicketsLabel_(nullptr),
    cancelledTicketsLabel_(nullptr),
    changedTicketsLabel_(nullptr),
    chartView_(nullptr),
    chart_(nullptr),
    chartSeries_(nullptr)
{
    setupUI();
    onRefreshClicked();
}

AdminTicketsReportPage::~AdminTicketsReportPage()
{
    // Qt sẽ tự động dọn dẹp
}

void AdminTicketsReportPage::setupUI()
{
    // === STYLE GIỐNG ADMINREVENUEREPORTPAGE ===
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:16px; }"
        "QFrame#StatCard { background: white; border:1px solid #0E3B7C; }"
    );

    // === LAYOUT CHÍNH (0 margin để dính sidebar) ===
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ========== TOP BAR (CÓ LỀ 24PX) ==========
    QWidget* topBar = new QWidget(this);
    auto* topBarLayout = new QVBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 16, 24, 12);
    topBarLayout->setSpacing(12);

    // --- Hàng 2: KHUNG CHỌN KHOẢNG THỜI GIAN (BO VIỀN TRẮNG) ---
    QWidget* dateBox = new QWidget;
    QVBoxLayout* dateBoxLayout = new QVBoxLayout(dateBox);
    dateBoxLayout->setContentsMargins(10, 10, 10, 10);
    dateBoxLayout->setSpacing(6);
    
    dateBox->setStyleSheet(
        "QWidget { background: white; border: 1px solid #c2cfe2; border-radius: 6px; }"
    );

    QLabel* dateBoxTitle = new QLabel("📅 Chọn khoảng thời gian cần thống kê");
    dateBoxTitle->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    dateBoxLayout->addWidget(dateBoxTitle);

    // === FIX: Giãn đều các ô nhập và nút tìm kiếm ===
    QHBoxLayout* dateRow = new QHBoxLayout();
    dateRow->setSpacing(12);  // Khoảng cách đều giữa các phần tử

    QLabel* fromLabel = new QLabel("Từ ngày:");
    fromLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    dateRow->addWidget(fromLabel);

    startDate_ = new QDateEdit(QDate::currentDate().addDays(-30), dateBox);
    startDate_->setCalendarPopup(true);
    startDate_->setDisplayFormat("dd/MM/yyyy");
    startDate_->setMinimumHeight(36);
    startDate_->setMinimumWidth(160);  // ← THAY ĐỔI: Từ fixedWidth sang minWidth
    dateRow->addWidget(startDate_, 1);  // ← THÊM stretch factor = 1

    QLabel* toLabel = new QLabel("Đến ngày:");
    toLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    dateRow->addWidget(toLabel);

    endDate_ = new QDateEdit(QDate::currentDate(), dateBox);
    endDate_->setCalendarPopup(true);
    endDate_->setDisplayFormat("dd/MM/yyyy");
    endDate_->setMinimumHeight(36);
    endDate_->setMinimumWidth(160);  // ← THAY ĐỔI: Từ fixedWidth sang minWidth
    dateRow->addWidget(endDate_, 1);  // ← THÊM stretch factor = 1

    refreshBtn_ = new QPushButton("🔍 Truy vấn", dateBox);
    refreshBtn_->setMinimumHeight(36);
    refreshBtn_->setMinimumWidth(100);  // ← GIẢM: Từ 120px xuống 100px
    refreshBtn_->setMaximumWidth(150);  // ← THÊM: Giới hạn chiều rộng tối đa
    refreshBtn_->setCursor(Qt::PointingHandCursor);
    refreshBtn_->setStyleSheet(
        "QPushButton { background:#4478BD; color:white; font-weight:600; "
        "border-radius:6px; padding: 0 16px; }"
        "QPushButton:hover { background:#365a9e; }"
    );
    dateRow->addWidget(refreshBtn_);
    // ← BỎ addStretch() để các phần tử tự giãn đều

    dateBoxLayout->addLayout(dateRow);
    topBarLayout->addWidget(dateBox);

    // --- Hàng 3: 5 Ô THỐNG KÊ (GIỐNG DASHBOARDPAGE) ---
    QHBoxLayout* statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(12);

    QFrame* box1 = createSummaryBox("TỔNG VÉ ĐÃ BÁN", totalTicketsLabel_);
    QFrame* box2 = createSummaryBox("VÉ PHỔ THÔNG", economyTicketsLabel_);
    QFrame* box3 = createSummaryBox("VÉ THƯƠNG GIA", businessTicketsLabel_);
    QFrame* box4 = createSummaryBox("VÉ ĐÃ HỦY", cancelledTicketsLabel_);
    QFrame* box5 = createSummaryBox("VÉ ĐÃ ĐỔI", changedTicketsLabel_);

    statsLayout->addWidget(box1);
    statsLayout->addWidget(box2);
    statsLayout->addWidget(box3);
    statsLayout->addWidget(box4);
    statsLayout->addWidget(box5);

    topBarLayout->addLayout(statsLayout);

    mainLayout->addWidget(topBar);

    // ========== TIÊU ĐỀ BIỂU ĐỒ ==========
    QWidget* chartHeader = new QWidget(this);
    auto* chartHeaderLayout = new QHBoxLayout(chartHeader);
    chartHeaderLayout->setContentsMargins(24, 6, 24, 0);
    chartHeaderLayout->setSpacing(0);

    QLabel* chartTitle = new QLabel("Số vé đã bán theo Đại lý", chartHeader);
    chartTitle->setProperty("class", "SectionTitle");
    chartHeaderLayout->addWidget(chartTitle);
    chartHeaderLayout->addStretch();

    mainLayout->addWidget(chartHeader);

    // ========== BIỂU ĐỒ TRONG KHUNG TRẮNG + SCROLL ==========
    QWidget* chartContainer = new QWidget(this);
    auto* chartContainerLayout = new QVBoxLayout(chartContainer);
    chartContainerLayout->setContentsMargins(24, 6, 24, 24);

    // === KHUNG TRẮNG CHỨA BIỂU ĐỒ ===
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
    chartView_->setMinimumWidth(800);

    // === THÊM QSCROLLAREA ===
    QScrollArea* scrollArea = new QScrollArea(chartFrame);
    scrollArea->setWidget(chartView_);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // ← Tắt scroll ngang
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setMinimumHeight(300);
    scrollArea->setMaximumHeight(450);
    scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
    );

    chartFrameLayout->addWidget(scrollArea);
    chartContainerLayout->addWidget(chartFrame);
    
    mainLayout->addWidget(chartContainer, 1);

    connect(refreshBtn_, &QPushButton::clicked, this, &AdminTicketsReportPage::onRefreshClicked);
}

QFrame* AdminTicketsReportPage::createSummaryBox(const QString& title, QLabel*& valueLabel)
{
    QFrame* card = new QFrame(this);
    card->setObjectName("StatCard");
    card->setFixedHeight(80);
    card->setStyleSheet(
        "QFrame#StatCard {"
        "   background: white;"
        "   border: 1px solid #0E3B7C;"
        "}"
    );

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    QWidget* head = new QWidget(card);
    head->setStyleSheet("background:#0E3B7C; color:white;");
    head->setFixedHeight(28);
    
    auto* headLayout = new QHBoxLayout(head);
    headLayout->setContentsMargins(10, 0, 10, 0);
    
    QLabel* titleLabel = new QLabel(title, head);
    titleLabel->setStyleSheet("font-size:13px; font-weight:600; color:white;");
    titleLabel->setAlignment(Qt::AlignCenter);
    headLayout->addWidget(titleLabel);

    QWidget* body = new QWidget(card);
    auto* bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(10, 6, 10, 6);

    if (valueLabel == nullptr) {
        valueLabel = new QLabel("0", body);
    }
    valueLabel->setStyleSheet("color:#0E3B7C; font-size:20px; font-weight:700;");
    valueLabel->setAlignment(Qt::AlignCenter);
    bodyLayout->addWidget(valueLabel, 0, Qt::AlignCenter);

    cardLayout->addWidget(head);
    cardLayout->addWidget(body);

    return card;
}

void AdminTicketsReportPage::onRefreshClicked()
{
    auto start = startDate_->date().toString("dd/MM/yyyy").toStdString();
    auto end = endDate_->date().toString("dd/MM/yyyy").toStdString();

    int totalSold = reportManager_->getTicketsSoldInRange(start, end);
    int economySold = reportManager_->getEconomyTicketsInRange(start, end);
    int businessSold = reportManager_->getBusinessTicketsInRange(start, end);
    
    int cancelled = reportManager_->getCancelledTicketsInRange(start, end);
    int changed = reportManager_->getChangedTicketsInRange(start, end);

    totalTicketsLabel_->setText(QString::number(totalSold));
    economyTicketsLabel_->setText(QString::number(economySold));
    businessTicketsLabel_->setText(QString::number(businessSold));
    cancelledTicketsLabel_->setText(QString::number(cancelled));
    changedTicketsLabel_->setText(QString::number(changed));

    updateChart(start, end);
}

void AdminTicketsReportPage::updateChart(const std::string& start, const std::string& end)
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

    // 2. Tạo series mới (BIỂU ĐỒ THANH NGANG)
    auto* chartSeries = new QHorizontalBarSeries();

    // 3. Lấy dữ liệu
    auto* reports = reportManager_->generateAgentReportInRange(start, end);

    QStringList categories;
    QList<int> ticketCounts;
    auto* barSet = new QBarSet("Số vé");
    barSet->setColor(QColor(91, 134, 198));
    barSet->setBorderColor(Qt::darkBlue);

    int maxTickets = 0;
    bool hasData = false;

    if (reports) {
        for (auto* r : *reports) {
            if (!r) continue;

            if (r->issuedTickets > 0) {
                *barSet << r->issuedTickets;
                categories << QString::fromStdString(r->agentName);
                ticketCounts << r->issuedTickets;
                
                if (r->issuedTickets > maxTickets) maxTickets = r->issuedTickets;
                hasData = true;
            }
            delete r;
        }
        delete reports;
    }

    int xAxisMax = 10;

    if (!hasData) {
        qDebug() << "No ticket data for any agent, using dummy data";
        *barSet << 1;
        categories << "Không có dữ liệu";
        xAxisMax = 10;
    } else {
        // === FIX: Tăng khoảng đệm để số không bị cắt ===
        xAxisMax = maxTickets + (maxTickets * 0.15);  // ← Thêm 15% khoảng đệm
        // VD: maxTickets=346 → xAxisMax = 346 + 52 = 398
    }

    // === FIX: Cấu hình hiển thị số trên thanh ===
    barSet->setLabelColor(QColor(18, 59, 122));
    QFont labelFont;
    labelFont.setPointSize(9);
    labelFont.setBold(true);
    barSet->setLabelFont(labelFont);  // ← THÊM: Font cho số
    
    chartSeries->setLabelsVisible(true);
    chartSeries->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
    chartSeries->setLabelsFormat("@value");  // ← THÊM: Format số (bỏ ký hiệu thập phân)
    
    chartSeries->append(barSet);
    chart_->addSeries(chartSeries);

    // 5. Tạo trục Y (DANH SÁCH ĐẠI LÝ - BÊN TRÁI)
    auto* axisY = new QBarCategoryAxis();
    axisY->append(categories);

    int agentCount = categories.size();
    QFont axisLabelFont;
    
    if (agentCount > 20) {
        axisLabelFont.setPointSize(7);
    } else if (agentCount > 10) {
        axisLabelFont.setPointSize(8);
    } else {
        axisLabelFont.setPointSize(9);
    }
    
    axisY->setLabelsFont(axisLabelFont);
    axisY->setLabelsAngle(0);
    axisY->setLabelsColor(QColor(18, 59, 122));
    
    chart_->addAxis(axisY, Qt::AlignLeft);
    chartSeries->attachAxis(axisY);

    // 6. Tạo trục X (SỐ VÉ - PHÍA DƯỚI)
    auto* axisX = new QValueAxis();
    axisX->setRange(0, xAxisMax);
    axisX->setLabelFormat("%d");
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries->attachAxis(axisX);

    // 7. Cấu hình biểu đồ
    chart_->setTitle("");
    chart_->setAnimationOptions(QChart::SeriesAnimations);
    chart_->legend()->setVisible(false);

    chart_->setBackgroundVisible(true);
    chart_->setBackgroundBrush(Qt::white);
    axisY->setGridLineVisible(false);
    axisX->setGridLineVisible(true);
    axisX->setGridLineColor(QColor(220, 220, 220));

    // === FIX CHÍNH: Tăng margin phải để số không bị cắt ===
    chart_->setMargins(QMargins(20, 10, 30, 10));
    // ↑ left=30px (giữ nguyên)
    // ↑ right=80px (tăng từ 60px) để đủ chỗ cho số 3 chữ số

    // ========== TĂNG CHIỀU CAO ĐỘNG ==========
    int minHeightPerAgent = 35;
    int calculatedHeight = agentCount * minHeightPerAgent;
    chartView_->setMinimumHeight(calculatedHeight);

    chartView_->setChart(chart_);
    chartView_->update();
}