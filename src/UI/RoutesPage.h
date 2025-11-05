#ifndef ROUTESPAGE_H
#define ROUTESPAGE_H

#include <QWidget>

// Khai báo sớm
class QLineEdit;
class QPushButton;
class QTableView;
class QStandardItemModel;
class FlightManager; // <--- Sửa lỗi: Tên đúng (số ít)
class AirportManager;
class AirportComboBox;

class RoutesPage : public QWidget
{
    Q_OBJECT

public:
    // <--- Sửa lỗi: Sửa tên tham số cho rõ ràng
    explicit RoutesPage(FlightManager* flightManager, AirportManager* airportManager, QWidget *parent = nullptr);

private slots:
    // (Các slot giữ nguyên)
    void onSearchById();
    void onSearchByAirline();
    void onSearchByRoute(); 
    void onAddRoute();
    void onEditRoute();
    void onDeleteRoute();
    void refreshTable();

private:
    void setupUi();
    void setupModel();
    void setupConnections();

    // <--- Sửa lỗi: Sửa tên biến thành viên cho nhất quán
    FlightManager* flightManager_;
    AirportManager* airportManager_;

    // --- UI Components ---
    QLineEdit* idSearchEdit_;
    QLineEdit* airlineSearchEdit_;
    AirportComboBox* fromSearchCombo_;
    AirportComboBox* toSearchCombo_;   

    QTableView* tableView_;
    QStandardItemModel* model_;

    // <--- Sửa lỗi: Thêm các nút tìm kiếm làm biến thành viên
    QPushButton* searchByIdBtn_;
    QPushButton* searchByAirlineBtn_;
    QPushButton* searchByRouteBtn_;
    
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
};

#endif // ROUTESPAGE_H

