#ifndef AGENTBOOKINGSPAGE_H
#define AGENTBOOKINGSPAGE_H

#include <QWidget>

// Khai báo sớm
class BookingManager;
class FlightManager;  // <--- THÊM MỚI
class AccountManager; // <--- THÊM MỚI
class AirportManager;
class QTableView;
class QStandardItemModel;
class QLineEdit;      // <--- THÊM MỚI
class QDateEdit;      // <--- THÊM MỚI
class QPushButton;    // <--- THÊM MỚI
class AirportComboBox;
class Booking;

class AgentBookingsPage : public QWidget
{
    Q_OBJECT

public:
    // <--- CẬP NHẬT CONSTRUCTOR: Nhận thêm 2 manager
    explicit AgentBookingsPage(BookingManager* bkManager,
                               FlightManager* flManager,
                               AccountManager* accManager,
                               AirportManager* airportManager,
                               QWidget *parent = nullptr);

public slots:
    void refreshPage();  // New method to refresh when page shown or user changed

private slots:
    // Hàng tìm kiếm
    void onSearchByBookingId();
    void onSearchByPassengerId();
 
    void onCancelBookingClicked(); // Hủy vé
    void onViewDetailsClicked(); // Xem chi tiết vé
    void onChangeBookingClicked(); // Đổi vé

private:
    void setupUi();
    void setupModel();
    void setupConnections();
    void refreshTable(); // Hàm tải dữ liệu
    void displayBooking(Booking* booking);  // Helper để hiển thị một booking lên bảng

    // --- Managers ---
    BookingManager* bookingManager_;
    FlightManager* flightManager_;   // <--- THÊM MỚI
    AccountManager* accountManager_; // <--- THÊM MỚI
    AirportManager* airportManager_;

    // --- UI Components ---
    QLineEdit* bookingIdSearchEdit_;   // Dòng tìm theo Mã Đặt chỗ
    QLineEdit* passengerIdSearchEdit_; // Dòng tìm theo CCCD hành khách
    QPushButton* searchButton_;        // Nút tìm theo Mã đặt chỗ
    QPushButton* searchByPassengerBtn_;// Nút tìm theo CCCD
    QPushButton* refreshButton_;

    QTableView* tableView_;
    QStandardItemModel* model_;
    
    QPushButton* cancelBookingBtn_; // Nút Hủy vé
    QPushButton* viewDetailsBtn_;   // Nút Xem chi tiết
    QPushButton* changeBookingBtn_; // Nút Đổi vé
};

#endif // AGENTBOOKINGSPAGE_H