#include "AgentReportPage.h"
#include "core/ReportManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDate>

AgentReportPage::AgentReportPage(const std::string& agentId,
                                 ReportManager* rm,
                                 QWidget* parent)
    : QWidget(parent),
      agentId_(agentId),
      reportManager_(rm)
{
    setupUI();
    loadReport();
}

void AgentReportPage::setupUI()
{
    auto *layout = new QVBoxLayout(this);

    auto *dateLayout = new QHBoxLayout();
    startDate_ = new QDateEdit(QDate::currentDate(), this);
    startDate_->setCalendarPopup(true);
    endDate_ = new QDateEdit(QDate::currentDate(), this);
    endDate_->setCalendarPopup(true);
    refreshBtn_ = new QPushButton("Cập nhật", this);
    connect(refreshBtn_, &QPushButton::clicked, this, &AgentReportPage::onRefreshClicked);

    dateLayout->addWidget(new QLabel("Từ ngày:"));
    dateLayout->addWidget(startDate_);
    dateLayout->addWidget(new QLabel("Đến ngày:"));
    dateLayout->addWidget(endDate_);
    dateLayout->addWidget(refreshBtn_);
    layout->addLayout(dateLayout);

    table_ = new QTableWidget(this);
    table_->setColumnCount(4);
    table_->setHorizontalHeaderLabels({"Ngày","Tổng booking","Vé đã bán","Doanh thu"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(table_);
}

void AgentReportPage::loadReport()
{
    table_->setRowCount(0);
    std::string start = startDate_->date().toString("yyyy-MM-dd").toStdString();
    std::string end   = endDate_->date().toString("yyyy-MM-dd").toStdString();

    // Lấy tổng số booking + vé bán + doanh thu trong khoảng thời gian
    int sold = reportManager_->getTicketsSoldInRange(start,end);

    int row = table_->rowCount();
    table_->insertRow(row);
    table_->setItem(row,0,new QTableWidgetItem(QString::fromStdString(start + " → " + end)));
    table_->setItem(row,1,new QTableWidgetItem(QString::number(sold)));
    table_->setItem(row,2,new QTableWidgetItem(QString::number(reportManager_->getDailyTicketsSold(agentId_))));
    table_->setItem(row,3,new QTableWidgetItem(QString::number(reportManager_->getDailyRevenue(agentId_),'f',2)));
}

void AgentReportPage::onRefreshClicked()
{
    loadReport();
}

// #include "AgentReportPage.h"
// #include "core/ReportManager.h"
// #include "core/AccountManager.h"
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QHeaderView>
// #include <QDate>

// AgentReportPage::AgentReportPage(AccountManager* am,
//                                  ReportManager* rm,
//                                  QWidget* parent)
//     : QWidget(parent),
//       accountManager_(am),
//       reportManager_(rm)
// {
//     setupUI();
//     loadReport();
// }

// void AgentReportPage::setupUI()
// {
//     auto *layout = new QVBoxLayout(this);

//     auto *dateLayout = new QHBoxLayout();
//     startDate_ = new QDateEdit(QDate::currentDate(), this);
//     startDate_->setCalendarPopup(true);
//     endDate_ = new QDateEdit(QDate::currentDate(), this);
//     endDate_->setCalendarPopup(true);
//     refreshBtn_ = new QPushButton("Cập nhật", this);
//     connect(refreshBtn_, &QPushButton::clicked, this, &AgentReportPage::onRefreshClicked);

//     dateLayout->addWidget(new QLabel("Từ ngày:"));
//     dateLayout->addWidget(startDate_);
//     dateLayout->addWidget(new QLabel("Đến ngày:"));
//     dateLayout->addWidget(endDate_);
//     dateLayout->addWidget(refreshBtn_);
//     layout->addLayout(dateLayout);

//     table_ = new QTableWidget(this);
//     table_->setColumnCount(4);
//     table_->setHorizontalHeaderLabels({"Ngày","Tổng booking","Vé đã bán","Doanh thu"});
//     table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//     layout->addWidget(table_);
// }

// void AgentReportPage::loadReport()
// {
//     table_->setRowCount(0);

//     auto currentUser = accountManager_->getCurrentUser();
//     if (!currentUser) return;

//     std::string agentId = currentUser->getId();
//     std::string start = startDate_->date().toString("yyyy-MM-dd").toStdString();
//     std::string end   = endDate_->date().toString("yyyy-MM-dd").toStdString();

//     int sold = reportManager_->getTicketsSoldInRange(agentId, start, end);
//     double revenue = reportManager_->getRevenueInRange(agentId, start, end);

//     int row = table_->rowCount();
//     table_->insertRow(row);
//     table_->setItem(row,0,new QTableWidgetItem(QString::fromStdString(start + " → " + end)));
//     table_->setItem(row,1,new QTableWidgetItem(QString::number(sold)));
//     table_->setItem(row,2,new QTableWidgetItem(QString::number(reportManager_->getDailyTicketsSold(agentId))));
//     table_->setItem(row,3,new QTableWidgetItem(QString::number(revenue,'f',2)));
// }

// void AgentReportPage::onRefreshClicked()
// {
//     loadReport();
// }
