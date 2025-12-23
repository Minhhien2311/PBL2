#include "RoutesPage.h"
#include "AirportComboBox.h"
#include "BoldItemDelegate.h"
#include "PageRefresher.h"
#include "RouteDialog.h"
#include "core/AirportManager.h"
#include "core/FlightManager.h"
#include "entities/Flight.h"
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QThread>
#include <QVBoxLayout>
#include <iostream>
#include <string>

QWidget* createSearchGroup_Routes(const QString& title, QLineEdit*& edit, QPushButton*& button, const QString& buttonText)
{
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(6);
    
    layout->addWidget(new QLabel(title));
    edit = new QLineEdit;
    layout->addWidget(edit);
    button = new QPushButton(buttonText);
    layout->addWidget(button);
    return widget;
}

RoutesPage::RoutesPage(FlightManager* flightManager, AirportManager* airportManager, QWidget *parent)
    : QWidget(parent),
      flightManager_(flightManager),
      airportManager_(airportManager)
{
    Q_ASSERT(flightManager_ != nullptr);
    
    setupUi();
    setupModel();
    setupConnections();

    refreshTable();
}

void RoutesPage::setupUi()
{
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size: 17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:16px; }"
        "QLineEdit { background:white; border:1px solid #608bc1; border-radius:4px; height:26px; padding-left:6px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; height:24px; font-weight:600; }"
        "TableTitle { font-size: 18px; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ========== TOP BAR ==========
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 16);
    topLayout->setSpacing(16);
    topLayout->addStretch();

    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);
    headerRow->addStretch();

    QPushButton* refreshButton = new QPushButton("Làm mới trang", topBar);
    
    refreshButton->setIcon(QIcon("C:/PBL2/assets/icons/reload.png"));
    refreshButton->setIconSize(QSize(14, 14));

    refreshButton->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #133e87;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   border: none;"
        "   text-align: left;"
        "   padding: 0px;"
        "}"
        "QPushButton:hover {"
        "   background: transparent;"
        "   text-decoration: underline;"
        "}"
    );
    
    refreshButton->setCursor(Qt::PointingHandCursor);
    
    headerRow->addWidget(refreshButton);
    topLayout->addLayout(headerRow);

    connect(refreshButton, &QPushButton::clicked, this, &RoutesPage::refreshPage);

    // ========== SEARCH BOX ==========
    QWidget* searchBox = new QWidget;
    QVBoxLayout* searchBoxLayout = new QVBoxLayout(searchBox);
    searchBoxLayout->setContentsMargins(12, 12, 12, 12);
    searchBoxLayout->setSpacing(10);
    
    searchBox->setStyleSheet(
        "QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }"
    );

    QLabel* searchTitle = new QLabel("🔎 Tìm kiếm tuyến bay");
    searchTitle->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    searchBoxLayout->addWidget(searchTitle);

    QHBoxLayout* filterRowLayout = new QHBoxLayout();
    filterRowLayout->setSpacing(12);

    QVBoxLayout* col1 = new QVBoxLayout();
    col1->setSpacing(6);
    fromSearchCombo_ = new AirportComboBox(airportManager_);
    fromSearchCombo_->setMinimumHeight(36);
    col1->addWidget(fromSearchCombo_);
    filterRowLayout->addLayout(col1, 1);

    QVBoxLayout* arrowCol = new QVBoxLayout();
    arrowCol->setSpacing(6);
    
    QLabel* arrowLabel = new QLabel("→");
    arrowLabel->setAlignment(Qt::AlignCenter);
    arrowLabel->setStyleSheet("background: transparent; border: none; color: #123B7A; font-size: 18px;");
    QFont arrowFont = arrowLabel->font();
    arrowFont.setPointSize(16);
    arrowLabel->setFont(arrowFont);
    arrowLabel->setMinimumHeight(36);
    arrowCol->addWidget(arrowLabel);
    filterRowLayout->addLayout(arrowCol);

    QVBoxLayout* col2 = new QVBoxLayout();
    col2->setSpacing(6);
    toSearchCombo_ = new AirportComboBox(airportManager_);
    toSearchCombo_->setMinimumHeight(36);
    col2->addWidget(toSearchCombo_);
    filterRowLayout->addLayout(col2, 1);

    QVBoxLayout* col4 = new QVBoxLayout();
    col4->setSpacing(6);
    
    searchByRouteBtn_ = new QPushButton("Tìm kiếm");
    searchByRouteBtn_->setProperty("class", "SearchBtn");
    searchByRouteBtn_->setMinimumHeight(36);
    searchByRouteBtn_->setMinimumWidth(110);
    searchByRouteBtn_->setCursor(Qt::PointingHandCursor);
    searchByRouteBtn_->setStyleSheet(
        "QPushButton { background:#4472C4; color:white; font-weight:600; "
        "border-radius:6px; padding: 0 16px; }"
        "QPushButton:hover { background:#365a9e; }"
    );
    col4->addWidget(searchByRouteBtn_);
    filterRowLayout->addLayout(col4);

    searchBoxLayout->addLayout(filterRowLayout);

    topLayout->addWidget(searchBox);
    mainLayout->addWidget(topBar);

    // ========== STATUS + BUTTONS ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0);
    tableHeaderLayout->setSpacing(10);

    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    tableHeaderLayout->addWidget(statusLabel_);

    tableHeaderLayout->addStretch();

    addButton_ = new QPushButton("Thêm tuyến", this);
    editButton_ = new QPushButton("Sửa tuyến", this);
    deleteButton_ = new QPushButton("Xóa tuyến", this);

    addButton_->setStyleSheet("background: #27C93F; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #1b4d1b; color: white;");
    editButton_->setStyleSheet("background: #FFBD2E; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #b35c00; color: white;");
    deleteButton_->setStyleSheet("background: #FF5F57; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #8b1c1c; color: white;");

    tableHeaderLayout->addWidget(addButton_);
    tableHeaderLayout->addWidget(editButton_);
    tableHeaderLayout->addWidget(deleteButton_);

    mainLayout->addWidget(tableHeader);

    // ========== TABLE ==========
    QWidget *tableBox = new QWidget(this);

    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setStyleSheet(
        "QTableView { background: white; border: 1px solid #133e87; border-radius: 6px; }"
    );
    tableView_->setItemDelegate(new BoldItemDelegate(this));
    
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    tableView_->verticalHeader()->setVisible(false);
    tableView_->setAlternatingRowColors(true);
    tableView_->setShowGrid(false);
    
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);
}

