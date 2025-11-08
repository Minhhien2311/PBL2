#include "AgentTicketsReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/Booking.h"

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
#include <QDate>
#include <QDateTime>
#include <QDebug> // Thêm thư viện này
#include <numeric> // Thêm thư viện này

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
    businessTicketsLabel_(nullptr),
    economyTicketsLabel_(nullptr),
    chartView_(nullptr),
    chart_(nullptr),
    chartSeries_(nullptr)
{
  setupUI();
  onRefreshClicked();
}

AgentTicketsReportPage::~AgentTicketsReportPage()
{
  // Qt sẽ tự động dọn dẹp
}

void AgentTicketsReportPage::setupUI()
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
  chart_->setTitle("Số vé đã bán theo Tháng");
  chart_->legend()->setVisible(false);

  chartView_ = new QChartView(chart_, this);
  chartView_->setRenderHint(QPainter::Antialiasing);

  layout->addWidget(chartView_);

  connect(refreshBtn_, &QPushButton::clicked, this, &AgentTicketsReportPage::onRefreshClicked);
}

QFrame* AgentTicketsReportPage::createSummaryBox(const QString& title, QLabel*& valueLabel)
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

void AgentTicketsReportPage::onRefreshClicked()
{
  auto start = startDate_->date().toString("dd/MM/yyyy").toStdString();
  // addDays(1) là không chính xác nếu dùng để lọc, vì nó bao gồm cả ngày hôm sau
  // Chúng ta nên dùng ngày cuối cùng mà người dùng chọn
  auto end = endDate_->date().toString("dd/MM/yyyy").toStdString();

  // Lấy dữ liệu thực từ ReportManager - cho agent hiện tại
  int totalSold = reportManager_->getAgentTicketsSoldInRange(start, end);
  int businessSold = reportManager_->getAgentBusinessTicketsInRange(start, end);
  int economySold = reportManager_->getAgentEconomyTicketsInRange(start, end);

  totalTicketsLabel_->setText(QString::number(totalSold));
  businessTicketsLabel_->setText(QString::number(businessSold));
  economyTicketsLabel_->setText(QString::number(economySold));

  updateChart();
}


// =================================================================
// === HÀM UPDATECHART ĐÃ ĐƯỢC TÁI CẤU TRÚC (REFACTOR) HOÀN TOÀN ===
// =================================================================
void AgentTicketsReportPage::updateChart()
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
  qDebug() << "=== Tickets Chart Data ===";
  qDebug() << "Fetching monthly tickets for year:" << currentYear;

  std::vector<int> monthlyTickets = reportManager_->getAgentMonthlyTickets(currentYear);

  // (Toàn bộ vòng lặp for, QDebug, parse ngày... đã được XÓA khỏi đây)
  // --- KẾT THÚC TÁI CẤU TRÚC ---

  // 2. Chỉ tập trung vào VẼ DỮ LIỆU
  auto* barSet = new QBarSet("Số vé");
  barSet->setColor(QColor(91, 134, 198));
  barSet->setBorderColor(Qt::darkBlue);

  QStringList categories;
  categories << "Tháng 1" << "Tháng 2" << "Tháng 3" << "Tháng 4" << "Tháng 5" << "Tháng 6" 
        << "Tháng 7" << "Tháng 8" << "Tháng 9" << "Tháng 10" << "Tháng 11" << "Tháng 12";

  int maxTickets = 0;
  int totalTickets = 0;

  qDebug() << "=== Monthly Tickets (from ReportManager) ===";
  for (int i = 0; i < 12; ++i) {
    int tickets = monthlyTickets[i];
    qDebug() << categories[i] << ":" << tickets << "vé";

    if (tickets > maxTickets) {
      maxTickets = tickets;
    }
    totalTickets += tickets;
  }

  int yAxisMax = 10; // Mặc định trục Y

  // Logic dữ liệu giả
  if (totalTickets == 0) {
    qDebug() << "No tickets data, using dummy data for display";
    for (int i = 0; i < 12; ++i) {
      *barSet << 1; // Thêm 12 giá trị giả
    }
    yAxisMax = 10; // Đặt trục Y cho dữ liệu giả
  } else {
    qDebug() << "Using real tickets data";
    for (int tickets : monthlyTickets) {
      *barSet << tickets; // Thêm 12 giá trị thật
    }
    yAxisMax = maxTickets + 1; // Đặt trục Y cao hơn giá trị max một chút
  }

  chartSeries_->append(barSet);
  chart_->addSeries(chartSeries_);

  // 3. Cấu hình trục
  auto* axisX = new QBarCategoryAxis();
  axisX->append(categories);
  axisX->setTitleText("Tháng");
  chart_->addAxis(axisX, Qt::AlignBottom);
  chartSeries_->attachAxis(axisX);

  auto* axisY = new QValueAxis();
  axisY->setRange(0, yAxisMax); // Sử dụng giá trị yAxisMax đã tính
  axisY->setLabelFormat("%d");
  axisY->setTitleText("Số vé");
  chart_->addAxis(axisY, Qt::AlignLeft);
  chartSeries_->attachAxis(axisY);

  // 4. Cấu hình biểu đồ
  chart_->setTitle("Số vé đã bán theo Tháng - Năm " + QString::number(currentYear));
  chart_->setAnimationOptions(QChart::SeriesAnimations);
  chart_->legend()->setVisible(true);
  chart_->legend()->setAlignment(Qt::AlignBottom);

  // Đảm bảo biểu đồ được cập nhật
  chartView_->setChart(chart_);
  chartView_->update();

  qDebug() << "=== Tickets Chart Update Complete ===";
}