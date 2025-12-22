#include "AdminRevenueReportPage.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "PageRefresher.h"

#include <numeric>
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QDate>
#include <QDebug>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QValueAxis>
#include <QVBoxLayout>

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

    headerRow->addStretch();

    QPushButton* refreshButton = new QPushButton("Làm mới trang", topBar);
    refreshButton->setIcon(QIcon("C:/PBL2/assets/icons/reload.png"));
    refreshButton->setIconSize(QSize(14, 14));
    refreshButton->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #133e87;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   border: none;"
        "   text-align: left;"
        "   padding: 0px;"
        "}"
        "QPushButton:hover {"
        "   background: transparent;"
        "   text-decoration: underline;"
        "}"
    );
    refreshButton->setCursor(Qt::PointingHandCursor);
    headerRow->addWidget(refreshButton);
    topBarLayout->addLayout(headerRow);

    QHBoxLayout* statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    QFrame* box1 = createRevenueBox("TRONG NGÀY", "0 triệu VNĐ");
    QFrame* box2 = createRevenueBox("TRONG TUẦN", "0 triệu VNĐ");
    QFrame* box3 = createRevenueBox("TRONG THÁNG", "0 triệu VNĐ");

    statsLayout->addWidget(box1);
    statsLayout->addWidget(box2);
    statsLayout->addWidget(box3);

    topBarLayout->addLayout(statsLayout);
    mainLayout->addWidget(topBar);

    // ========== CHART HEADER ==========
    QWidget* chartHeader = new QWidget(this);
    auto* chartHeaderLayout = new QHBoxLayout(chartHeader);
    chartHeaderLayout->setContentsMargins(24, 6, 24, 0);
    chartHeaderLayout->setSpacing(0);

    QLabel* chartTitle = new QLabel("Doanh thu theo tháng (Toàn hệ thống)", chartHeader);
    chartTitle->setProperty("class", "SectionTitle");
    chartHeaderLayout->addWidget(chartTitle);
    chartHeaderLayout->addStretch();

    mainLayout->addWidget(chartHeader);

    // ========== CHART CONTAINER ==========
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

    chartFrameLayout->addWidget(chartView_);
    chartContainerLayout->addWidget(chartFrame);
    
    mainLayout->addWidget(chartContainer, 1);

    connect(refreshButton, &QPushButton::clicked, this, &AdminRevenueReportPage::refreshPage);
}

QFrame* AdminRevenueReportPage::createRevenueBox(const QString& title, const QString& value)
{
    QFrame* card = new QFrame(this);
    card->setObjectName("StatCard");
    card->setFixedHeight(90);
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

    QLabel* valueLabel = new QLabel(value, body);
    valueLabel->setStyleSheet("color:#0E3B7C; font-size:20px; font-weight:700;");
    valueLabel->setAlignment(Qt::AlignCenter);
    bodyLayout->addWidget(valueLabel, 0, Qt::AlignCenter);

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
    double todayRevenue = reportManager_->getTodayRevenue();
    double weekRevenue = reportManager_->getThisWeekRevenue();
    double monthRevenue = reportManager_->getThisMonthRevenue();

    // Chuyển đổi sang triệu VNĐ
    todayRevenue /= 1000000;
    weekRevenue /= 1000000;
    monthRevenue /= 1000000;

    if (dailyRevenueLabel_) {
    dailyRevenueLabel_->setText(QString::number(todayRevenue, 'f', 1) + " triệu VNĐ");
    }
    if (weeklyRevenueLabel_) {
    weeklyRevenueLabel_->setText(QString::number(weekRevenue, 'f', 1) + " triệu VNĐ");
    }
    if (monthlyRevenueLabel_) {
    monthlyRevenueLabel_->setText(QString::number(monthRevenue, 'f', 1) + " triệu VNĐ");
    }

    updateChart();
}

void AdminRevenueReportPage::updateChart()
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
    qDebug() << "=== Admin Revenue Chart Data ===";
    qDebug() << "Fetching total monthly revenue for year:" << currentYear;

    std::vector<double> monthlyRevenue = reportManager_->getMonthlyRevenue(currentYear);

    auto* barSet = new QBarSet("Doanh thu");
    barSet->setColor(QColor(91, 134, 198));
    barSet->setBorderColor(Qt::darkBlue);

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
    qDebug() << categories[i] << ":" << revenueInMillion << "triệu VNĐ";
    }

    double yAxisMax = 1.0;

    if (totalRevenue == 0.0) {
    qDebug() << "No revenue data, using dummy data for display";
    for (int i = 0; i < 12; ++i) {
        *barSet << 0.1;
    }
    yAxisMax = 1.0;
    } else {
    qDebug() << "Using real revenue data";
    for (double revenue : monthlyRevenue) {
        *barSet << (revenue / 1000000.0);
    }
    yAxisMax = maxRevenueInMillion * 1.2;
    }

    chartSeries_->append(barSet);
    chart_->addSeries(chartSeries_);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries_->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    axisY->setRange(0, yAxisMax);
    axisY->setLabelFormat("%.1f");
    axisY->setTitleText("Triệu VNĐ");
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
    chart_->setMargins(QMargins(6, 20, 20, 20));
    chartView_->setChart(chart_);
    chartView_->update();
}

void AdminRevenueReportPage::refreshPage() {
    PageRefresher::executeRefresh([this]() {
        updateData();
    });
}