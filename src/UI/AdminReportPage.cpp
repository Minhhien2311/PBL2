#include "AdminReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"

// --- Includes cho layout ---
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QDateEdit>
#include <QPushButton>
#include <QFrame>
#include <QChartView>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>

AdminReportPage::AdminReportPage(AccountManager* am,
                                 BookingManager* bm,
                                 ReportManager* rm,
                                 QWidget* parent)
    : QWidget(parent),
      accountManager_(am),
      bookingManager_(bm),
      reportManager_(rm),
      menuList_(nullptr),
      pagesStack_(nullptr),
      ticketsStartDate_(nullptr),
      ticketsEndDate_(nullptr),
      ticketsRefreshBtn_(nullptr),
      totalTicketsLabel_(nullptr),
      businessTicketsLabel_(nullptr),
      economyTicketsLabel_(nullptr),
      chartView_(nullptr),
      chart_(nullptr),
      chartSeries_(nullptr)
{
    setupUI();
    onTicketsReportRefresh();
}

AdminReportPage::~AdminReportPage()
{
    // Qt sẽ tự động dọn dẹp
}

void AdminReportPage::setupUI()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* leftMenu = createLeftMenu();
    mainLayout->addWidget(leftMenu);

    QWidget* rightPane = createRightPane();
    mainLayout->addWidget(rightPane, 1);

    connect(menuList_, &QListWidget::currentRowChanged,
            this, &AdminReportPage::onMenuSelectionChanged);
}

QWidget* AdminReportPage::createLeftMenu()
{
    auto* menuContainer = new QWidget(this);
    menuContainer->setFixedWidth(200);
    menuContainer->setStyleSheet("background-color: #EBF1F9;");

    auto* menuLayout = new QVBoxLayout(menuContainer);
    menuLayout->setContentsMargins(10, 10, 10, 10);
    
    auto* title = new QLabel("THỐNG KÊ", this);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #123B7A; padding-left: 5px;");
    menuLayout->addWidget(title);
    menuLayout->addSpacing(10);

    menuList_ = new QListWidget(this);
    menuList_->setStyleSheet(
        "QListWidget { border: none; background: transparent; outline: 0; }"
        "QListWidget::item { padding: 10px; border-radius: 4px; }"
        "QListWidget::item:selected { background: #5B86C6; color: white; }"
    );
    
    menuList_->addItem("Vé đã bán");
    menuList_->addItem("Doanh thu");
    menuList_->setCurrentRow(0);

    menuLayout->addWidget(menuList_);
    menuLayout->addStretch();

    return menuContainer;
}

QWidget* AdminReportPage::createRightPane()
{
    pagesStack_ = new QStackedWidget(this);
    
    QWidget* ticketsPage = createTicketsSoldPage();
    QWidget* revenuePage = createRevenuePage();

    pagesStack_->addWidget(ticketsPage);
    pagesStack_->addWidget(revenuePage);

    return pagesStack_;
}

QWidget* AdminReportPage::createTicketsSoldPage()
{
    auto* pageWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(pageWidget);
    layout->setAlignment(Qt::AlignTop);

    // --- 1. Hàng chọn ngày ---
    auto* dateLayout = new QHBoxLayout();
    dateLayout->setSpacing(10);
    dateLayout->setAlignment(Qt::AlignLeft);
    
    ticketsStartDate_ = new QDateEdit(QDate::currentDate().addDays(-30), this);
    ticketsStartDate_->setCalendarPopup(true);
    ticketsStartDate_->setFixedWidth(120);
    ticketsEndDate_ = new QDateEdit(QDate::currentDate(), this);
    ticketsEndDate_->setCalendarPopup(true);
    ticketsEndDate_->setFixedWidth(120);
    ticketsRefreshBtn_ = new QPushButton("Xem thống kê", this);
    
    dateLayout->addWidget(new QLabel("Thống kê theo:"));
    dateLayout->addWidget(new QLabel("Từ ngày:"));
    dateLayout->addWidget(ticketsStartDate_);
    dateLayout->addWidget(new QLabel("Đến ngày:"));
    dateLayout->addWidget(ticketsEndDate_);
    dateLayout->addWidget(ticketsRefreshBtn_);
    dateLayout->addStretch();
    layout->addLayout(dateLayout);
    layout->addSpacing(20);

    // --- 2. Hàng thống kê tổng (3 hộps) ---
    auto* summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(20);

    // Khởi tạo labels trước
    totalTicketsLabel_ = new QLabel("0", this);
    businessTicketsLabel_ = new QLabel("0", this);
    economyTicketsLabel_ = new QLabel("0", this);

    QFrame* box1 = createSummaryBox("TỔNG SỐ VÉ ĐÃ BÁN", totalTicketsLabel_);
    QFrame* box2 = createSummaryBox("SỐ VÉ THƯƠNG GIA", businessTicketsLabel_);
    QFrame* box3 = createSummaryBox("SỐ VÉ PHỔ THÔNG", economyTicketsLabel_);
    
    summaryLayout->addWidget(box1);
    summaryLayout->addWidget(box2);
    summaryLayout->addWidget(box3);
    layout->addLayout(summaryLayout);
    layout->addSpacing(20);

    // --- 3. Biểu đồ ---
    // Khởi tạo chart components
    chart_ = new QChart();
    chartSeries_ = new QBarSeries();
    chartView_ = new QChartView(chart_, this);
    
    chartView_->setRenderHint(QPainter::Antialiasing);
    chart_->addSeries(chartSeries_);
    
    // Tạo trục X
    auto* axisX = new QBarCategoryAxis();
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries_->attachAxis(axisX);
    
    // Tạo trục Y
    auto* axisY = new QValueAxis();
    axisY->setMin(0);
    axisY->setLabelFormat("%d");
    chart_->addAxis(axisY, Qt::AlignLeft);
    chartSeries_->attachAxis(axisY);

    chart_->legend()->setVisible(false);
    chart_->setTitle("Số vé đã bán theo Đại lý");
    
    layout->addWidget(chartView_);
    
    connect(ticketsRefreshBtn_, &QPushButton::clicked, this, &AdminReportPage::onTicketsReportRefresh);

    return pageWidget;
}

