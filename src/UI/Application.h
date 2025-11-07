#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>

// Khai báo sớm
class QStackedWidget;
class LoginPage;
class AdminInterface;
class AgentInterface;
class AccountManager; 
class FlightManager;
class BookingManager;
class ReportManager;
class AirportManager;

class Application : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor nhận các con trỏ manager
    explicit Application(AccountManager* accManager, 
                         FlightManager* flManager, 
                         BookingManager* bkManager,
                         ReportManager* reportManager,
                         AirportManager* airportManager,
                         QWidget *parent = nullptr); 
    ~Application();

private slots:
    void onLoginSuccess(const QString& role, const QString& token);
    void onLogout();

private:
    void setupUi();
    void setupConnections();

    QStackedWidget *stack_;

    // Các trang con cấp cao
    LoginPage *loginPage_;
    AdminInterface *adminInterface_;
    AgentInterface *agentInterface_;

    // Con trỏ tới các Manager (không sở hữu, chỉ tham chiếu)
    AccountManager* accountManager_; 
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
};

#endif // APPLICATION_H

