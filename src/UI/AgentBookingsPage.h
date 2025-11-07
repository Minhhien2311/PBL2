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
    void onSearchClicked(); // Tìm/Làm mới
    void onCancelBookingClicked(); // Hủy vé
    void onViewDetailsClicked(); // Xem chi tiết vé

private:
    void setupUi();
    void setupModel();
    void setupConnections();
    void refreshTable(); // Hàm tải dữ liệu

    // --- Managers ---
    BookingManager* bookingManager_;
    FlightManager* flightManager_;   // <--- THÊM MỚI
    AccountManager* accountManager_; // <--- THÊM MỚI
    AirportManager* airportManager_;

    // --- UI Components ---
    QLineEdit* bookingIdSearchEdit_; // Tìm theo Mã Đặt chỗ
    AirportComboBox* fromSearchCombo_;
    AirportComboBox* toSearchCombo_;
    QDateEdit* dateSearchEdit_;      // Tìm theo Ngày đặt
    QPushButton* searchButton_;
    QPushButton* refreshButton_;

    QTableView* tableView_;
    QStandardItemModel* model_;
    
    QPushButton* cancelBookingBtn_; // Nút Hủy vé
    QPushButton* viewDetailsBtn_;   // Nút Xem chi tiết
};

#endif // AGENTBOOKINGSPAGE_H