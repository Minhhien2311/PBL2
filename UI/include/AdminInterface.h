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
class AdminBookingsPage;

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

// ========== PRIVATE MEMBERS ==========

private:
    AccountManager* accountManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
    PassengerManager* passengerManager_;

    QFrame* sidebar_;
    QStackedWidget* stack_;

    QPushButton* btnDashboard_;
    QPushButton* btnRoutes_;
    QPushButton* btnFlights_;
    QPushButton* btnBookings_;
    QPushButton* btnFlightRules_;
    QPushButton* btnPromotions_;
    QPushButton* btnTicketsReport_;
    QPushButton* btnRevenueReport_;
    QPushButton* btnAccounts_;
    QPushButton* btnAgentList_;
    QPushButton* logoutBtn_;

    DashboardPage* dashboardPage_;
    RoutesPage* routesPage_;
    FlightsPage* flightsPage_;
    AdminBookingsPage* bookingPage_;
    AccountsPage* accountsPage_;
    AdminTicketsReportPage* ticketsReportPage_;
    AdminRevenueReportPage* revenueReportPage_;
    FlightRulesPage* flightRulesPage_;
    PromotionsPage* promotionsPage_;
    AgentListPage* agentListPage_;

    QLabel* userNameLabel_;

signals:
    void logoutClicked();
    void onUserChanged();
};

#endif // ADMININTERFACE_H
