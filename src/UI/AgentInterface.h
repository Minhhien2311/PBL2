#ifndef AGENTINTERFACE_H
#define AGENTINTERFACE_H

#include <QWidget>

// Khai báo sớm
class QStackedWidget;
class QPushButton;
class QFrame;
class AccountManager;
class FlightManager;
class BookingManager;
class ReportManager;
class PassengerManager;
class AirportManager;
class AgentTicketsReportPage;  // Thêm
class AgentRevenueReportPage;  // Thêm

// Khai báo sớm các trang con
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

private:
    void setupUi();
    void setupConnections();

private slots:
    void onUserChanged();      // Handle user login/logout
    void onPageChanged(int index);  // Handle page switching

private:
    // --- UI Components ---
    QFrame* sidebar_;
    QStackedWidget* stack_;
    QPushButton* logoutBtn_;

    // Nút sidebar
    QPushButton* btnDashboard_;
    QPushButton* btnSearchBook_;
    QPushButton* btnBookings_;
    QPushButton* btnAccounts_;
    QPushButton* btnTicketsReport_;  // Thêm
    QPushButton* btnRevenueReport_;  // Thêm

    // --- Page references for refreshing ---
    DashboardPage* dashboardPage_;
    SearchBookPage* searchBookPage_;
    AgentBookingsPage* agentBookingsPage_;
    AgentReportPage* agentReportPage_;
    AgentTicketsReportPage* ticketsReportPage_;  // Thêm
    AgentRevenueReportPage* revenueReportPage_;  // Thêm

    // --- Managers (không sở hữu) ---
    AccountManager* accountManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
    PassengerManager* passengerManager_;
};

#endif // AGENTINTERFACE_H