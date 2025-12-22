#ifndef PROMOTIONSPAGE_H
#define PROMOTIONSPAGE_H

#include "entities/Promotion.h"
#include <QWidget>
#include <vector>

class QLineEdit;
class QPushButton;
class QTableView;
class QStandardItemModel;
class QLabel;

class PromotionsPage : public QWidget
{
    Q_OBJECT

public:
    explicit PromotionsPage(QWidget *parent = nullptr);
    ~PromotionsPage();

private slots:
    void onSearchByCode();
    void onAddPromotion();
    void onEditPromotion();
    void onDeletePromotion();
    void refreshPage(); 

private:
    void setupUi();
    void setupModel();
    void setupConnections();

    void loadDataFromFile();
    void saveDataToFile();
    void refreshTable(const std::vector<Promotion>& data);

    std::vector<Promotion> promotions_;
    const std::string DATA_FILE = "C:/PBL2/data/promotions.txt";

    QLineEdit* codeSearchEdit_;
    QPushButton* searchBtn_;
    QPushButton* refreshButton_;
    QLabel* statusLabel_;
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
    QTableView* tableView_;
    QStandardItemModel* model_;
};

#endif // PROMOTIONSPAGE_H