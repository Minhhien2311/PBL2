#include "AgentTicketsReportPage.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "entities/Booking.h"

#include <numeric>
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDebug>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QValueAxis>
#include <QVBoxLayout>

AgentTicketsReportPage::AgentTicketsReportPage(AccountManager* am,
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
}

AgentTicketsReportPage::~AgentTicketsReportPage()
{
}

void AgentTicketsReportPage::setupUI()
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

    // ========== TOP BAR (CÓ LỀ 24PX) ==========
    QWidget* topBar = new QWidget(this);
    auto* topBarLayout = new QVBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 16, 24, 12);
    topBarLayout->setSpacing(12);

    QWidget* dateBox = new QWidget;
    QVBoxLayout* dateBoxLayout = new QVBoxLayout(dateBox);
    dateBoxLayout->setContentsMargins(10, 10, 10, 10);
    dateBoxLayout->setSpacing(6);
    
    dateBox->setStyleSheet(
        "QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }"
    );

    QLabel* dateBoxTitle = new QLabel("📅 Chọn khoảng thời gian cần thống kê");
    dateBoxTitle->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    dateBoxLayout->addWidget(dateBoxTitle);

    QHBoxLayout* dateRow = new QHBoxLayout();
    dateRow->setSpacing(12);

    QLabel* fromLabel = new QLabel("Từ ngày:");
    fromLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    dateRow->addWidget(fromLabel);

    startDate_ = new QDateEdit(QDate::currentDate().addDays(-30), dateBox);
    startDate_->setStyleSheet("background: white; border: 1px solid #133e87; border-radius: 4px; padding-left: 6px;");
    startDate_->setCalendarPopup(true);
    startDate_->setDisplayFormat("dd/MM/yyyy");
    startDate_->setMinimumHeight(36);
    startDate_->setMinimumWidth(160);
    dateRow->addWidget(startDate_, 1);

    QLabel* toLabel = new QLabel("Đến ngày:");
    toLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    dateRow->addWidget(toLabel);

    endDate_ = new QDateEdit(QDate::currentDate(), dateBox);
    endDate_->setStyleSheet("background: white; border: 1px solid #133e87; border-radius: 4px; padding-left: 6px;");
    endDate_->setCalendarPopup(true);
    endDate_->setDisplayFormat("dd/MM/yyyy");
    endDate_->setMinimumHeight(36);
    endDate_->setMinimumWidth(160);
    dateRow->addWidget(endDate_, 1);

    refreshBtn_ = new QPushButton("Truy vấn", dateBox);
    refreshBtn_->setMinimumHeight(36);
    refreshBtn_->setMinimumWidth(100);
    refreshBtn_->setMaximumWidth(150);
    refreshBtn_->setCursor(Qt::PointingHandCursor);
    refreshBtn_->setStyleSheet(
        "QPushButton { background:#4478BD; color:white; font-weight:600; "
        "border-radius:6px; padding: 0 16px; }"
        "QPushButton:hover { background:#365a9e; }"
    );
    dateRow->addWidget(refreshBtn_);

    dateBoxLayout->addLayout(dateRow);
    topBarLayout->addWidget(dateBox);

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

    QLabel* chartTitle = new QLabel("Số vé đã bán theo tháng", chartHeader);
    chartTitle->setProperty("class", "SectionTitle");
    chartHeaderLayout->addWidget(chartTitle);
    chartHeaderLayout->addStretch();

    mainLayout->addWidget(chartHeader);

    // ========== BIỂU ĐỒ TRONG KHUNG TRẮNG (KHÔNG CÓ SCROLL - GIỐNG REVENUE) ==========
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
    chartFrameLayout->addWidget(chartView_, 1);

    chartFrameLayout->addWidget(chartView_);
    chartContainerLayout->addWidget(chartFrame);
    
    mainLayout->addWidget(chartContainer, 1);

    connect(refreshBtn_, &QPushButton::clicked, this, &AgentTicketsReportPage::onRefreshClicked);
}

QFrame* AgentTicketsReportPage::createSummaryBox(const QString& title, QLabel*& valueLabel)
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

