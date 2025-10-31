#include "RoutesPage.h"

// <--- Sửa lỗi: Include manager và các thư viện cần thiết
#include "core/FlightManager.h" 
#include "entities/Flight.h" // Cần để đọc dữ liệu
#include <string>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>

// (Hàm helper này có thể được chuyển ra 1 file util chung)
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

// <--- Sửa lỗi: Logic constructor (Lỗi 2)
RoutesPage::RoutesPage(FlightManager* flightManager, QWidget *parent)
    : QWidget(parent),
      flightManager_(flightManager) // Lưu con trỏ
{
    Q_ASSERT(flightManager_ != nullptr); // Đảm bảo manager hợp lệ
    
    setupUi();
    setupModel();
    setupConnections();

    refreshTable();
}

void RoutesPage::setupUi()
{
    // style chung giống dashboard
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size: 17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:16px; }"
        "QLineEdit { background:white; border:1px solid #608bc1; border-radius:4px; height:26px; padding-left:6px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; height:24px; font-weight:600; }"
        "QTableView { background:white; border:0px solid #d4dce5; }"
        "QHeaderView::section { background:#d5e2f2; padding:6px; border:1px solid #c2cfe2; }"
        "TableTitle { font-size: 18px; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ====== TOP BAR: tiêu đề + dải tìm kiếm ======
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 16);  // giống dashboard
    topLayout->setSpacing(16);

    // tiêu đề
    QLabel* title = new QLabel("Tìm tuyến bay", this);
    title->setProperty("class", "PageTitle");
    topLayout->addWidget(title);

    // dải tìm kiếm 3 box
    QHBoxLayout *searchRow = new QHBoxLayout();
    searchRow->setSpacing(18);

    // --- box 1: ID tuyến bay
    {
        QWidget *box = new QWidget;
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(5);

        idSearchEdit_ = new QLineEdit;
        searchByIdBtn_ = new QPushButton("Tìm theo ID");
        searchByIdBtn_->setProperty("class", "SearchBtn");

        v->addWidget(idSearchEdit_);
        v->addWidget(searchByIdBtn_);

        box->setMinimumWidth(200);
        searchRow->addWidget(box);
    }

    // --- box 2: Hãng hàng không
    {
        QWidget *box = new QWidget;
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(5);

        airlineSearchEdit_ = new QLineEdit;
        searchByAirlineBtn_ = new QPushButton("Tìm theo hãng bay");
        searchByAirlineBtn_->setProperty("class", "SearchBtn");

        v->addWidget(airlineSearchEdit_);
        v->addWidget(searchByAirlineBtn_);

        box->setMinimumWidth(200);
        searchRow->addWidget(box);
    }

    // --- box 3: Lộ trình
    {
        QWidget *box = new QWidget;
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(5);

        QWidget *routeRow = new QWidget;
        QHBoxLayout *routeHL = new QHBoxLayout(routeRow);
        routeHL->setContentsMargins(0,0,0,0);
        routeHL->setSpacing(6);

        fromSearchEdit_ = new QLineEdit;
        fromSearchEdit_->setPlaceholderText("Điểm xuất phát");
        toSearchEdit_ = new QLineEdit;
        toSearchEdit_->setPlaceholderText("Điểm đến");

        routeHL->addWidget(fromSearchEdit_);
        routeHL->addWidget(new QLabel("–"));
        routeHL->addWidget(toSearchEdit_);

        searchByRouteBtn_ = new QPushButton("Tìm theo lộ trình bay");
        searchByRouteBtn_->setProperty("class", "SearchBtn");

        v->addWidget(routeRow);
        v->addWidget(searchByRouteBtn_);

        searchRow->addWidget(box, 1);
    }

    topLayout->addLayout(searchRow);
    mainLayout->addWidget(topBar);

    // ====== TIÊU ĐỀ BẢNG ======
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0);
    tableHeaderLayout->setSpacing(0);

    QLabel* tableTitle = new QLabel("Tất cả tuyến bay", this);
    tableTitle->setObjectName("TableTitle");
    tableTitle->setProperty("class", "SectionTitle");
    tableHeaderLayout->addWidget(tableTitle);
    tableHeaderLayout->addStretch();

    mainLayout->addWidget(tableHeader);

    // ====== BẢNG ======
    tableView_ = new QTableView(this);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // cột STT giống dashboard
    tableView_->verticalHeader()->setVisible(true);
    tableView_->verticalHeader()->setMinimumWidth(32);
    tableView_->verticalHeader()->setDefaultSectionSize(30);
    tableView_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    tableView_->horizontalHeader()->setStretchLastSection(true);

    // bọc để có lề 24px
    QWidget *tableWrapper = new QWidget(this);
    QVBoxLayout *tableWrapLayout = new QVBoxLayout(tableWrapper);
    tableWrapLayout->setContentsMargins(24, 10, 24, 0);
    tableWrapLayout->addWidget(tableView_);

    mainLayout->addWidget(tableWrapper, 1);

    // ====== CRUD BAR ======
    QWidget *crudBar = new QWidget(this);
    QHBoxLayout *crudLayout = new QHBoxLayout(crudBar);
    crudLayout->setContentsMargins(24, 16, 24, 20);  // căn lề giống trên
    crudLayout->setSpacing(16);

    addButton_ = new QPushButton("Thêm tuyến");
    editButton_ = new QPushButton("Sửa tuyến");
    deleteButton_ = new QPushButton("Xóa tuyến");

    QString crudStyle =
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:10px; height:40px; padding:0 36px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }";

    addButton_->setStyleSheet(crudStyle);
    editButton_->setStyleSheet(crudStyle);
    deleteButton_->setStyleSheet(crudStyle);

    crudLayout->addStretch();
    crudLayout->addWidget(addButton_);
    crudLayout->addWidget(editButton_);
    crudLayout->addWidget(deleteButton_);
    crudLayout->addStretch();

    mainLayout->addWidget(crudBar);
}

