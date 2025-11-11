#ifndef CHANGEBOOKINGDIALOG_H
#define CHANGEBOOKINGDIALOG_H

#include <QDialog>

// Khai báo sớm các lớp dùng trong dialog
class QVBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QTextEdit;
class QRadioButton;
class QPushButton;
class QWidget;
class QGroupBox;
class QScrollArea;
class Booking;
class BookingManager;
class FlightManager;
class FlightInstance;

// Lớp chính hiển thị và xử lý logic đổi vé
class ChangeBookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeBookingDialog(Booking* currentBooking,
                                 BookingManager* bookingManager,
                                 FlightManager* flightManager,
                                 QWidget *parent = nullptr);

private slots:
    void onViewFlightInfoClicked();
    void onClassChanged();
    void onConfirmClicked();

private:
    // ==== HÀM HỖ TRỢ ====
    void setupUi();                // Khởi tạo giao diện
    void renderSeatMap();          // Vẽ sơ đồ ghế
    void updatePriceDifference();  // Cập nhật giá chênh lệch

    // ==== DỮ LIỆU ====
    Booking* currentBooking_;              // Vé hiện tại
    BookingManager* bookingManager_;       // Quản lý đặt chỗ
    FlightManager* flightManager_;         // Quản lý chuyến bay

    FlightInstance* currentFlightInstance_; // Chuyến bay hiện tại
    FlightInstance* selectedNewFlight_;     // Chuyến bay mới được chọn

    QString selectedSeatId_;               // ID ghế được chọn mới

    // ==== WIDGETS ====
    QLineEdit* newFlightIdEdit_;           // Nhập mã chuyến bay mới
    QTextEdit* flightInfoText_;            // Hiển thị thông tin chuyến bay
    QRadioButton* economyRadio_;           // Nút chọn hạng phổ thông
    QRadioButton* businessRadio_;          // Nút chọn hạng thương gia
    QWidget* seatMapContainer_;            // Vùng chứa sơ đồ ghế
    QGridLayout* seatMapLayout_;           // Layout cho sơ đồ ghế
    QLabel* priceDiffLabel_;               // Hiển thị giá chênh lệch
    QPushButton* confirmBtn_;              // Nút xác nhận đổi vé
};

#endif // CHANGEBOOKINGDIALOG_H
