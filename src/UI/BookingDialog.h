#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H

#include <QDialog>
#include "entities/Booking.h"

// Forward declarations
class QLineEdit;
class QComboBox;
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
    
    QString getPassengerId() const;
    QString getPassengerName() const;
    QString getDateOfBirth() const;
    QString getPassengerPhone() const;
    BookingClass getSelectedClass() const;
    QString getSelectedSeatId() const;

private slots:
    void onNextClicked();
    void onBackClicked();
    void onConfirmClicked();
    void onCancelClicked();
    void onClassChanged();

private:
    void setupUi();
    void setupProgressHeader();
    void setupPage1_FlightInfo();
    void setupPage2_PassengerInfo();
    void setupPage3_SeatSelection();
    
    void renderSeatMap();
    void updateStepIndicator();
    void updateFareDisplay();

    Flight* flight_;
    FlightManager* flightManager_;
    BookingManager* bookingManager_;
    AccountManager* accountManager_;
    PassengerManager* passengerManager_;
    
    QStackedWidget* stackedWidget_;
    
    QLabel* step1Label_;
    QLabel* step2Label_;
    QLabel* step3Label_;
    QWidget* line1_;
    QWidget* line2_;

    QPushButton* backBtn_;
    QPushButton* nextBtn_;
    QPushButton* confirmBtn_;
    QPushButton* cancelBtn_;

    QLineEdit* passengerIdEdit_;
    QLineEdit* passengerNameEdit_;
    QLineEdit* dateOfBirthEdit_;
    QLineEdit* passengerPhoneEdit_;
    
    QRadioButton* economyRadio_;
    QRadioButton* businessRadio_;
    QLabel* fareLabel_;
    QWidget* seatMapContainer_;
    QGridLayout* seatMapLayout_;
    QString selectedSeatId_;
    
    QLabel* selectedSeatDisplayLabel_;
};

#endif // BOOKINGDIALOG_H