#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>
#include <QPushButton>

class QTableView;
class QStandardItemModel;
class QLabel;
class AccountManager;
class ReportManager;
class FlightManager;
class BookingManager;
class AirportManager;

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(AccountManager* accManager, ReportManager* reportManager, 
                          FlightManager* flightManager, BookingManager* bookingManager,
                          AirportManager* airportManager, QWidget *parent = nullptr);

public slots:
    void refreshData();
    void refreshPage();
private:
    void setupUi();
    void setupModel();

    AccountManager* accountManager_;
    ReportManager* reportManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    AirportManager* airportManager_;
    
    QWidget* createStatCard(const QString& title, const QString& defaultValue, QLabel*& outValueLabel);

    QLabel *salesCountLabel_;
    QLabel *salesTotalLabel_;
    QLabel *cancelCountLabel_;
    QLabel *doVeLabel_;

    QTableView *flightsTable_;
    QStandardItemModel *flightsModel_;
    QPushButton* refreshBtn_;
};

#endif // DASHBOARDPAGE_H

