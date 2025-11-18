#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H

#include <QDialog>

#include "entities/Booking.h"

class QLineEdit;
class QComboBox;
class QLabel;
class QGridLayout;
class QWidget;
class QRadioButton;
class QButtonGroup;
class Flight;
class FlightManager;
class BookingManager;
class AccountManager;
class PassengerManager;

class BookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDialog(Flight* flightInstance, 
                          FlightManager* flightManager,
                          BookingManager* bookingManager,
                          AccountManager* accountManager,
                          PassengerManager* passengerManager,
                          QWidget *parent = nullptr);
    
    // Lấy thông tin đã nhập
    QString getPassengerId() const;
    QString getPassengerName() const;
    QString getDateOfBirth() const;
    QString getPassengerPhone() const;
    BookingClass getSelectedClass() const;
    QString getSelectedSeatId() const;

private:
    void setupUi();
    void renderSeatMap();
    
    Flight* flight_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    AccountManager* accountManager_;
    PassengerManager* passengerManager_;
    
    // Form fields
    QLineEdit* passengerIdEdit_;      // CCCD/ID
    QLineEdit* passengerNameEdit_;    // Họ tên
    QLineEdit* dateOfBirthEdit_;       // Ngày sinh
    QLineEdit* passengerPhoneEdit_;   // Số điện thoại
    QRadioButton* economyRadio_;
    QRadioButton* businessRadio_;
    
    QLabel* fareLabel_;               // Hiển thị giá vé
    
    // Seat selection
    QWidget* seatMapContainer_;       // Container cho sơ đồ ghế
    QGridLayout* seatMapLayout_;      // Layout cho sơ đồ ghế
    QString selectedSeatId_;          // ID ghế đã chọn
};

#endif // BOOKINGDIALOG_H
