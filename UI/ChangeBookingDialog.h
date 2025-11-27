#ifndef CHANGEBOOKINGDIALOG_H
#define CHANGEBOOKINGDIALOG_H

#include <QDialog>
#include "entities/Booking.h" 

// Forward declarations
class QLineEdit;
class QLabel;
class QGridLayout;
class QWidget;
class QRadioButton;
class QButtonGroup;
class QStackedWidget;
class QPushButton;
class Flight;
class FlightManager;
class BookingManager;
class SeatManager;
class AirportManager;

class ChangeBookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeBookingDialog(Booking* currentBooking,
                                 BookingManager* bookingManager,
                                 FlightManager* flightManager,
                                 AirportManager* airportManager,
                                 QWidget *parent = nullptr);
    ~ChangeBookingDialog() override = default;

    // Constants cho style ghế ngồi (Cần thiết cho logic renderSeatMap trong .cpp)
    static const QString SEAT_BASE_STYLE;
    static const QString BUSINESS_AVAILABLE_STYLE;
    static const QString ECONOMY_AVAILABLE_STYLE;
    static const QString BOOKED_STYLE;
    static const QString LOCKED_STYLE;
    static const QString SELECTED_STYLE_SUFFIX;

private slots:
    void onSearchByIdClicked();      // Xử lý tìm kiếm chuyến bay (Step 1)
    void onSelectFoundFlight();      // Chuyển sang chọn ghế (Step 1 -> Step 2)
    void onBackToStep1();            // Quay lại tìm chuyến bay (Step 2 -> Step 1)
    void onClassChanged();           // Xử lý khi đổi hạng vé (Economy/Business)
    void onConfirmClicked();         // Xác nhận đổi vé cuối cùng

private:
    void setupUi();
    void setupConnections();
    
    // Các hàm setup giao diện chi tiết
    void setupProgressHeader();      // Header 1-2
    void setupPage1_FlightSearch();  // Trang 1: Tìm kiếm & Info
    void setupPage2_SeatSelection(); // Trang 2: Chọn ghế
    
    // Các hàm logic hiển thị
    void showStep(int step);         // Điều hướng giữa các bước
    void updateStepIndicator();      // Cập nhật thanh tiến trình
    void renderSeatMap();            // Vẽ sơ đồ ghế
    void updateFareDisplay();        // Cập nhật giá vé hiển thị
    void updatePriceDifference();    // Tính toán chênh lệch giá cũ/mới
    QString formatWithDot(int v) const; // Helper format tiền tệ

    // --- Data Members ---
    Booking* currentBooking_;
    BookingManager* bookingManager_;
    FlightManager* flightManager_;
    AirportManager* airportManager_;
    
    Flight* currentFlight_;
    Flight* selectedNewFlight_;

    int currentStep_;
    QString selectedSeatId_;

    // --- UI Members ---
    QStackedWidget* stackedWidget_;  // Quản lý chuyển trang

    // Header Widgets
    QLabel* step1Label_;
    QWidget* line1_;
    QLabel* step2Label_;

    // Page 1 Widgets (Flight Search)
    QWidget* step1Container_;
    QLineEdit* flightIdEdit_;
    QPushButton* searchBtn_;
    
    // Page 2 Widgets (Seat Selection)
    QWidget* step2Container_;
    QRadioButton* economyRadio_;
    QRadioButton* businessRadio_;
    QLabel* fareLabel_;
    QWidget* seatMapContainer_;
    QGridLayout* seatMapLayout_;
    QLabel* selectedSeatDisplayLabel_;
    QLabel* priceDiffLabel_;

    // Footer Buttons
    QPushButton* cancelBtn_;
    QPushButton* backBtn_;
    QPushButton* step1NextBtn_; // Nút "Trang sau" (ở Step 1)
    QPushButton* confirmBtn_;   // Nút "Xác nhận đổi vé" (ở Step 2)
};

#endif // CHANGEBOOKINGDIALOG_H