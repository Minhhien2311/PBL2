#ifndef ADMININTERFACE_H
#define ADMININTERFACE_H

#include <QWidget>

// Khai báo sớm
class QListWidget;
class QStackedWidget;
class QPushButton;
class QFrame;
class AccountManager;
class FlightManager; // <--- Sửa lỗi: Tên đúng (số ít)
class BookingManager;
class ReportManager;
class AirportManager;

class AdminInterface : public QWidget
{
    Q_OBJECT 

public:
    // <--- Sửa lỗi: Constructor phải khớp với file .cpp và Application.cpp
    explicit AdminInterface(AccountManager* accManager,
                            FlightManager* flManager, // <--- Sửa lỗi: Tên đúng (số ít)
                            BookingManager* bkManager,
                            ReportManager* reportManager,
                            AirportManager* airportManager,
                            QWidget* parent = nullptr);

signals:
    void logoutClicked(); 

private:
    void setupUi();
    void setupConnections();

    // Sidebar
    QFrame* sidebar_;
    QListWidget* nav_;
    QPushButton* logoutBtn_;

    // Router
    QStackedWidget* stack_;

    // Con trỏ tới các Manager (không sở hữu)
    AccountManager* accountManager_;
    FlightManager* flightManager_; // <--- Sửa lỗi: Tên đúng (số ít)
    BookingManager* bookingManager_;
    ReportManager* reportManager_;
    AirportManager* airportManager_;
};

#endif // ADMININTERFACE_H

