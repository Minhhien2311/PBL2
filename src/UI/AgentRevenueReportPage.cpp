// #include "AgentRevenueReportPage.h"
// #include "core/ReportManager.h"
// #include "core/AccountManager.h"
// #include "core/BookingManager.h"

// #include <QDate>   
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QLabel>
// #include <QFrame>
// #include <QPushButton>
// #include <QChartView>
// #include <QChart>
// #include <QBarSeries>
// #include <QBarSet>
// #include <QBarCategoryAxis>
// #include <QValueAxis>

// AgentRevenueReportPage::AgentRevenueReportPage(AccountManager* am,
//                                                BookingManager* bm,
//                                                ReportManager* rm,
//                                                QWidget* parent)
//     : QWidget(parent),
//       accountManager_(am),
//       bookingManager_(bm),
//       reportManager_(rm),
//       dailyRevenueLabel_(nullptr),
//       weeklyRevenueLabel_(nullptr),
//       monthlyRevenueLabel_(nullptr),
//       chartView_(nullptr),
//       chart_(nullptr),
//       chartSeries_(nullptr)
// {
//     setupUI();
//     updateData();
// }

// AgentRevenueReportPage::~AgentRevenueReportPage()
// {
//     // Qt sẽ tự động dọn dẹp
// }

// void AgentRevenueReportPage::setupUI()
// {
//     auto* layout = new QVBoxLayout(this);
//     layout->setAlignment(Qt::AlignTop);

//     // --- Nút làm mới ---
//     auto* refreshLayout = new QHBoxLayout();
//     refreshLayout->setAlignment(Qt::AlignLeft);
    
//     auto* refreshBtn = new QPushButton("Làm mới", this);
//     refreshLayout->addWidget(refreshBtn);
//     refreshLayout->addStretch();
    
//     layout->addLayout(refreshLayout);
//     layout->addSpacing(10);

//     // --- 1. Hàng thống kê tổng (3 hộp) ---
//     auto* summaryLayout = new QHBoxLayout();
//     summaryLayout->setSpacing(20);

//     // Tạo 3 ô doanh thu
//     QFrame* box1 = createRevenueBox("Trong ngày", "0 triệu VND");
//     QFrame* box2 = createRevenueBox("Trong tuần", "0 triệu VND");
//     QFrame* box3 = createRevenueBox("Trong tháng", "0 triệu VND");
    
//     summaryLayout->addWidget(box1);
//     summaryLayout->addWidget(box2);
//     summaryLayout->addWidget(box3);
//     layout->addLayout(summaryLayout);
//     layout->addSpacing(20);

//     // --- 2. Biểu đồ ---
//     chart_ = new QChart();
//     chartSeries_ = new QBarSeries();
//     chartView_ = new QChartView(chart_, this);
    
//     chartView_->setRenderHint(QPainter::Antialiasing);
//     chart_->addSeries(chartSeries_);
    
//     // Tạo trục X (các tháng)
//     auto* axisX = new QBarCategoryAxis();
//     chart_->addAxis(axisX, Qt::AlignBottom);
//     chartSeries_->attachAxis(axisX);
    
//     // Tạo trục Y (doanh thu)
//     auto* axisY = new QValueAxis();
//     axisY->setMin(0);
//     axisY->setLabelFormat("%.0f trieu VND");
//     chart_->addAxis(axisY, Qt::AlignLeft);
//     chartSeries_->attachAxis(axisY);

//     chart_->legend()->setVisible(false);
//     chart_->setTitle("Doanh thu theo tháng");
    
//     layout->addWidget(chartView_);

//     // Kết nối nút làm mới
//     connect(refreshBtn, &QPushButton::clicked, this, &AgentRevenueReportPage::updateData);
// }

// QFrame* AgentRevenueReportPage::createRevenueBox(const QString& title, const QString& value)
// {
//     auto* box = new QFrame(this);
//     box->setFrameShape(QFrame::StyledPanel);
//     box->setStyleSheet(
//         "QFrame { background: white; border: 1px solid #C9D6EB; border-radius: 8px; padding: 10px; }"
//     );
    
//     auto* boxLayout = new QVBoxLayout(box);
    
//     auto* titleLabel = new QLabel(title, box);
//     titleLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #5B86C6; background: transparent; border: none;");
    
//     auto* valueLabel = new QLabel(value, box);
//     valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #123B7A; background: transparent; border: none;");
    
//     boxLayout->addWidget(titleLabel);
//     boxLayout->addWidget(valueLabel);
    
//     // Lưu con trỏ đến label giá trị để cập nhật sau
//     if (title == "Trong ngày") {
//         dailyRevenueLabel_ = valueLabel;
//     } else if (title == "Trong tuần") {
//         weeklyRevenueLabel_ = valueLabel;
//     } else if (title == "Trong tháng") {
//         monthlyRevenueLabel_ = valueLabel;
//     }
    
//     return box;
// }

// void AgentRevenueReportPage::updateData()
// {
//     // Cập nhật dữ liệu thực từ ReportManager - cho agent hiện tại
//     double todayRevenue = reportManager_->getAgentTodayRevenue();
//     double weekRevenue = reportManager_->getAgentThisWeekRevenue();
//     double monthRevenue = reportManager_->getAgentThisMonthRevenue();

//     // Chuyển đổi sang triệu VND (giả sử đơn vị là VND)
//     todayRevenue /= 1000000;
//     weekRevenue /= 1000000;
//     monthRevenue /= 1000000;

