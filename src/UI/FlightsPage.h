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

class FlightsPage : public QWidget
{
    Q_OBJECT

public:
    explicit FlightsPage(FlightManager* flightManager, QWidget *parent = nullptr);

private slots:
    void onSearchById();
    void onSearchByRoute();
    void onSearchByDate();
    void onAddFlight();
    void onEditFlight();
    void onDeleteFlight();
    void refreshTable();

private:
    void setupUi();
    void setupModel();
    void setupConnections();

    FlightManager* flightManager_; 

    // --- UI Components ---
    QLineEdit* idSearchEdit_;
    QLineEdit* fromSearchEdit_; 
    QLineEdit* toSearchEdit_;   
    QDateEdit* dateSearchEdit_; 

    QTableView* tableView_;
    QStandardItemModel* model_;

    // <--- Sửa lỗi: Thêm các nút tìm kiếm làm biến thành viên
    QPushButton* searchByIdBtn_;
    QPushButton* searchByRouteBtn_;
    QPushButton* searchByDateBtn_;
    
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
};

#endif // FLIGHTSPAGE_H

