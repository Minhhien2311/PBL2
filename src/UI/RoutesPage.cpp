#include "RoutesPage.h"

// <--- Sửa lỗi: Include manager và các thư viện cần thiết
#include "core/FlightManager.h"
#include "core/AirportManager.h"
#include "entities/Flight.h" // Cần để đọc dữ liệu
#include "AirportComboBox.h"
#include "RouteDialog.h"
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
RoutesPage::RoutesPage(FlightManager* flightManager, AirportManager* airportManager, QWidget *parent)
    : QWidget(parent),
      flightManager_(flightManager),
      airportManager_(airportManager)
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

    // ====== KHỐI TÌM KIẾM ======
    QGridLayout *searchGrid = new QGridLayout();
    searchGrid->setHorizontalSpacing(15);
    searchGrid->setVerticalSpacing(10);
    searchGrid->setContentsMargins(0, 0, 0, 0);

    // Thiết lập độ giãn cột để phân bổ không gian hợp lý
    searchGrid->setColumnStretch(0, 0);  // Label "Từ:"
    searchGrid->setColumnStretch(1, 3);  // Combo từ
    searchGrid->setColumnStretch(2, 0);  // Arrow
    searchGrid->setColumnStretch(3, 0);  // Label "Đến:"
    searchGrid->setColumnStretch(4, 3);  // Combo đến
    searchGrid->setColumnStretch(5, 2);  // Airline input

    // ----- HÀNG 0: INPUTS -----
    // (0,0) Label "Từ:"
    QLabel* fromLabel = new QLabel("Từ:");
    fromLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    searchGrid->addWidget(fromLabel, 0, 0);

    // (0,1) Combo sân bay đi
    fromSearchCombo_ = new AirportComboBox(airportManager_);
    fromSearchCombo_->setMinimumHeight(40);
    searchGrid->addWidget(fromSearchCombo_, 0, 1);

    // (0,2) Mũi tên
    QLabel* arrowLabel = new QLabel("→");
    arrowLabel->setAlignment(Qt::AlignCenter);
    QFont arrowFont = arrowLabel->font();
    arrowFont.setPointSize(16);
    arrowLabel->setFont(arrowFont);
    searchGrid->addWidget(arrowLabel, 0, 2);

    // (0,3) Label "Đến:"
    QLabel* toLabel = new QLabel("Đến:");
    toLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    searchGrid->addWidget(toLabel, 0, 3);

    // (0,4) Combo sân bay đến
    toSearchCombo_ = new AirportComboBox(airportManager_);
    toSearchCombo_->setMinimumHeight(40);
    searchGrid->addWidget(toSearchCombo_, 0, 4);

    // (0,5) Input Hãng hàng không
    airlineSearchEdit_ = new QLineEdit;
    airlineSearchEdit_->setPlaceholderText("Nhập hãng hàng không...");
    airlineSearchEdit_->setMinimumHeight(40);
    searchGrid->addWidget(airlineSearchEdit_, 0, 5);

    // ----- HÀNG 1: BUTTONS -----
    // (1,1-4) Button tìm theo lộ trình (span từ cột 1 đến 4)
    searchByRouteBtn_ = new QPushButton("Tìm theo lộ trình bay");
    searchByRouteBtn_->setProperty("class", "SearchBtn");
    searchByRouteBtn_->setMinimumHeight(45);
    searchByRouteBtn_->setCursor(Qt::PointingHandCursor);
    searchGrid->addWidget(searchByRouteBtn_, 1, 1, 1, 4);  // rowspan=1, colspan=4

    // (1,5) Button tìm theo hãng bay
    searchByAirlineBtn_ = new QPushButton("Tìm theo hãng bay");
    searchByAirlineBtn_->setProperty("class", "SearchBtn");
    searchByAirlineBtn_->setMinimumHeight(45);
    searchByAirlineBtn_->setCursor(Qt::PointingHandCursor);
    searchGrid->addWidget(searchByAirlineBtn_, 1, 5);

    topLayout->addLayout(searchGrid);
    mainLayout->addWidget(topBar);

    // ====== TIÊU ĐỀ BẢNG + NÚT REFRESH ======
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0);
    tableHeaderLayout->setSpacing(0);

    QLabel* tableTitle = new QLabel("Tất cả tuyến bay", this);
    tableTitle->setObjectName("TableTitle");
    tableTitle->setProperty("class", "SectionTitle");
    tableHeaderLayout->addWidget(tableTitle);
    tableHeaderLayout->addStretch();

    // ⬅️ THÊM NÚT REFRESH
    QPushButton* refreshButton_ = new QPushButton("🔄 Tải lại tất cả");
    refreshButton_->setProperty("class", "SearchBtn");
    refreshButton_->setMinimumWidth(120);
    refreshButton_->setMinimumHeight(32);
    tableHeaderLayout->addWidget(refreshButton_);
    
    // Kết nối nút refresh
    connect(refreshButton_, &QPushButton::clicked, this, &RoutesPage::refreshTable);

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

    tableView_->horizontalHeader()->setStretchLastSection(false);

    // Header ngang giãn đều – tránh lỗi dồn trái
    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
    model_ = new QStandardItemModel(0, 4, this);
    model_->setHorizontalHeaderLabels({
        "ID tuyến (FlightId)", "Hãng hàng không", "Điểm đi (IATA)", "Điểm đến (IATA)"
    });
    tableView_->setModel(model_);
}

