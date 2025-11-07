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

signals:
    void logoutClicked(); 

private:
    void setupConnections();

    // Sidebar
    QFrame* sidebar_;

    // Buttons cho các mục sidebar
    QPushButton* btnDashboard_;
    QPushButton* btnRoutes_;
    QPushButton* btnFlights_;
    QPushButton* btnAccounts_;
    QPushButton* logoutBtn_;

    // Stack chứa các trang
    QStackedWidget* stack_;

    // Con trỏ tới các Manager (không sở hữu)
    AccountManager* accountManager_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
};

#endif // ADMININTERFACE_H
