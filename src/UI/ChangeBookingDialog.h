#ifndef CHANGEBOOKINGDIALOG_H
#define CHANGEBOOKINGDIALOG_H

#include <QDialog>
#include <QString>

// Forward declarations
class Booking;
class BookingManager;
class FlightManager;
class FlightInstance;
class QLineEdit;
class QTextEdit;
class QRadioButton;
class QPushButton;
class QLabel;
class QWidget;
class QGridLayout;

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

    // === Data members ===
    Booking* currentBooking_;
    BookingManager* bookingManager_;
    FlightManager* flightManager_;
    FlightInstance* currentFlightInstance_;
    FlightInstance* selectedNewFlight_;

    // === UI components ===
    QLineEdit* newFlightIdEdit_;
    QTextEdit* flightInfoText_;
    QRadioButton* economyRadio_;
    QRadioButton* businessRadio_;
    QPushButton* confirmBtn_;
    QLabel* priceDiffLabel_;
    
    // Seat map
    QWidget* seatMapContainer_;
    QGridLayout* seatMapLayout_;
    QString selectedSeatId_;
};

#endif // CHANGEBOOKINGDIALOG_H
