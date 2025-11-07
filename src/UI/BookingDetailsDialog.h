#ifndef BOOKINGDETAILSDIALOG_H
#define BOOKINGDETAILSDIALOG_H

#include <QDialog>

class QLabel;
class Booking;
class FlightManager;
class AccountManager;

// Dialog hiển thị chi tiết đầy đủ của một booking
class BookingDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDetailsDialog(Booking* booking, 
                                 FlightManager* flightManager,
                                 AccountManager* accountManager,
                                 QWidget *parent = nullptr);

private:
    void setupUi();
    
    Booking* booking_;
    FlightManager* flightManager_;
    AccountManager* accountManager_;
    
    // Các label hiển thị thông tin
    QLabel* bookingIdLabel_;
    QLabel* flightInfoLabel_;
    QLabel* passengerInfoLabel_;
    QLabel* bookingDateLabel_;
    QLabel* classLabel_;
    QLabel* fareLabel_;
    QLabel* statusLabel_;
};

#endif // BOOKINGDETAILSDIALOG_H