QWidget* AdminReportPage::createRevenuePage()
{
    auto* pageWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(pageWidget);
    auto* label = new QLabel("Giao diện Thống kê Doanh thu sẽ được phát triển sau.", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18px; color: #888;");
    layout->addWidget(label);
    return pageWidget;
}

QFrame* AdminReportPage::createSummaryBox(const QString& title, QLabel*& valueLabel)
{
    auto* box = new QFrame(this);
    box->setFrameShape(QFrame::StyledPanel);
    box->setStyleSheet(
        "QFrame { background: white; border: 1px solid #C9D6EB; border-radius: 8px; padding: 10px; }"
    );
    
    auto* boxLayout = new QVBoxLayout(box);
    
    auto* titleLabel = new QLabel(title, box);
    titleLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #5B86C6; background: transparent; border: none;");
    
    if (valueLabel == nullptr) {
        valueLabel = new QLabel("0", box);
    }
    valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #123B7A; background: transparent; border: none;");
    
    boxLayout->addWidget(titleLabel);
    boxLayout->addWidget(valueLabel);
    
    return box;
}

void AdminReportPage::onTicketsReportRefresh()
{
    auto start = ticketsStartDate_->date().toString("yyyy-MM-dd").toStdString();
    auto end = ticketsEndDate_->date().addDays(1).toString("yyyy-MM-dd").toStdString();

    int totalSold = reportManager_->getTicketsSoldInRange(start, end);
    totalTicketsLabel_->setText(QString::number(totalSold));
    
    businessTicketsLabel_->setText("N/A");
    economyTicketsLabel_->setText("N/A");

    updateChart(start, end);
}

void AdminReportPage::updateChart(const std::string& start, const std::string& end)
{
    // Xóa dữ liệu cũ
    chartSeries_->clear();
    
    auto* reports = reportManager_->generateAgentReportInRange(start, end);
    if (!reports) return;

    QStringList categories;
    auto* barSet = new QBarSet("Vé đã bán");

    int maxTickets = 0;

    for (int i = 0; i < reports->size(); ++i) {
        auto* r = (*reports)[i];
        if (!r) continue;
        
        *barSet << r->issuedTickets;
        categories << QString::fromStdString(r->agentName);
        
        if (r->issuedTickets > maxTickets) {
            maxTickets = r->issuedTickets;
        }
        
        delete r;
    }
    delete reports;

    chartSeries_->append(barSet);

    // Cập nhật trục X
    QBarCategoryAxis* axisX = qobject_cast<QBarCategoryAxis*>(chart_->axes(Qt::Horizontal).first());
    if (axisX) {
        axisX->clear();
        axisX->append(categories);
    }
    
    // Cập nhật trục Y
    QValueAxis* axisY = qobject_cast<QValueAxis*>(chart_->axes(Qt::Vertical).first());
    if (axisY) {
        axisY->setMax(maxTickets + (maxTickets / 5));
    }
}

void AdminReportPage::onMenuSelectionChanged(int index)
{
    if (index == 0) {
        onTicketsReportRefresh();
    }
}