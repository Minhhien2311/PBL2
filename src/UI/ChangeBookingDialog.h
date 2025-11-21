#ifndef CHANGEBOOKINGDIALOG_H
#define CHANGEBOOKINGDIALOG_H

#include <QDialog>

class Booking;
class BookingManager;
class FlightManager;
class Flight;
class SeatManager;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QRadioButton;
class QLabel;
class QWidget;
class QTableView;
class QStandardItemModel;
class QGridLayout;

class ChangeBookingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChangeBookingDialog(Booking* currentBooking,
                                 BookingManager* bookingManager,
                                 FlightManager* flightManager,
                                 QWidget *parent = nullptr);
    ~ChangeBookingDialog() override = default;

private slots:
    void onSearchByIdClicked();
    void onSelectFoundFlight();

    void showStep(int step);
    void onBackToStep1();

    void onClassChanged();
    void renderSeatMap();
    void updatePriceDifference();
    void onConfirmClicked();

private:
    void setupUi();
    void setupConnections();
    QString formatWithDot(int v) const;

    Booking* currentBooking_;
    BookingManager* bookingManager_;
    FlightManager* flightManager_;
    Flight* currentFlight_;
    Flight* selectedNewFlight_;

    int currentStep_;

    QWidget* step1Container_;
    QLineEdit* flightIdEdit_;
    QPushButton* searchBtn_;
    QStandardItemModel* resultModel_;
    QTableView* resultTable_;
    QPushButton* step1NextBtn_;

    QWidget* step2Container_;
    QTextEdit* flightInfoText_;
    QRadioButton* economyRadio_;
    QRadioButton* businessRadio_;
    QLabel* fareLabel_;
    QWidget* seatMapContainer_;
    QGridLayout* seatMapLayout_;
    QLabel* selectedSeatDisplayLabel_;
    QLabel* priceDiffLabel_;
    QPushButton* backBtn_;
    QPushButton* confirmBtn_;

    QString selectedSeatId_;

    static const QString SEAT_BASE_STYLE;
    static const QString BUSINESS_AVAILABLE_STYLE;
    static const QString ECONOMY_AVAILABLE_STYLE;
    static const QString BOOKED_STYLE;
    static const QString LOCKED_STYLE;
    static const QString SELECTED_STYLE_SUFFIX;
};

#endif // CHANGEBOOKINGDIALOG_H
