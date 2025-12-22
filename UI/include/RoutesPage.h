#ifndef ROUTESPAGE_H
#define ROUTESPAGE_H

#include <QWidget>

// Khai báo sớm
class QLineEdit;
class QPushButton;
class QTableView;
class QStandardItemModel;
class FlightManager;
class AirportManager;
class AirportComboBox;
class QComboBox;
class QLabel;

class RoutesPage : public QWidget
{
    Q_OBJECT

public:
    explicit RoutesPage(FlightManager* flightManager, AirportManager* airportManager, QWidget *parent = nullptr);

private slots:
    void onSearchByRoute();
    void onAddRoute();
    void onEditRoute();
    void onDeleteRoute();
    void refreshTable();
    void refreshPage();  // New method to refresh when page shown or user changed

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

    QTableView* tableView_;
    QStandardItemModel* model_;
    QLabel* statusLabel_;
    QPushButton* searchByIdBtn_;
    QPushButton* searchByRouteBtn_;
    
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
};

#endif // ROUTESPAGE_H
