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

    static const QString SEAT_BASE_STYLE;
    static const QString BUSINESS_AVAILABLE_STYLE;
    static const QString ECONOMY_AVAILABLE_STYLE;
    static const QString BOOKED_STYLE;
    static const QString LOCKED_STYLE;
    static const QString SELECTED_STYLE_SUFFIX;

// ========== SLOTS ==========
private slots:
    void onSearchByIdClicked();
    void onSelectFoundFlight();
    void onBackToStep1();
    void onClassChanged();
    void onConfirmClicked();

// ========== HELPER FUNCTIONS ==========
private:
    void setupUi();
    void setupConnections();
    void setupProgressHeader();
    void setupPage1_FlightSearch();
    void setupPage2_SeatSelection();
    void showStep(int step);
    void updateStepIndicator();
    void renderSeatMap();
    void updateFareDisplay();
    void updatePriceDifference();
    QString formatWithDot(int v) const;

// ========== DATA MEMBERS ==========
    Booking* currentBooking_;
    BookingManager* bookingManager_;
    FlightManager* flightManager_;
    AirportManager* airportManager_;
    
    Flight* currentFlight_;
    Flight* selectedNewFlight_;

    int currentStep_;
    QString selectedSeatId_;

// ========== UI COMPONENTS ==========
    QStackedWidget* stackedWidget_;
    QLabel* step1Label_;
    QWidget* line1_;
    QLabel* step2Label_;
    QWidget* step1Container_;
    QLineEdit* flightIdEdit_;
    QPushButton* searchBtn_;
    QWidget* step2Container_;
    QRadioButton* economyRadio_;
    QRadioButton* businessRadio_;
    QLabel* fareLabel_;
    QWidget* seatMapContainer_;
    QGridLayout* seatMapLayout_;
    QLabel* selectedSeatDisplayLabel_;
    QLabel* priceDiffLabel_;
    QPushButton* cancelBtn_;
    QPushButton* backBtn_;
    QPushButton* step1NextBtn_;
    QPushButton* confirmBtn_;
};

#endif // CHANGEBOOKINGDIALOG_H