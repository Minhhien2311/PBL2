#ifndef AGENTBOOKINGSPAGE_H
#define AGENTBOOKINGSPAGE_H

#include <QWidget>

// Khai báo sớm
class BookingManager;
class FlightManager;  // <--- THÊM MỚI
class AccountManager; // <--- THÊM MỚI
class QTableView;
class QStandardItemModel;
class QLineEdit;      // <--- THÊM MỚI
class QDateEdit;      // <--- THÊM MỚI
class QPushButton;    // <--- THÊM MỚI

class AgentBookingsPage : public QWidget
{
    Q_OBJECT

public:
    // <--- CẬP NHẬT CONSTRUCTOR: Nhận thêm 2 manager
    explicit AgentBookingsPage(BookingManager* bkManager,
                               FlightManager* flManager,
                               AccountManager* accManager,
                               QWidget *parent = nullptr);

private slots:
    void onSearchClicked(); // Tìm/Làm mới
    void onCancelBookingClicked(); // Hủy vé

private:
    void setupUi();
    void setupModel();
    void setupConnections();
    void refreshTable(); // Hàm tải dữ liệu

    // --- Managers ---
    BookingManager* bookingManager_;
    FlightManager* flightManager_;   // <--- THÊM MỚI
    AccountManager* accountManager_; // <--- THÊM MỚI

    // --- UI Components ---
    QLineEdit* bookingIdSearchEdit_; // Tìm theo Mã Đặt chỗ
    QDateEdit* dateSearchEdit_;      // Tìm theo Ngày đặt
    QPushButton* searchButton_;
    QPushButton* refreshButton_;

    QTableView* tableView_;
    QStandardItemModel* model_;
    
    QPushButton* cancelBookingBtn_; // Nút Hủy vé
};

#endif // AGENTBOOKINGSPAGE_H