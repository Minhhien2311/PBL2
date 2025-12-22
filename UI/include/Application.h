#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>

class QStackedWidget;
class LoginPage;
class ForgotPasswordPage;
class AdminInterface;
class AgentInterface;
class AccountManager;
class FlightManager;
class BookingManager;
class ReportManager;
class AirportManager;
class PassengerManager;

class Application : public QMainWindow
{
    Q_OBJECT

public:
    explicit Application(AccountManager* accManager, 
                         FlightManager* flManager, 
                         BookingManager* bkManager,
                         ReportManager* reportManager,
                         AirportManager* airportManager,
                         PassengerManager* passengerManager,
                         QWidget *parent = nullptr);
    ~Application();

private slots:
    void onLoginSuccess(const QString& role, const QString& token);
    void onLogout();

private:
    void setupUi();
    void setupConnections();

    QStackedWidget *stack_;

    LoginPage *loginPage_;
    ForgotPasswordPage *forgotPasswordPage_;
    AdminInterface *adminInterface_;
    AgentInterface *agentInterface_;

    AccountManager* accountManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
    PassengerManager* passengerManager_;
};

#endif // APPLICATION_H