#pragma once

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QChartView>
#include <QChart>
#include <QBarSeries>

class AccountManager;
class BookingManager;
class ReportManager;

class AdminRevenueReportPage : public QWidget
{
    Q_OBJECT
public:
    explicit AdminRevenueReportPage(AccountManager* am,
                                    BookingManager* bm,
                                    ReportManager* rm,
                                    QWidget* parent = nullptr);
    ~AdminRevenueReportPage();
public slots:
    void refreshPage();
    
private:
    void setupUI();
    QFrame* createRevenueBox(const QString& title, const QString& value);
    void updateData();
    void updateChart();

    AccountManager* accountManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;

    QLabel* dailyRevenueLabel_;
    QLabel* weeklyRevenueLabel_;
    QLabel* monthlyRevenueLabel_;

    QChartView* chartView_;
    QChart* chart_;
    QBarSeries* chartSeries_;
};