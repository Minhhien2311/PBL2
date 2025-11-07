#pragma once

#include <QWidget>
#include <QDate>
#include <QFrame>
#include <QLabel>
#include <QDateEdit>
#include <QPushButton>
#include <QChartView>
#include <QChart>
#include <QBarSeries>

class AccountManager;
class BookingManager;
class ReportManager;

class AdminTicketsReportPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminTicketsReportPage(AccountManager* am,
                                    BookingManager* bm,
                                    ReportManager* rm,
                                    QWidget* parent = nullptr);
    ~AdminTicketsReportPage();

private slots:
    void onRefreshClicked();

private:
    void setupUI();
    QFrame* createSummaryBox(const QString& title, QLabel*& valueLabel);
    void updateChart(const std::string& start, const std::string& end);

    AccountManager* accountManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;

    QDateEdit* startDate_;
    QDateEdit* endDate_;
    QPushButton* refreshBtn_;
    QLabel* totalTicketsLabel_;
    QLabel* businessTicketsLabel_;
    QLabel* economyTicketsLabel_;
    
    QChartView* chartView_;
    QChart* chart_;
    QBarSeries* chartSeries_;
};