#ifndef ADMINBOOKINGPAGE_H
#define ADMINBOOKINGPAGE_H

#include <QWidget>

// Khai báo sớm
class BookingManager;
class FlightManager;  
class AccountManager; 
class AirportManager;
class PassengerManager;  // Thêm dòng này
class QTableView;
class QStandardItemModel;
class QLineEdit;      
class QDateEdit;      
class QPushButton;    
class AirportComboBox;
class Booking;
class QLabel;

class AdminBookingPage : public QWidget
{
    Q_OBJECT

public:
    // Thêm PassengerManager vào constructor
    explicit AdminBookingPage(BookingManager* bkManager,
                               FlightManager* flManager,
                               AccountManager* accManager,
                               AirportManager* airportManager,
                               PassengerManager* passengerManager,  // Thêm tham số mới
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
    FlightManager* flightManager_;   
    AccountManager* accountManager_; 
    AirportManager* airportManager_;
    PassengerManager* passengerManager_;  // Thêm thành viên mới

    // --- UI Components ---
    QLineEdit* bookingIdSearchEdit_;   // Dòng tìm theo Mã Đặt chỗ
    QLineEdit* passengerIdSearchEdit_; // Dòng tìm theo CCCD hành khách
    QPushButton* searchButton_;        // Nút tìm theo Mã đặt chỗ
    QPushButton* searchByPassengerBtn_;// Nút tìm theo CCCD
    QPushButton* refreshButton;

    QTableView* tableView_;
    QStandardItemModel* model_;
    QLabel* statusLabel_;
    
    QPushButton* cancelBookingBtn_; // Nút Hủy vé
    QPushButton* viewDetailsBtn_;   // Nút Xem chi tiết
    QPushButton* changeBookingBtn_; // Nút Đổi vé
};

#endif // ADMINBOOKINGPAGE_H