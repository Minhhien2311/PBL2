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
class AirportComboBox;
class FlightInstance;

/**
 * @brief Trang tìm kiếm chuyến bay và đặt vé cho Agent
 */
class SearchBookPage : public QWidget
{
    Q_OBJECT
public:
    explicit SearchBookPage(FlightManager* flManager,
                            BookingManager* bkManager,
                            AccountManager* accManager,
                            AirportManager* airportManager,
                            QWidget *parent = nullptr);

public slots:
    void refreshPage();  // New method to clear filters and results

private slots:
    // Unified search
    void onSearchClicked();
    // đặt vé
    void onBookClicked();

private:
    void setupUi();
    void setupModel();
    void setupConnections();
    void loadAllFlights();

    // nạp dữ liệu vào bảng
    void fillTable(const std::vector<FlightInstance*>& instances);

    // --- managers ---
    FlightManager*   flightManager_;
    BookingManager*  bookingManager_;
    AccountManager*  accountManager_;
    AirportManager*  airportManager_;

    // --- ô tìm kiếm ---
    AirportComboBox*  fromSearchCombo_;  // điểm đi
    AirportComboBox*  toSearchCombo_;    // điểm đến
    QDateEdit*  dateSearchEdit_;         // ngày khởi hành
    QComboBox* airlineFilterCombo_;      // hãng hàng không
    QLineEdit* priceMinEdit_;            // giá tối thiểu
    QLineEdit* priceMaxEdit_;            // giá tối đa
    QLabel* statusLabel_;                // status label for search results

    // --- bảng ---
    QTableView*          tableView_;
    QStandardItemModel*  model_;

    // --- nút đặt vé ---
    QPushButton* bookButton_;
};

#endif // SEARCHBOOKPAGE_H
