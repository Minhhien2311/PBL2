#ifndef SEARCHBOOKPAGE_H
#define SEARCHBOOKPAGE_H

#include <QWidget>
#include <vector>

class QLineEdit;
class QDateEdit;
class QTableView;
class QStandardItemModel;
class QPushButton;
class QComboBox;
class QLabel;
class FlightManager;
class BookingManager;
class AccountManager;
class AirportManager;
class PassengerManager;
class AirportComboBox;
class Flight;

class SearchBookPage : public QWidget
{
    Q_OBJECT
public:
    explicit SearchBookPage(FlightManager* flManager,
                            BookingManager* bkManager,
                            AccountManager* accManager,
                            AirportManager* airportManager,
                            PassengerManager* passengerManager,
                            QWidget *parent = nullptr);

public slots:
    void refreshPage();

private slots:
    void onSearchClicked();
    void onBookClicked();
    void onSortChanged(int index);

private:
    void setupUi();
    void setupModel();
    void setupConnections();
    void loadAllFlights();

    void fillTable(const std::vector<Flight*>& flights);

    std::vector<Flight*> currentFlights_;

    FlightManager*   flightManager_;
    BookingManager*  bookingManager_;
    AccountManager*  accountManager_;
    AirportManager*  airportManager_;
    PassengerManager* passengerManager_;

    AirportComboBox*  fromSearchCombo_;
    AirportComboBox*  toSearchCombo_;
    QLineEdit*  dateSearchEdit_;
    QComboBox* airlineFilterCombo_;
    QComboBox* sortingCombo_;
    QLineEdit* priceMinEdit_;
    QLineEdit* priceMaxEdit_;
    QLabel* statusLabel_;
    QLabel* warningLabel_;
        
    QTableView*          tableView_;
    QStandardItemModel*  model_;

    QPushButton* bookButton_;
};

#endif // SEARCHBOOKPAGE_H
