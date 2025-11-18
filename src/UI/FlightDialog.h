#ifndef FLIGHTDIALOG_H
#define FLIGHTDIALOG_H

#include <QDialog>
#include <QString>
#include <QDate>

class QLineEdit;
class QPushButton;
class QComboBox;
class QDateEdit;
class QTimeEdit;
class QSpinBox;
class FlightManager;
class AirportManager;
class AirportComboBox;
class QLabel;

class FlightDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructor cho chế độ THÊM MỚI
    explicit FlightDialog(FlightManager* flightManager, 
                         AirportManager* airportManager, 
                         QWidget* parent = nullptr);
    
    // Constructor cho chế độ SỬA
    FlightDialog(FlightManager* flightManager,
                AirportManager* airportManager,
                const QString& flightId,
                const QString& flightNumber,
                const QString& airline,
                const QString& fromIATA,
                const QString& toIATA,
                const QString& depDate,
                const QString& depTime,
                const QString& arrDate,
                const QString& arrTime,
                int availableSeats,
                int totalCapacity,
                int fareEconomy,
                int fareBusiness,
                QWidget* parent = nullptr);

    // Getters
    QString getFlightId() const;
    QString getFlightNumber() const;
    QString getAirline() const;
    QString getFromIATA() const;
    QString getToIATA() const;
    QString getDepartureDate() const;
    QString getDepartureTime() const;
    QString getArrivalDate() const;
    QString getArrivalTime() const;
    int getTotalCapacity() const;
    int getAvailableSeats() const;
    int getFareEconomy() const;
    int getFareBusiness() const;
    QString getRouteId() const;

private slots:
    void onAccept();
    void onFlightRouteChanged();

private:
    void setupUi(bool isEditMode);
    void loadExistingRoutes(bool includeEmptyOption = true);

    FlightManager* flightManager_;
    AirportManager* airportManager_;
    bool isEditMode_;
    QString currentFlightId_;

    // UI Components
    QComboBox* flightRouteCombo_;  // Chọn tuyến bay (HAN → SGN)
    QLineEdit* flightNumberEdit_;  // Số hiệu (tự động fill hoặc nhập)
    AirportComboBox* fromCombo_;   // Điểm đi (readonly)
    AirportComboBox* toCombo_;     // Điểm đến (readonly)
    QComboBox* airlineCombo_;         // Hãng (readonly)
    
    QDateEdit* departureDateEdit_;
    QTimeEdit* departureTimeEdit_;
    QDateEdit* arrivalDateEdit_;
    QTimeEdit* arrivalTimeEdit_;
    
    QSpinBox* totalCapacitySpin_;
    QSpinBox* fareEconomySpin_;
    QSpinBox* fareBusinessSpin_;
    
    QPushButton* saveButton_;
    QPushButton* cancelButton_;
};

#endif // FLIGHTDIALOG_H