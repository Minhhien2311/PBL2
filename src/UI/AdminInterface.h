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
class PassengerManager;

class AdminTicketsReportPage;
class AdminRevenueReportPage;
class FlightRulesPage;
class PromotionsPage;
class AgentListPage;
class RoutesPage;
class FlightsPage;
class DashboardPage;
class AccountsPage;
class AdminBookingPage;

class AdminInterface : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInterface(AccountManager* accManager,
                           FlightManager* flManager,
                           BookingManager* bkManager,
                           ReportManager* reportManager,
                           AirportManager* airportManager,
                           PassengerManager* passengerManager,
                           QWidget* parent = nullptr);

private slots:
    void setupConnections();

private:
    AccountManager* accountManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
    PassengerManager* passengerManager_;

    QFrame* sidebar_;
    QStackedWidget* stack_;

    // Các nút menu
    QPushButton* btnDashboard_;
    QPushButton* btnRoutes_;
    QPushButton* btnFlights_;
    QPushButton* btnBookings_;
    QPushButton* btnFlightRules_;    // Mới: Quản lý luật bay
    QPushButton* btnPromotions_;     // Mới: Quản lý khuyến mãi
    QPushButton* btnTicketsReport_;  // Mới: Vé đã bán
    QPushButton* btnRevenueReport_;  // Mới: Doanh thu
    QPushButton* btnAccounts_;
    QPushButton* btnAgentList_;      // Mới: Danh sách đại lý
    QPushButton* logoutBtn_;

    // Các trang
    DashboardPage* dashboardPage_;
    RoutesPage* routesPage_;
    FlightsPage* flightsPage_;
    AdminBookingPage* bookingPage_;             // Mới
    AccountsPage* accountsPage_;
    AdminTicketsReportPage* ticketsReportPage_;  // Mới
    AdminRevenueReportPage* revenueReportPage_;  // Mới
    FlightRulesPage* flightRulesPage_;            // Mới
    PromotionsPage* promotionsPage_;              // Mới
    AgentListPage* agentListPage_;                // Mới

signals:
    void logoutClicked();
};

#endif // ADMININTERFACE_H
