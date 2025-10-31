#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H

#include <QDialog>
#include "entities/Booking.h"

class QLineEdit;
class QComboBox;
class QLabel;
class FlightInstance;

/**
 * @brief Dialog để nhập thông tin hành khách và đặt vé
 */
class BookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDialog(FlightInstance* flightInstance, QWidget *parent = nullptr);
    
    // Lấy thông tin đã nhập
    QString getPassengerId() const;
    QString getPassengerName() const;
    QString getPassengerPhone() const;
    QString getPassportNumber() const;
    BookingClass getSelectedClass() const;

private:
    void setupUi();
    
    FlightInstance* flightInstance_;
    
    // Form fields
    QLineEdit* passengerIdEdit_;      // CCCD/ID
    QLineEdit* passengerNameEdit_;    // Họ tên
    QLineEdit* passengerPhoneEdit_;   // Số điện thoại
    QLineEdit* passportNumberEdit_;   // Số hộ chiếu
    QComboBox* classComboBox_;        // Hạng vé
    
    QLabel* fareLabel_;               // Hiển thị giá vé
};

#endif // BOOKINGDIALOG_H