void RoutesPage::setupModel()
{
    model_ = new QStandardItemModel(0, 4, this);
    model_->setHorizontalHeaderLabels({
        "STT", 
        "ID tuyến (FlightId)", 
        "Điểm đi", 
        "Điểm đến"
    });
    tableView_->setModel(model_);

    QHeaderView *header = tableView_->horizontalHeader();

    header->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView_->setColumnWidth(0, 50);

    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
}

// ========== CONNECTIONS ==========
void RoutesPage::setupConnections()
{
    connect(searchByRouteBtn_, &QPushButton::clicked, this, &RoutesPage::onSearchByRoute);
    connect(addButton_, &QPushButton::clicked, this, &RoutesPage::onAddRoute);
    connect(editButton_, &QPushButton::clicked, this, &RoutesPage::onEditRoute);
    connect(deleteButton_, &QPushButton::clicked, this, &RoutesPage::onDeleteRoute);
}

// ========== SLOTS ==========
void RoutesPage::refreshTable()
{
    model_->removeRows(0, model_->rowCount());
    const std::vector<Route*>& routes = flightManager_->getAllRoutes();
    
    for (int i = 0; i < routes.size(); ++i) {
        Route* route = routes[i];
        if (route) {
            QList<QStandardItem *> rowItems;
            
            rowItems << new QStandardItem(QString::number(i + 1));
            rowItems << new QStandardItem(QString::fromStdString(route->getRouteId()));
            rowItems << new QStandardItem(QString::fromStdString(airportManager_->getDisplayName(route->getDepartureAirport())));
            rowItems << new QStandardItem(QString::fromStdString(airportManager_->getDisplayName(route->getArrivalAirport())));
            
            for (auto* item : rowItems) item->setTextAlignment(Qt::AlignCenter);
            
            model_->appendRow(rowItems);
        }
    }
    statusLabel_->setText(QString("Hiển thị tất cả %1 tuyến bay").arg(routes.size()));
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
}

void RoutesPage::refreshPage() {
    PageRefresher::executeRefresh([this]() {
        refreshTable();
    });
}

void RoutesPage::onAddRoute()
{
    RouteDialog dialog(airportManager_, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString fromIATA = dialog.getFromIATA();
        QString toIATA = dialog.getToIATA();
        
        bool success = flightManager_->createNewRoute(
            fromIATA.toStdString(),
            toIATA.toStdString()
        );
        
        if (success) {
            std::cout << "Thêm tuyến bay thành công: " 
                 << fromIATA.toStdString() << " -> " 
                 << toIATA.toStdString() << std::endl;
            refreshTable();
        } else {
            std::cout << "Thêm tuyến bay thất bại (có thể đã tồn tại): " 
                 << fromIATA.toStdString() << " -> " 
                 << toIATA.toStdString() << std::endl;
        }
    }
}

