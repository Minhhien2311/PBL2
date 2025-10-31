#ifndef SEARCHBOOKPAGE_H
#define SEARCHBOOKPAGE_H

#include <QWidget>

class QLineEdit;
class QDateEdit;
class QTableView;
class QStandardItemModel;
class QPushButton;
class FlightManager;
class BookingManager;
class AccountManager;
template <typename T> class DynamicArray;
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
                            QWidget *parent = nullptr);

private slots:
    // 3 kiểu tìm
    void onSearchById();
    void onSearchByRoute();
    void onSearchByDate();
    // tải lại
    void onRefreshAll();
    // đặt vé
    void onBookClicked();

private:
    void setupUi();
    void setupModel();
    void setupConnections();

    // nạp dữ liệu vào bảng
    void fillTable(const DynamicArray<FlightInstance*>& instances);

    // --- managers ---
    FlightManager*   flightManager_;
    BookingManager*  bookingManager_;
    AccountManager*  accountManager_;

    // --- ô tìm kiếm ---
    QLineEdit*  idSearchEdit_;     // tìm theo instanceId
    QLineEdit*  fromSearchEdit_;   // điểm đi
    QLineEdit*  toSearchEdit_;     // điểm đến
    QDateEdit*  dateSearchEdit_;   // ngày khởi hành

    // --- nút tìm kiếm ---
    QPushButton* searchByIdBtn_;
    QPushButton* searchByRouteBtn_;
    QPushButton* searchByDateBtn_;
    QPushButton* refreshBtn_;

    // --- bảng ---
    QTableView*          tableView_;
    QStandardItemModel*  model_;

    // --- nút đặt vé ---
    QPushButton* bookButton_;
};

#endif // SEARCHBOOKPAGE_H
