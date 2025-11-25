#ifndef FLIGHTSPAGE_H
#define FLIGHTSPAGE_H

#include <QWidget>
#include <vector>

// Forward declarations
class QLineEdit;
class QPushButton;
class QTableView;
class QStandardItemModel;
class FlightManager;
class AirportManager;
class AirportComboBox; 
class QComboBox;
class QLabel;
class Flight; // Forward declaration cho Flight
class QCheckBox;

class FlightsPage : public QWidget
{
    Q_OBJECT

public:
    explicit FlightsPage(FlightManager* flightManager, AirportManager* airportManager, QWidget *parent = nullptr);

private slots:
    // Search & Filter
    void onSearchById();
    void onSearchFilter();
    
    // CRUD
    void onAddFlight();
    void onEditFlight();
    void onDeleteFlight();
    
    // Utilities
    void refreshPage();       // Làm mới trang (reset search)
    void onSortChanged(int index); // Xử lý sắp xếp
    void onFutureFilterChanged(bool checked); // Xử lý lọc chuyến chưa bay

private:
    void setupUi();
    void setupModel();
    void setupConnections();

    // --- Helper Functions (Giống SearchBookPage) ---
    void loadAllFlights(); // Nạp tất cả dữ liệu ban đầu
    void fillTable(const std::vector<Flight*>& flights); // Hàm nạp bảng dùng chung

    // --- Data ---
    std::vector<Flight*> currentFlights_; // Lưu danh sách đang hiển thị để sort

    // --- Managers ---
    FlightManager* flightManager_;
    AirportManager* airportManager_;

    // --- UI Components ---
    QLineEdit* idSearchEdit_;
    AirportComboBox* fromSearchCombo_;
    AirportComboBox* toSearchCombo_;
    QLineEdit* dateSearchEdit_; 
    QComboBox* airlineFilterCombo_;
    QComboBox* sortingCombo_;
    QLabel* statusLabel_;
    QLabel* warningLabel_1; // Nhãn cảnh báo cho lỗi nhập liệu
    QLabel* warningLabel_2; // Nhãn cảnh báo cho lỗi nhập liệu

    QTableView* tableView_;
    QStandardItemModel* model_;

    // --- Buttons ---
    QPushButton* searchByIdBtn_;
    QPushButton* searchFilterBtn_;
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
    QPushButton* refreshButton;
    QCheckBox* showFutureOnlyCheckbox_; 
};

#endif // FLIGHTSPAGE_H