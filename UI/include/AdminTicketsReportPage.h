#pragma once

#include <QBarSeries>
#include <QChart>
#include <QChartView>
#include <QDate>
#include <QDateEdit>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

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
    QLabel* economyTicketsLabel_;
    QLabel* businessTicketsLabel_;
    QLabel* cancelledTicketsLabel_;
    QLabel* changedTicketsLabel_;

    QChartView* chartView_;
    QChart* chart_;
    QBarSeries* chartSeries_;
};