void RoutesPage::setupModel()
{
    model_ = new QStandardItemModel(0, 5, this);
    model_->setHorizontalHeaderLabels({
        "ID tuyến (FlightId)", "Số hiệu (FlightNumber)", "Hãng hàng không", "Điểm đi (IATA)", "Điểm đến (IATA)"
    });
    tableView_->setModel(model_);
}

void RoutesPage::setupConnections()
{
    // <--- Sửa lỗi: Kết nối các nút tìm kiếm
    connect(searchByIdBtn_, &QPushButton::clicked, this, &RoutesPage::onSearchById);
    connect(searchByAirlineBtn_, &QPushButton::clicked, this, &RoutesPage::onSearchByAirline);
    connect(searchByRouteBtn_, &QPushButton::clicked, this, &RoutesPage::onSearchByRoute);

    // Kết nối CRUD
    connect(addButton_, &QPushButton::clicked, this, &RoutesPage::onAddRoute);
    connect(editButton_, &QPushButton::clicked, this, &RoutesPage::onEditRoute);
    connect(deleteButton_, &QPushButton::clicked, this, &RoutesPage::onDeleteRoute);
}

void RoutesPage::refreshTable()
{
    // --- [CHỖ NỐI API] ---
    model_->removeRows(0, model_->rowCount());

    // 1. Gọi API/Core để lấy tất cả *Tuyến bay* (Flight)
    const DynamicArray<Flight*>& routes = flightManager_->getAllFlights();
    
    // 2. Nạp dữ liệu mới
    for (int i = 0; i < routes.size(); ++i) {
        Flight* route = routes[i];
        if (route) {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(QString::fromStdString(route->getFlightId()))
                   << new QStandardItem(QString::fromStdString(route->getFlightNumber()))
                   << new QStandardItem(QString::fromStdString(route->getAirline()))
                   << new QStandardItem(QString::fromStdString(route->getDepartureAirport()))
                   << new QStandardItem(QString::fromStdString(route->getArrivalAirport()));
            model_->appendRow(rowItems);
        }
    }
    // --- [HẾT CHỖ NỐI API] ---
}

void RoutesPage::onAddRoute()
{
    // --- [CHỖ NỐI API] ---
    // (Bạn cần tạo 1 QDialog, ví dụ RouteFormDialog)
    // QString number = ...
    // QString airline = ...
    // bool success = flightManager_->createNewFlight(number.toStdString(), ...);
    // if (success) refreshTable();
    QMessageBox::information(this, "WIP", "Chức năng Thêm tuyến (sẽ mở Dialog).");
    // --- [HẾT CHỖ NỐI API] ---
}

void RoutesPage::onEditRoute()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một tuyến bay để sửa.");
        return;
    }
    QString routeId = model_->itemFromIndex(selected.first().siblingAtColumn(0))->text();

    // --- [CHỖ NỐI API] ---
    // (Bạn cần tạo 1 QDialog và gọi hàm update, tuy nhiên FlightManager.h
    // chưa có hàm update, bạn sẽ cần bổ sung hàm đó vào Core)
    QMessageBox::information(this, "WIP", QString("Chức năng Sửa tuyến %1.").arg(routeId));
    // --- [HẾT CHỖ NỐI API] ---
}

void RoutesPage::onDeleteRoute()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một tuyến bay để xóa.");
        return;
    }
    QString routeId = model_->itemFromIndex(selected.first().siblingAtColumn(0))->text();

    auto reply = QMessageBox::question(this, "Xác nhận xóa", 
        QString("Bạn có chắc muốn xóa tuyến bay %1?").arg(routeId), 
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // --- [CHỖ NỐI API] ---
        // (Tương tự, FlightManager.h chưa có hàm deleteFlight)
        // bool success = flightManager_->deleteFlight(routeId.toStdString());
        // if (success) refreshTable();
        QMessageBox::information(this, "WIP", QString("Chức năng Xóa tuyến %1.").arg(routeId));
        // --- [HẾT CHỐN NỐI API] ---
    }
}

// --- Slots tìm kiếm (WIP) ---
void RoutesPage::onSearchById()
{
    QMessageBox::information(this, "WIP", QString("Tìm kiếm theo ID: %1").arg(idSearchEdit_->text()));
}
void RoutesPage::onSearchByAirline()
{
     QMessageBox::information(this, "WIP", QString("Tìm kiếm theo Hãng: %1").arg(airlineSearchEdit_->text()));
}
void RoutesPage::onSearchByRoute()
{
    QMessageBox::information(this, "WIP", QString("Tìm kiếm theo Lộ trình: %1 - %2")
                                                .arg(fromSearchEdit_->text())
                                                .arg(toSearchEdit_->text()));
}
