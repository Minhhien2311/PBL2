#pragma once

#include <QWidget>
#include <QDate>
#include <QFrame>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QDateEdit>
#include <QPushButton>

// Forward declarations
class AccountManager;
class BookingManager;
class ReportManager;

// Thay đổi: Không dùng QtCharts namespace, include trực tiếp
#include <QChartView>
#include <QChart>
#include <QBarSeries>

class AdminReportPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminReportPage(AccountManager* am,
                             BookingManager* bm,
                             ReportManager* rm,
                             QWidget* parent = nullptr);
    ~AdminReportPage();

private slots:
    void onMenuSelectionChanged(int index);
    void onTicketsReportRefresh();

private:
    void setupUI();
    QWidget* createLeftMenu();
    QWidget* createRightPane();
    
    QWidget* createTicketsSoldPage();
    QWidget* createRevenuePage();

    QFrame* createSummaryBox(const QString& title, QLabel*& valueLabel);
    void updateChart(const std::string& start, const std::string& end);

    AccountManager* accountManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;

    QListWidget* menuList_;
    QStackedWidget* pagesStack_;

    // --- Thành phần của trang "Vé đã bán" ---
    QDateEdit* ticketsStartDate_;
    QDateEdit* ticketsEndDate_;
    QPushButton* ticketsRefreshBtn_;
    QLabel* totalTicketsLabel_;
    QLabel* businessTicketsLabel_;
    QLabel* economyTicketsLabel_;
    
    // Thành phần biểu đồ - KHÔNG dùng QtCharts::
    QChartView* chartView_;
    QChart* chart_;
    QBarSeries* chartSeries_;
};