#ifndef AGENTINTERFACE_H
#define AGENTINTERFACE_H

#include <QWidget>

// Khai báo sớm
class QListWidget;
class QStackedWidget;
class QPushButton;
class QFrame;
class AccountManager;
class FlightManager; // <--- CẬP NHẬT
class BookingManager;

// (Khai báo sớm các trang con MỚI)
class SearchBookPage;
class AgentBookingsPage;
class AccountsPage;

class AgentInterface : public QWidget
{
    Q_OBJECT

public:
    // <--- CẬP NHẬT: Thêm FlightManager
    explicit AgentInterface(AccountManager* accManager,
                            FlightManager* flManager,
                            BookingManager* bkManager,
                            QWidget *parent = nullptr);

signals:
    void logoutClicked();

private:
    void setupUi();
    void setupConnections();

    // --- UI Components ---
    QFrame* sidebar_;
    QListWidget* nav_;
    QPushButton* logoutBtn_;
    QStackedWidget* stack_;

    // --- Managers (Không sở hữu) ---
    AccountManager* accountManager_;
    FlightManager* flightManager_; // <--- CẬP NHẬT
    BookingManager* bookingManager_;
};

#endif // AGENTINTERFACE_H