void RoutesPage::setupConnections()
{
    // Kết nối các nút tìm kiếm
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
    const std::vector<Flight*>& routes = flightManager_->getAllFlights();
    
    // 2. Nạp dữ liệu mới
    for (int i = 0; i < routes.size(); ++i) {
        Flight* route = routes[i];
        if (route) {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(QString::fromStdString(route->getFlightId()))
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
    // Mở dialog thêm tuyến
    RouteDialog dialog(airportManager_, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString airline = dialog.getAirline();
        QString fromIATA = dialog.getFromIATA();
        QString toIATA = dialog.getToIATA();
        
        // Gọi API tạo tuyến mới
        bool success = flightManager_->createNewFlight(
            airline.toStdString(),
            fromIATA.toStdString(),
            toIATA.toStdString()
        );
        
        if (success) {
            QMessageBox::information(this, "Thành công", 
                QString("Đã thêm tuyến bay mới:\n\n"
                       "Hãng: %1\n"
                       "Từ: %2 → Đến: %3")
                    .arg(airline, fromIATA, toIATA));
            refreshTable();
        } else {
            QMessageBox::critical(this, "Thất bại", 
                "Không thể thêm tuyến bay.\n\n"
                "Có thể do:\n"
                "• Tuyến bay đã tồn tại\n"
                "• Lỗi lưu dữ liệu");
        }
    }
}

void RoutesPage::onEditRoute()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một tuyến bay để sửa.");
        return;
    }

    // Lấy thông tin hiện tại từ bảng
    int row = selected.first().row();
    QString routeId = model_->item(row, 0)->text();
    QString airline = model_->item(row, 1)->text();
    QString fromIATA = model_->item(row, 2)->text();
    QString toIATA = model_->item(row, 3)->text();

    // Mở dialog với dữ liệu hiện tại
    RouteDialog dialog(airportManager_, routeId, airline, fromIATA, toIATA, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString newAirline = dialog.getAirline();
        QString newFrom = dialog.getFromIATA();
        QString newTo = dialog.getToIATA();
        
        // Gọi API cập nhật
        bool success = flightManager_->updateFlight(
            routeId.toStdString(),
            newAirline.toStdString(),
            newFrom.toStdString(),
            newTo.toStdString()
        );
        
        if (success) {
            QMessageBox::information(this, "Thành công", 
                QString("Đã cập nhật tuyến bay:\n\n"
                       "Mã tuyến: %1\n"
                       "Hãng: %2\n"
                       "Lộ trình: %3 → %4")
                    .arg(routeId, newAirline, newFrom, newTo));
            refreshTable();
        } else {
            QMessageBox::critical(this, "Thất bại", 
                "Không thể cập nhật tuyến bay.\n\n"
                "Vui lòng thử lại.");
        }
    }
}

