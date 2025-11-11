#include "AdminTicketsReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/AccountAgent.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
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
#include <QDebug>

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
    businessTicketsLabel_(nullptr),
    economyTicketsLabel_(nullptr),
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
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    // --- 1. Hàng chọn ngày ---
    // (Giữ nguyên, không thay đổi)
    auto* dateLayout = new QHBoxLayout();
    dateLayout->setSpacing(10);
    dateLayout->setAlignment(Qt::AlignLeft);

    startDate_ = new QDateEdit(QDate::currentDate().addDays(-30), this);
    startDate_->setCalendarPopup(true);
    startDate_->setFixedWidth(120);
    endDate_ = new QDateEdit(QDate::currentDate(), this);
    endDate_->setCalendarPopup(true);
    endDate_->setFixedWidth(120);
    refreshBtn_ = new QPushButton("Làm mới", this);

    dateLayout->addWidget(new QLabel("Thống kê theo:"));
    dateLayout->addWidget(new QLabel("Từ ngày:"));
    dateLayout->addWidget(startDate_);
    dateLayout->addWidget(new QLabel("Đến ngày:"));
    dateLayout->addWidget(endDate_);
    dateLayout->addWidget(refreshBtn_);
    dateLayout->addStretch();
    layout->addLayout(dateLayout);
    layout->addSpacing(20);

    // --- 2. Hàng thống kê tổng (3 hộp) ---
    // (Giữ nguyên, không thay đổi)
    auto* summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(20);

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
    // === SỬA: Chỉ tạo QChart và QChartView ===
    chart_ = new QChart();
    chart_->setTitle("Số vé đã bán theo Đại lý");

    chartView_ = new QChartView(chart_, this);
    chartView_->setRenderHint(QPainter::Antialiasing);

    // (Đã xóa code tạo series và trục ở đây)

    layout->addWidget(chartView_);

    connect(refreshBtn_, &QPushButton::clicked, this, &AdminTicketsReportPage::onRefreshClicked);
}

QFrame* AdminTicketsReportPage::createSummaryBox(const QString& title, QLabel*& valueLabel)
{
    // (Giữ nguyên, không thay đổi)
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

void AdminTicketsReportPage::onRefreshClicked()
{
    // === SỬA: Sửa lại logic lấy ngày cuối ===
    auto start = startDate_->date().toString("dd/MM/yyyy").toStdString();
    auto end = endDate_->date().toString("dd/MM/yyyy").toStdString(); // Không +1 ngày

    // Lấy dữ liệu thực từ ReportManager
    int totalSold = reportManager_->getTicketsSoldInRange(start, end);
    int businessSold = reportManager_->getBusinessTicketsInRange(start, end);
    int economySold = reportManager_->getEconomyTicketsInRange(start, end);

    totalTicketsLabel_->setText(QString::number(totalSold));
    businessTicketsLabel_->setText(QString::number(businessSold));
    economyTicketsLabel_->setText(QString::number(economySold));

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

    // 2. Tạo series mới
    chartSeries_ = new QBarSeries();

    // 3. Lấy dữ liệu
    auto* reports = reportManager_->generateAgentReportInRange(start, end);

    QStringList categories;
    auto* barSet = new QBarSet("Số vé");
    // === THÊM: Màu sắc ===
    barSet->setColor(QColor(91, 134, 198));
    barSet->setBorderColor(Qt::darkBlue);

    int maxTickets = 0;
    bool hasData = false;

    if (reports) {
    for (auto* r : *reports) {
        if (!r) continue;

        // Chỉ thêm agent có bán được vé
        if (r->issuedTickets > 0) {
        *barSet << r->issuedTickets;
        categories << QString::fromStdString(r->agentName);
        if (r->issuedTickets > maxTickets) maxTickets = r->issuedTickets;
        hasData = true;
        }
        delete r; // Dọn dẹp
    }
    delete reports; // Dọn dẹp
    }

    int yAxisMax = 10; // Trục Y mặc định

    // 4. Xử lý dữ liệu giả
    if (!hasData) {
    qDebug() << "No ticket data for any agent, using dummy data";
    *barSet << 1;
    categories << "Không có dữ liệu";
    yAxisMax = 10;
    } else {
    yAxisMax = maxTickets + 1; // Thêm 1 khoảng đệm
    }

    chartSeries_->append(barSet);
    chart_->addSeries(chartSeries_);

    // 5. Tạo trục X (MỚI)
    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Đại lý");
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries_->attachAxis(axisX);

    // 6. Tạo trục Y (MỚI)
    auto* axisY = new QValueAxis();
    axisY->setRange(0, yAxisMax);
    axisY->setLabelFormat("%d");
    axisY->setTitleText("Số vé");
    chart_->addAxis(axisY, Qt::AlignLeft);
    chartSeries_->attachAxis(axisY);

    // 7. Thêm hiệu ứng và legend
    chart_->setTitle("Số vé đã bán theo Đại lý");
    chart_->setAnimationOptions(QChart::SeriesAnimations);
    chart_->legend()->setVisible(true);
    chart_->legend()->setAlignment(Qt::AlignBottom);

    chartView_->setChart(chart_);
    chartView_->update();
}