//     // Cập nhật các label
//     if (dailyRevenueLabel_) {
//         dailyRevenueLabel_->setText(QString::number(todayRevenue, 'f', 1) + " triệu VND");
//     }
//     if (weeklyRevenueLabel_) {
//         weeklyRevenueLabel_->setText(QString::number(weekRevenue, 'f', 1) + " triệu VND");
//     }
//     if (monthlyRevenueLabel_) {
//         monthlyRevenueLabel_->setText(QString::number(monthRevenue, 'f', 1) + " triệu VND");
//     }

//     updateChart();
// }

// void AgentRevenueReportPage::updateChart()
// {
//     // Xóa dữ liệu cũ
//     chartSeries_->clear();

//     // Lấy dữ liệu doanh thu theo tháng từ ReportManager - cho agent hiện tại
//     int currentYear = QDate::currentDate().year();
//     std::vector<double> monthlyRevenue = reportManager_->getAgentMonthlyRevenue(currentYear);

//     QStringList categories;
//     categories << "JAN" << "FEB" << "MAR" << "APR" << "MAY" << "JUN" 
//                << "JUL" << "AUG" << "SEP" << "OCT" << "NOV" << "DEC";

//     auto* barSet = new QBarSet("Doanh thu");
    
//     double maxRevenue = 0;
//     for (int i = 0; i < 12; ++i) {
//         double revenueInMillion = monthlyRevenue[i] / 1000000; // Chuyển sang triệu VND
//         *barSet << revenueInMillion;
//         if (revenueInMillion > maxRevenue) {
//             maxRevenue = revenueInMillion;
//         }
//     }

//     chartSeries_->append(barSet);

//     // Cập nhật trục X
//     QBarCategoryAxis* axisX = qobject_cast<QBarCategoryAxis*>(chart_->axes(Qt::Horizontal).first());
//     if (axisX) {
//         axisX->clear();
//         axisX->append(categories);
//     }
    
//     // Cập nhật trục Y
//     QValueAxis* axisY = qobject_cast<QValueAxis*>(chart_->axes(Qt::Vertical).first());
//     if (axisY) {
//         axisY->setMax(maxRevenue + (maxRevenue * 0.2)); // Thêm 20% khoảng đệm
//     }
// }

#include "AgentRevenueReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/Booking.h"

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
    QFrame* box1 = createRevenueBox("Trong ngày", "0 triệu VND");
    QFrame* box2 = createRevenueBox("Trong tuần", "0 triệu VND");
    QFrame* box3 = createRevenueBox("Trong tháng", "0 triệu VND");
    
    summaryLayout->addWidget(box1);
    summaryLayout->addWidget(box2);
    summaryLayout->addWidget(box3);
    layout->addLayout(summaryLayout);
    layout->addSpacing(20);

    // --- 2. Biểu đồ ---
    chart_ = new QChart();
    chartSeries_ = new QBarSeries();
    chartView_ = new QChartView(chart_, this);
    
    chartView_->setRenderHint(QPainter::Antialiasing);
    chart_->addSeries(chartSeries_);
    
    // Tạo trục X (các tháng)
    auto* axisX = new QBarCategoryAxis();
    chart_->addAxis(axisX, Qt::AlignBottom);
    chartSeries_->attachAxis(axisX);
    
    // Tạo trục Y (doanh thu)
    auto* axisY = new QValueAxis();
    axisY->setMin(0);
    axisY->setLabelFormat("%.0f trieu VND");
    chart_->addAxis(axisY, Qt::AlignLeft);
    chartSeries_->attachAxis(axisY);

    chart_->legend()->setVisible(false);
    chart_->setTitle("Doanh thu theo tháng");
    
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
    if (title == "Trong ngày") {
        dailyRevenueLabel_ = valueLabel;
    } else if (title == "Trong tuần") {
        weeklyRevenueLabel_ = valueLabel;
    } else if (title == "Trong tháng") {
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

void AgentRevenueReportPage::updateChart()
{
    // Xóa dữ liệu cũ
    chartSeries_->clear();

    // Lấy dữ liệu doanh thu theo tháng từ booking thực tế
    std::vector<double> monthlyRevenue(12, 0.0);
    
    // Lấy danh sách booking của agent hiện tại
    std::string currentAgentId = accountManager_->getCurrentAgentId();
    std::vector<Booking*> agentBookings = bookingManager_->getBookingsByAgentId(currentAgentId);
    
    int currentYear = QDate::currentDate().year();
    
    for (Booking* booking : agentBookings) {
        if (booking && booking->getStatus() == BookingStatus::Issued) {
            // Lấy ngày booking và trích xuất tháng
            std::string bookingDate = booking->getBookingDate();
            // Giả sử định dạng ngày: "YYYY-MM-DD HH:MM:SS"
            if (bookingDate.length() >= 10) {
                int bookingYear = QString::fromStdString(bookingDate.substr(0, 4)).toInt();
                int bookingMonth = QString::fromStdString(bookingDate.substr(5, 2)).toInt();
                
                if (bookingYear == currentYear && bookingMonth >= 1 && bookingMonth <= 12) {
                    monthlyRevenue[bookingMonth - 1] += booking->getBaseFare();
                }
            }
        }
    }

    QStringList categories;
    categories << "JAN" << "FEB" << "MAR" << "APR" << "MAY" << "JUN" 
               << "JUL" << "AUG" << "SEP" << "OCT" << "NOV" << "DEC";

    auto* barSet = new QBarSet("Doanh thu");
    
    double maxRevenue = 0;
    for (int i = 0; i < 12; ++i) {
        double revenueInMillion = monthlyRevenue[i] / 1000000; // Chuyển sang triệu VND
        *barSet << revenueInMillion;
        if (revenueInMillion > maxRevenue) {
            maxRevenue = revenueInMillion;
        }
    }

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
        axisY->setMax(maxRevenue + (maxRevenue * 0.2)); // Thêm 20% khoảng đệm
    }
}