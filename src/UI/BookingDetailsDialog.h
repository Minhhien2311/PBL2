#ifndef BOOKINGDETAILSDIALOG_H
#define BOOKINGDETAILSDIALOG_H

#include <QDialog>

class Booking;
class FlightManager;
class AccountManager;
class PassengerManager;
class QLabel;
class QGroupBox;
class QFrame;

class BookingDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDetailsDialog(Booking* booking, 
                                 FlightManager* flightManager,
                                 AccountManager* accountManager,
                                 PassengerManager* passengerManager,
                                 QWidget *parent = nullptr);

private:
    void setupUi();
    QGroupBox* createBookingInfoGroup();
    QGroupBox* createFlightInfoGroup();
    QGroupBox* createPassengerInfoGroup();
    QFrame* createSeparator();

private:
    Booking* booking_;
    FlightManager* flightManager_;
    AccountManager* accountManager_;
    PassengerManager* passengerManager_;
    
    QLabel* bookingIdLabel_;
    QLabel* bookingDateLabel_;
    QLabel* classLabel_;
    QLabel* fareLabel_;
    QLabel* statusLabel_;
};

#endif // BOOKINGDETAILSDIALOG_H
