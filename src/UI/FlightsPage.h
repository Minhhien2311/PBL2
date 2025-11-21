#ifndef FLIGHTSPAGE_H
#define FLIGHTSPAGE_H

#include <QWidget>

// Khai báo sớm
class QLineEdit;
class QPushButton;
class QTableView;
class QStandardItemModel;
class FlightManager; // Dùng FlightManager
class QDateEdit;
class AirportManager;
class AirportComboBox; 
class QComboBox;
class QLabel;

class FlightsPage : public QWidget
{
    Q_OBJECT

public:
    explicit FlightsPage(FlightManager* flightManager, AirportManager* airportManager, QWidget *parent = nullptr);

private slots:
    private slots:
    void onSearchById();
    void onSearchFilter();         // ← THÊM dòng này (tìm kiếm tổng hợp)
    void onAddFlight();
    void onEditFlight();
    void onDeleteFlight();
    void refreshTable();
    void refreshPage();  // New method to refresh when page shown or user changed

private:
    private:
    void setupUi();
    void setupModel();
    void setupConnections();

    FlightManager* flightManager_;
    AirportManager* airportManager_;

    // --- UI Components ---
    QLineEdit* idSearchEdit_;
    AirportComboBox* fromSearchCombo_;
    AirportComboBox* toSearchCombo_;
    QLineEdit* dateSearchEdit_; 
    QComboBox* airlineFilterCombo_;  // ← THÊM dòng này
    QLabel* statusLabel_;             // ← THÊM dòng này

    QTableView* tableView_;
    QStandardItemModel* model_;

    // <--- Sửa lỗi: Thêm các nút tìm kiếm làm biến thành viên
    QPushButton* searchByIdBtn_;
    QPushButton* searchByRouteBtn_;
    QPushButton* searchByDateBtn_;
    QPushButton* searchFilterBtn_;    // ← THÊM dòng này (nút tìm kiếm tổng hợp)
    
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
    QPushButton* refreshButton;      // ← THÊM dòng này
};

#endif // FLIGHTSPAGE_H
