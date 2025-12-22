#ifndef AGENTINTERFACE_H
#define AGENTINTERFACE_H

#include <QWidget>

class QStackedWidget;
class QPushButton;
class QFrame;
class QLabel;
class AccountManager;
class FlightManager;
class BookingManager;
class ReportManager;
class PassengerManager;
class AirportManager;
class AgentTicketsReportPage;
class AgentRevenueReportPage;

class SearchBookPage;
class AgentBookingsPage;
class AccountsPage;
class DashboardPage;
class AgentReportPage;

class AgentInterface : public QWidget
{
    Q_OBJECT

public:
    explicit AgentInterface(AccountManager* accManager,
                            FlightManager* flManager,
                            BookingManager* bkManager,
                            ReportManager* reportManager,
                            AirportManager* airportManager,
                            PassengerManager* passengerManager,
                            QWidget *parent = nullptr);

signals:
    void logoutClicked();

// ========== PRIVATE METHODS ==========

private:
    void setupUi();
    void setupConnections();

private slots:
    void onUserChanged();
    void onPageChanged(int index);

// ========== PRIVATE MEMBERS ==========

private:
    QFrame* sidebar_;
    QStackedWidget* stack_;
    QPushButton* logoutBtn_;

    QPushButton* btnDashboard_;
    QPushButton* btnSearchBook_;
    QPushButton* btnBookings_;
    QPushButton* btnAccounts_;
    QPushButton* btnTicketsReport_;
    QPushButton* btnRevenueReport_;

    DashboardPage* dashboardPage_;
    SearchBookPage* searchBookPage_;
    AgentBookingsPage* agentBookingsPage_;
    AgentReportPage* agentReportPage_;
    AgentTicketsReportPage* ticketsReportPage_;
    AgentRevenueReportPage* revenueReportPage_;

    AccountManager* accountManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
    PassengerManager* passengerManager_;

    QLabel* userNameLabel_;
};

#endif // AGENTINTERFACE_H