void RoutesPage::onDeleteRoute()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một tuyến bay để xóa.");
        return;
    }

    QString routeId = model_->item(selected.first().row(), 0)->text();

    auto reply = QMessageBox::question(this, "⚠️ Xác nhận xóa tuyến bay", 
        QString("Bạn có chắc chắn muốn xóa tuyến bay <b>%1</b>?<br><br>"
               "<font color='red'><b>Cảnh báo:</b></font><br>"
               "• Tất cả các chuyến bay thuộc tuyến này sẽ bị ảnh hưởng<br>"
               "• Hành động này <b>KHÔNG THỂ</b> hoàn tác")
            .arg(routeId), 
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);  // Default = No để an toàn

    if (reply == QMessageBox::Yes) {
        bool success = flightManager_->deleteFlight(routeId.toStdString());
        
        if (success) {
            QMessageBox::information(this, "✅ Xóa thành công", 
                QString("Đã xóa tuyến bay: <b>%1</b><br><br>"
                       "Dữ liệu đã được cập nhật.")
                    .arg(routeId));
            refreshTable();
        } else {
            QMessageBox::critical(this, "❌ Xóa thất bại", 
                QString("Không thể xóa tuyến bay <b>%1</b>.<br><br>"
                       "Có thể do:<br>"
                       "• Tuyến đang có chuyến bay hoạt động<br>"
                       "• Lỗi lưu dữ liệu")
                    .arg(routeId));
        }
    }
}

// --- Slots tìm kiếm (WIP) ---
void RoutesPage::onSearchByAirline()
{
    QString searchText = airlineSearchEdit_->text().trimmed();
    
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng nhập tên hãng hàng không.");
        return;
    }

    const std::vector<Flight*>& allRoutes = flightManager_->getAllFlights();
    
    model_->removeRows(0, model_->rowCount());
    
    int count = 0;
    for (Flight* route : allRoutes) {
        if (route) {
            QString airline = QString::fromStdString(route->getAirline());
            if (airline.contains(searchText, Qt::CaseInsensitive)) {
                QList<QStandardItem*> rowItems;
                rowItems << new QStandardItem(QString::fromStdString(route->getFlightId()))
                         << new QStandardItem(airline)
                         << new QStandardItem(QString::fromStdString(route->getDepartureAirport()))
                         << new QStandardItem(QString::fromStdString(route->getArrivalAirport()));
                model_->appendRow(rowItems);
                count++;
            }
        }
    }

    if (count == 0) {
        QMessageBox::information(this, "Không tìm thấy", 
            QString("Không tìm thấy tuyến bay nào của hãng: <b>%1</b>").arg(searchText));
    } else {
        QMessageBox::information(this, "Kết quả", 
            QString("Tìm thấy <b>%1</b> tuyến bay của hãng <b>%2</b>").arg(count).arg(searchText));
    }
}

void RoutesPage::onSearchByRoute()
{
    std::string fromIATA = fromSearchCombo_->getSelectedIATA();
    std::string toIATA = toSearchCombo_->getSelectedIATA();

    if (fromIATA.empty() || toIATA.empty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng chọn cả điểm đi và điểm đến.");
        return;
    }

    std::vector<Flight*> results = flightManager_->findFlightByRoute(fromIATA, toIATA);
    
    model_->removeRows(0, model_->rowCount());
    
    if (results.empty()) {
        QMessageBox::information(this, "Không tìm thấy", 
            QString("Không tìm thấy tuyến bay từ <b>%1</b> đến <b>%2</b>")
                .arg(QString::fromStdString(fromIATA))
                .arg(QString::fromStdString(toIATA)));
        return;
    }

    for (Flight* route : results) {
        if (route) {
            QList<QStandardItem*> rowItems;
            rowItems << new QStandardItem(QString::fromStdString(route->getFlightId()))
                     << new QStandardItem(QString::fromStdString(route->getAirline()))
                     << new QStandardItem(QString::fromStdString(route->getDepartureAirport()))
                     << new QStandardItem(QString::fromStdString(route->getArrivalAirport()));
            model_->appendRow(rowItems);
        }
    }

    QMessageBox::information(this, "Kết quả", 
        QString("Tìm thấy <b>%1</b> tuyến bay").arg(results.size()));
}