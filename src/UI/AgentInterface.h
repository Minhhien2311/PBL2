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
class AirportManager;

// Khai báo sớm các trang con
class SearchBookPage;
class AgentBookingsPage;
class AccountsPage;
class DashboardPage;

class AgentInterface : public QWidget
{
    Q_OBJECT

public:
    explicit AgentInterface(AccountManager* accManager,
                            FlightManager* flManager,
                            BookingManager* bkManager,
                            ReportManager* reportManager,
                            AirportManager* airportManager,
                            QWidget *parent = nullptr);

signals:
    void logoutClicked();

private:
    void setupUi();
    void setupConnections();

private slots:
    void onUserChanged();      // Handle user login/logout
    void onPageChanged(int index);  // Handle page switching

    // --- UI Components ---
    QFrame* sidebar_;
    QStackedWidget* stack_;
    QPushButton* logoutBtn_;

    // Nút sidebar
    QPushButton* btnDashboard_;
    QPushButton* btnSearchBook_;
    QPushButton* btnBookings_;
    QPushButton* btnAccounts_;
    QPushButton* btnReport_;

    // --- Page references for refreshing ---
    DashboardPage* dashboardPage_;
    SearchBookPage* searchBookPage_;
    AgentBookingsPage* agentBookingsPage_;

    // --- Managers (không sở hữu) ---
    AccountManager* accountManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
};

#endif // AGENTINTERFACE_H
