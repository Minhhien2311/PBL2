#include "AdminReportPage.h"
#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDate>

AdminReportPage::AdminReportPage(AccountManager* am,
                                 BookingManager* bm,
                                 ReportManager* rm,
                                 QWidget* parent)
    : QWidget(parent),
      accountManager_(am),
      bookingManager_(bm),
      reportManager_(rm)
{
    setupUI();
    loadReport();
}

void AdminReportPage::setupUI()
{
    auto *layout = new QVBoxLayout(this);

    // Date range selection
    auto *dateLayout = new QHBoxLayout();
    startDate_ = new QDateEdit(QDate::currentDate(), this);
    startDate_->setCalendarPopup(true);
    endDate_ = new QDateEdit(QDate::currentDate(), this);
    endDate_->setCalendarPopup(true);
    refreshBtn_ = new QPushButton("Cập nhật", this);
    connect(refreshBtn_, &QPushButton::clicked, this, &AdminReportPage::onRefreshClicked);

    dateLayout->addWidget(new QLabel("Từ ngày:"));
    dateLayout->addWidget(startDate_);
    dateLayout->addWidget(new QLabel("Đến ngày:"));
    dateLayout->addWidget(endDate_);
    dateLayout->addWidget(refreshBtn_);
    layout->addLayout(dateLayout);

    // Table
    table_ = new QTableWidget(this);
    table_->setColumnCount(5);
    table_->setHorizontalHeaderLabels({"ID đại lý","Tên đại lý","Tổng booking","Vé đã bán","Doanh thu"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(table_);
}

void AdminReportPage::loadReport()
{
    table_->setRowCount(0);
    auto start = startDate_->date().toString("yyyy-MM-dd").toStdString();
    auto end   = endDate_->date().toString("yyyy-MM-dd").toStdString();

    auto* reports = reportManager_->generateAgentReportInRange(start, end);
    if (!reports) return;

    for (int i = 0; i < reports->size(); ++i) {
        auto* r = (*reports)[i];
        if (!r) continue;
        int row = table_->rowCount();
        table_->insertRow(row);
        table_->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r->agentId)));
        table_->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r->agentName)));
        table_->setItem(row, 2, new QTableWidgetItem(QString::number(r->totalBookings)));
        table_->setItem(row, 3, new QTableWidgetItem(QString::number(r->issuedTickets)));
        table_->setItem(row, 4, new QTableWidgetItem(QString::number(r->totalRevenue,'f',2)));
    }

    // Delete dynamically allocated AgentReport objects
    for (int i = 0; i < reports->size(); ++i) {
        delete (*reports)[i];
    }
    delete reports;
}

void AdminReportPage::onRefreshClicked()
{
    loadReport();
}
