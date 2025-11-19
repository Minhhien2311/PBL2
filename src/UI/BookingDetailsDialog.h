#ifndef BOOKINGDETAILSDIALOG_H
#define BOOKINGDETAILSDIALOG_H

#include <QDialog>
#include <QLayout>

class Booking;
class FlightManager;
class AccountManager;
class PassengerManager;
class QLabel;
class QFrame;
class QGridLayout;

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
    QFrame* createInfoFrame(const QString& title, QLayout* contentLayout);
    QGridLayout* createBookingInfoLayout();
    QGridLayout* createFlightInfoLayout();
    QGridLayout* createPassengerInfoLayout();
    void addInfoRow(QGridLayout* layout, int row, 
                   const QString& labelText, 
                   const QString& valueText, 
                   bool valueBold = false,
                   const QString& valueColor = "");

private:
    Booking* booking_;
    FlightManager* flightManager_;
    AccountManager* accountManager_;
    PassengerManager* passengerManager_;
};

#endif // BOOKINGDETAILSDIALOG_H