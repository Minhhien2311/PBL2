#ifndef CHANGEBOOKINGDIALOG_H
#define CHANGEBOOKINGDIALOG_H

#include <QDialog>
#include "entities/Booking.h"

class QLineEdit;
class QComboBox;
class QLabel;
class QGridLayout;
class QWidget;
class QTextEdit;
class QRadioButton;
class QPushButton;
class FlightInstance;
class FlightManager;
class BookingManager;

/**
 * @brief Dialog để đổi vé máy bay
 */
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
    void setupUi();
    void renderSeatMap();
    void updatePriceDifference();
    
    Booking* currentBooking_;
    BookingManager* bookingManager_;
    FlightManager* flightManager_;
    
    // Current booking info
    FlightInstance* currentFlightInstance_;
    
    // New flight selection
    QLineEdit* newFlightIdEdit_;
    QTextEdit* flightInfoText_;
    FlightInstance* selectedNewFlight_;
    
    // Class selection
    QRadioButton* economyRadio_;
    QRadioButton* businessRadio_;
    
    // Seat map
    QWidget* seatMapContainer_;
    QGridLayout* seatMapLayout_;
    QString selectedSeatId_;
    
    // Price display
    QLabel* priceDiffLabel_;
    
    // Buttons
    QPushButton* confirmBtn_;
};

#endif // CHANGEBOOKINGDIALOG_H