void AgentTicketsReportPage::onRefreshClicked()
{
  auto start = startDate_->date().toString("dd/MM/yyyy").toStdString();
  auto end = endDate_->date().toString("dd/MM/yyyy").toStdString();

  int totalSold = reportManager_->getAgentTicketsSoldInRange(start, end);
  int economySold = reportManager_->getAgentEconomyTicketsInRange(start, end);
  int businessSold = reportManager_->getAgentBusinessTicketsInRange(start, end);
  int cancelled = reportManager_->getAgentCancelledTicketsInRange(start, end);
  int changed = reportManager_->getAgentChangedTicketsInRange(start, end);

  totalTicketsLabel_->setText(QString::number(totalSold));
  economyTicketsLabel_->setText(QString::number(economySold));
  businessTicketsLabel_->setText(QString::number(businessSold));
  cancelledTicketsLabel_->setText(QString::number(cancelled));
  changedTicketsLabel_->setText(QString::number(changed));

  updateChart();
}

void AgentTicketsReportPage::updateChart()
{
    if (chart_) {
        chart_->removeAllSeries();
        for (QAbstractAxis* axis : chart_->axes()) {
            chart_->removeAxis(axis);
        }
    } else {
        chart_ = new QChart();
        chartView_->setChart(chart_);
    }

    chartSeries_ = new QBarSeries();

    int currentYear = QDate::currentDate().year();
    qDebug() << "=== Agent Tickets Chart Data ===";
    qDebug() << "Fetching monthly tickets for year:" << currentYear;

    std::vector<int> monthlyTickets = reportManager_->getAgentMonthlyTickets(currentYear);

    auto* barSet = new QBarSet("Số vé");
    barSet->setColor(QColor(91, 134, 198));
    barSet->setBorderColor(Qt::darkBlue);

    QStringList categories;
    categories << "Tháng 1" << "Tháng 2" << "Tháng 3" << "Tháng 4" << "Tháng 5" << "Tháng 6" 
        << "Tháng 7" << "Tháng 8" << "Tháng 9" << "Tháng 10" << "Tháng 11" << "Tháng 12";

    int maxTickets = 0;
    int totalTickets = std::accumulate(monthlyTickets.begin(), monthlyTickets.end(), 0);

    qDebug() << "=== Monthly Tickets (from ReportManager) ===";
    for (int i = 0; i < 12; ++i) {
        int tickets = monthlyTickets[i];
        if (tickets > maxTickets) {
            maxTickets = tickets;
        }
        qDebug() << categories[i] << ":" << tickets << "vé";
    }

    int yAxisMax = 10;

    if (totalTickets == 0) {
        qDebug() << "No tickets data, using dummy data for display";
        for (int i = 0; i < 12; ++i) {
            *barSet << 1;
        }
        yAxisMax = 10;
    } else {
        qDebug() << "Using real tickets data";
        for (int tickets : monthlyTickets) {
            *barSet << tickets;
        }
        yAxisMax = maxTickets * 1.2;
    }

    chartSeries_->append(barSet);
    chart_->addSeries(chartSeries_);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries_->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    axisY->setRange(0, yAxisMax);
    axisY->setLabelFormat("%d");
    axisY->setTitleText("Số vé");
    QFont titleFont;
    titleFont.setPointSize(10);
    titleFont.setBold(true);
    axisY->setTitleFont(titleFont);
    chart_->addAxis(axisY, Qt::AlignLeft);
    chartSeries_->attachAxis(axisY);

    chart_->setTitle("");
    chart_->setAnimationOptions(QChart::SeriesAnimations);
    chart_->legend()->setVisible(false);

    chart_->setBackgroundVisible(true);
    chart_->setBackgroundBrush(Qt::white);
    
    axisX->setGridLineVisible(false);
    axisY->setGridLineVisible(true);
    axisY->setGridLineColor(QColor(220, 220, 220));
    
    chart_->setMargins(QMargins(6, 20, 20, 40));
    chartView_->setChart(chart_);
    chartView_->update();

    qDebug() << "=== Agent Tickets Chart Update Complete ===";
}

void AgentTicketsReportPage::showEvent(QShowEvent *event)
{
    if (startDate_) {
        startDate_->setDate(QDate::currentDate().addDays(-30));
    }
    if (endDate_) {
        endDate_->setDate(QDate::currentDate());
    }
    
    onRefreshClicked();
    
    QWidget::showEvent(event);
}