#include "AdminTicketsReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"

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
    
    connect(refreshBtn_, &QPushButton::clicked, this, &AdminTicketsReportPage::onRefreshClicked);
}

QFrame* AdminTicketsReportPage::createSummaryBox(const QString& title, QLabel*& valueLabel)
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

void AdminTicketsReportPage::onRefreshClicked()
{
    auto start = startDate_->date().toString("yyyy-MM-dd").toStdString();
    auto end = endDate_->date().addDays(1).toString("yyyy-MM-dd").toStdString();

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
    // Xóa dữ liệu cũ
    chartSeries_->clear();
    
    auto* reports = reportManager_->generateAgentReportInRange(start, end);
    if (!reports || reports->empty()) {
        // Nếu không có dữ liệu, hiển thị thông báo
        if (reports) delete reports;
        return;
    }

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
        axisY->setMax(maxTickets + (maxTickets / 5)); // Thêm 20% khoảng đệm
    }
}