#include "FlightsPage.h"

// <--- Sửa lỗi: Include manager và các thư viện cần thiết
#include "core/FlightManager.h"
#include "entities/FlightInstance.h" // Cần để đọc dữ liệu
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
#include <QDateEdit> 
#include <QCalendarWidget> 

// (Hàm helper này có thể được chuyển ra 1 file util chung)
QWidget* createSearchGroup_Flights(const QString& title, QLineEdit*& edit, QPushButton*& button, const QString& buttonText)
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


FlightsPage::FlightsPage(FlightManager* flightManager, QWidget *parent)
    : QWidget(parent),
      flightManager_(flightManager)
{
    Q_ASSERT(flightManager_ != nullptr); 
    
    setupUi();
    setupModel();
    setupConnections();

    refreshTable();
}

void FlightsPage::setupUi()
{
    // style chung giống dashboard / routes
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:16px; }"
        "QLineEdit { background:white; border:1px solid #608bc1; border-radius:4px; height:26px; padding-left:6px; }"
        "QDateEdit { background:white; border:1px solid #608bc1; border-radius:4px; height:26px; padding-left:6px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; height:24px; font-weight:600; }"
        "QTableView { background:white; border:0px solid #d4dce5; }"
        "QHeaderView::section { background:#d5e2f2; padding:6px; border:1px solid #c2cfe2; }"
        "TableTitle { font-size: 18px; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ========== TOP BAR ==========
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 16);   // giống RoutesPage
    topLayout->setSpacing(16);

    // tiêu đề
    QLabel* title = new QLabel("Tìm chuyến bay", this);
    title->setProperty("class", "PageTitle");
    topLayout->addWidget(title);

    // hàng tìm kiếm 3 box
    QHBoxLayout *searchRow = new QHBoxLayout();
    searchRow->setSpacing(18);

    // --- Box 1: ID chuyến bay ---
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

    // --- Box 2: Lộ trình (từ – đến) ---
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

    // --- Box 3: Ngày khởi hành ---
    {
        QWidget *box = new QWidget;
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(5);

        dateSearchEdit_ = new QDateEdit(QDate::currentDate(), this);
        dateSearchEdit_->setCalendarPopup(true);
        dateSearchEdit_->setDisplayFormat("dd/MM/yyyy");

        searchByDateBtn_ = new QPushButton("Tìm theo ngày khởi hành");
        searchByDateBtn_->setProperty("class", "SearchBtn");

        v->addWidget(dateSearchEdit_);
        v->addWidget(searchByDateBtn_);

        searchRow->addWidget(box);
    }

    topLayout->addLayout(searchRow);
    mainLayout->addWidget(topBar);

    // ========== TIÊU ĐỀ BẢNG ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0);
    tableHeaderLayout->setSpacing(0);

    QLabel* tableTitle = new QLabel("Tất cả chuyến bay", this);
    tableTitle->setProperty("class", "SectionTitle");
    tableTitle->setObjectName("TableTitle");
    tableHeaderLayout->addWidget(tableTitle);
    tableHeaderLayout->addStretch();

    mainLayout->addWidget(tableHeader);

    // ========== BẢNG ==========
    tableView_ = new QTableView(this);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // bật STT giống dashboard/routes
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

    // ========== CRUD BAR ==========
    QWidget *crudBar = new QWidget(this);
    QHBoxLayout *crudLayout = new QHBoxLayout(crudBar);
    crudLayout->setContentsMargins(24, 16, 24, 20);
    crudLayout->setSpacing(16);

    addButton_ = new QPushButton("Thêm chuyến");
    editButton_ = new QPushButton("Sửa chuyến");
    deleteButton_ = new QPushButton("Xóa chuyến");

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

void FlightsPage::setupModel()
{
    model_ = new QStandardItemModel(0, 8, this);
    model_->setHorizontalHeaderLabels({
        "ID chuyến", "ID tuyến (FlightId)", "Ngày khởi hành", "Giờ khởi hành",
        "Ngày đến", "Giờ đến", "Ghế P.thông", "Ghế T.gia"
    });
    tableView_->setModel(model_);
}