void RoutesPage::onEditRoute()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        statusLabel_->setText("Vui lòng chọn một tuyến bay để sửa.");
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
        return;
    }

    int row = selected.first().row();
    QString routeId = model_->item(row, 1)->text();
    
    Route* currentRoute = flightManager_->findRouteById(routeId.toStdString());
    if (!currentRoute) {
        QMessageBox::critical(this, "Lỗi", "Không tìm thấy tuyến bay.");
        return;
    }
    
    QString fromIATA = QString::fromStdString(currentRoute->getDepartureAirport());
    QString toIATA = QString::fromStdString(currentRoute->getArrivalAirport());

    RouteDialog dialog(airportManager_, routeId, fromIATA, toIATA, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString newFrom = dialog.getFromIATA();
        QString newTo = dialog.getToIATA();
        
        if (newFrom == newTo) {
            QMessageBox::warning(this, "Dữ liệu không hợp lệ", 
                "Điểm đi và điểm đến phải khác nhau!");
            return;
        }
        
        QString newRouteId = newFrom + "-" + newTo;
        
        if (newRouteId == routeId) {
            QMessageBox::information(this, "Không có thay đổi", 
                "Thông tin tuyến bay không thay đổi.");
            return;
        }
        
        Route* existingRoute = flightManager_->findRouteById(newRouteId.toStdString());
        if (existingRoute != nullptr) {
            QMessageBox::warning(this, "Dữ liệu không hợp lệ", 
                "Tuyến bay này đã tồn tại!");
            return;
        }
        
        bool success = flightManager_->updateRoute(
            routeId.toStdString(),
            newFrom.toStdString(),
            newTo.toStdString()
        );
        
        if (success) {
            std::cout << "Cập nhật tuyến bay thành công: " 
                 << routeId.toStdString() << " -> " 
                 << newRouteId.toStdString() << std::endl;
            refreshTable();
        } else {
            std::cout << "Cập nhật tuyến bay thất bại: " 
                 << routeId.toStdString() << " -> " 
                 << newRouteId.toStdString() << std::endl; 
        }
    }
}

void RoutesPage::onDeleteRoute()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        statusLabel_->setText("Vui lòng chọn một tuyến bay để xóa.");
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
        return;
    }

    QString routeId = model_->item(selected.first().row(), 1)->text();

    auto reply = QMessageBox::question(this, "⚠️ Xác nhận xóa tuyến bay", 
        QString("Bạn có chắc chắn muốn xóa tuyến bay <b>%1</b>?<br><br>"
               "<font color='red'><b>Cảnh báo:</b></font><br>"
               "• Tất cả các chuyến bay thuộc tuyến này sẽ bị ảnh hưởng<br>"
               "• Hành động này <b>KHÔNG THỂ</b> hoàn tác")
            .arg(routeId), 
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        bool success = flightManager_->deleteRoute(routeId.toStdString());
        
        if (success) {
            std::cout << "Xóa tuyến bay thành công: " 
                 << routeId.toStdString() << std::endl;
            refreshTable();
        } else {
            std::cout << "Xóa tuyến bay thất bại: " 
                 << routeId.toStdString() << std::endl;
        }
    }
}

void RoutesPage::onSearchByRoute()
{
    std::string fromIATA = fromSearchCombo_->getSelectedIATA();
    std::string toIATA = toSearchCombo_->getSelectedIATA();
    
    if (fromIATA.empty() && toIATA.empty()) {
        statusLabel_->setText("Vui lòng chọn điểm đi và điểm đến để tìm kiếm.");
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
        return;
    }
    
    model_->removeRows(0, model_->rowCount());
    int count = 0;
    const std::vector<Route*>& allRoutes = flightManager_->getAllRoutes();
    
    for (Route* route : allRoutes) {
        if (route) {
            bool matchFrom = fromIATA.empty() || (route->getDepartureAirport() == fromIATA);
            bool matchTo = toIATA.empty() || (route->getArrivalAirport() == toIATA);
            
            if (matchFrom && matchTo) {
                count++;
                QList<QStandardItem*> rowItems;
                
                rowItems << new QStandardItem(QString::number(count));
                
                rowItems << new QStandardItem(QString::fromStdString(route->getRouteId()));
                rowItems << new QStandardItem(QString::fromStdString(airportManager_->getDisplayName(route->getDepartureAirport())));
                rowItems << new QStandardItem(QString::fromStdString(airportManager_->getDisplayName(route->getArrivalAirport())));
                
                for (auto* item : rowItems) item->setTextAlignment(Qt::AlignCenter);
                model_->appendRow(rowItems);
            }
        }
    }

    statusLabel_->setText(QString("Tìm thấy tuyến bay!"));
    statusLabel_->setStyleSheet("color: #27C93F; font-size: 13px; font-weight: 650;");

    if (count == 0) {
        statusLabel_->setText(QString("Không tìm thấy tuyến bay phù hợp!"));
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
    }
}