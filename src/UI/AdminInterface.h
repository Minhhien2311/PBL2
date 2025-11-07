#ifndef ADMININTERFACE_H
#define ADMININTERFACE_H

#include <QWidget>

class QStackedWidget;
class QPushButton;
class QFrame;
class QLabel;
class AccountManager;
class FlightManager;
class BookingManager;
class ReportManager;
class AirportManager;

class AdminTicketsReportPage;
class AdminRevenueReportPage;

class AdminInterface : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInterface(AccountManager* accManager,
                           FlightManager* flManager,
                           BookingManager* bkManager,
                           ReportManager* reportManager,
                           AirportManager* airportManager,
                           QWidget* parent = nullptr);

private slots:
    void setupConnections();

private:
    AccountManager* accountManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;

    QFrame* sidebar_;
    QStackedWidget* stack_;

    // Các nút menu
    QPushButton* btnDashboard_;
    QPushButton* btnRoutes_;
    QPushButton* btnFlights_;
    QPushButton* btnFlightRules_;    // Mới: Quản lý luật bay
    QPushButton* btnPromotions_;     // Mới: Quản lý khuyến mãi
    QPushButton* btnTicketsReport_;  // Mới: Vé đã bán
    QPushButton* btnRevenueReport_;  // Mới: Doanh thu
    QPushButton* btnAccounts_;
    QPushButton* btnAgentList_;      // Mới: Danh sách đại lý
    QPushButton* logoutBtn_;

    // Các trang
    // DashboardPage* dashboardPage_;
    // RoutesPage* routesPage_;
    // FlightsPage* flightsPage_;
    // AccountsPage* accountsPage_;
    // AdminTicketsReportPage* ticketsReportPage_;  // Mới
    // AdminRevenueReportPage* revenueReportPage_;  // Mới

signals:
    void logoutClicked();
};

#endif // ADMININTERFACE_H