void FlightsPage::setupConnections()
{
    // <--- Sửa lỗi: Kết nối các nút tìm kiếm
    connect(searchByIdBtn_, &QPushButton::clicked, this, &FlightsPage::onSearchById);
    connect(searchByRouteBtn_, &QPushButton::clicked, this, &FlightsPage::onSearchByRoute);
    connect(searchByDateBtn_, &QPushButton::clicked, this, &FlightsPage::onSearchByDate);

    // Kết nối CRUD
    connect(addButton_, &QPushButton::clicked, this, &FlightsPage::onAddFlight);
    connect(editButton_, &QPushButton::clicked, this, &FlightsPage::onEditFlight);
    connect(deleteButton_, &QPushButton::clicked, this, &FlightsPage::onDeleteFlight);
}

void FlightsPage::refreshTable()
{
    // --- [CHỖ NỐI API] ---
    model_->removeRows(0, model_->rowCount());

    // 1. Gọi API/Core để lấy tất cả *Chuyến bay* (FlightInstance)
    const DynamicArray<FlightInstance*>& instances = flightManager_->getAllInstances();
    
    // 2. Nạp dữ liệu mới
    for (int i = 0; i < instances.size(); ++i) {
        FlightInstance* inst = instances[i];
        if (inst) {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(QString::fromStdString(inst->getInstanceId()))
                   << new QStandardItem(QString::fromStdString(inst->getFlightId()))
                   << new QStandardItem(QString::fromStdString(inst->getDepartureDate()))
                   << new QStandardItem(QString::fromStdString(inst->getDepartureTime()))
                   << new QStandardItem(QString::fromStdString(inst->getArrivalDate()))
                   << new QStandardItem(QString::fromStdString(inst->getArrivalTime()))
                   << new QStandardItem(QString("%1/%2").arg(inst->getEconomyAvailable()).arg(inst->getEconomyTotal())) // Sửa lỗi: Đổi tên hàm
                   << new QStandardItem(QString("%1/%2").arg(inst->getBusinessAvailable()).arg(inst->getBusinessTotal())); // Sửa lỗi: Đổi tên hàm
            model_->appendRow(rowItems);
        }
    }
    // --- [HẾT CHỖ NỐI API] ---
}

void FlightsPage::onAddFlight()
{
    // --- [CHỖ NỐI API] ---
    // (Bạn cần tạo 1 QDialog, ví dụ FlightInstanceFormDialog)
    // QString flightId = ...
    // QString depDate = ...
    // bool success = flightManager_->createNewInstance(flightId.toStdString(), ...);
    // if (success) refreshTable();
    QMessageBox::information(this, "WIP", "Chức năng Thêm chuyến (sẽ mở Dialog).");
    // --- [HẾT CHỖ NỐI API] ---
}

void FlightsPage::onEditFlight()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một chuyến bay để sửa.");
        return;
    }
    QString instanceId = model_->itemFromIndex(selected.first().siblingAtColumn(0))->text();
    // --- [CHỖ NỐI API] ---
    // (FlightManager.h chưa có hàm updateInstance, bạn sẽ cần bổ sung)
    QMessageBox::information(this, "WIP", QString("Chức năng Sửa chuyến %1.").arg(instanceId));
    // --- [HẾT CHỖ NỐI API] ---
}

void FlightsPage::onDeleteFlight()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một chuyến bay để xóa.");
        return;
    }
    QString instanceId = model_->itemFromIndex(selected.first().siblingAtColumn(0))->text();

    auto reply = QMessageBox::question(this, "Xác nhận xóa", 
        QString("Bạn có chắc muốn xóa chuyến bay %1?").arg(instanceId), 
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // --- [CHỖ NỐI API] ---
        // (FlightManager.h chưa có hàm deleteInstance)
        // bool success = flightManager_->deleteInstance(instanceId.toStdString());
        // if (success) refreshTable();
        QMessageBox::information(this, "WIP", QString("Chức năng Xóa chuyến %1.").arg(instanceId));
        // --- [HẾT CHỐN NỐI API] ---
    }
}

// --- Slots tìm kiếm (WIP) ---
void FlightsPage::onSearchById()
{
    QMessageBox::information(this, "WIP", QString("Tìm kiếm theo ID: %1").arg(idSearchEdit_->text()));
}

void FlightsPage::onSearchByRoute()
{
    QMessageBox::information(this, "WIP", QString("Tìm kiếm theo lộ trình: %1 - %2")
                                                .arg(fromSearchEdit_->text())
                                                .arg(toSearchEdit_->text()));
}

void FlightsPage::onSearchByDate()
{
    QMessageBox::information(this, "WIP", QString("Tìm kiếm theo ngày: %1")
                                                .arg(dateSearchEdit_->date().toString("dd/MM/yyyy")));
}
