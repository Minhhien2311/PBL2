#include "RoutesPage.h"
#include <iostream>
#include "core/FlightManager.h"
#include "core/AirportManager.h"
#include "entities/Flight.h" // Cần để đọc dữ liệu
#include "AirportComboBox.h"
#include "RouteDialog.h"
#include "BoldItemDelegate.h"
#include "PageRefresher.h"
#include "RouteDialog.h"
#include <string>

#include <QVBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QThread> // Required for QThread::msleep

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
    // style chung giống dashboard
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

    // ====== TOP BAR: NÚT TẢI LẠI ======
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 16);
    topLayout->setSpacing(16);
    topLayout->addStretch();

    // === Hàng 1: Nút Tải lại ===
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);

    // NÚT TẢI LẠI (góc phải trên)
    QPushButton* refreshButton = new QPushButton("Làm mới trang", topBar);
    
    // [QUAN TRỌNG] Set Icon (Bạn thay đường dẫn file ảnh vào đây)
    // Lưu ý: Nên dùng icon có màu #133e87 để đồng bộ với chữ
    refreshButton->setIcon(QIcon("C:/PBL2/assets/icons/reload.png")); // Đường dẫn icon")); 
    refreshButton->setIconSize(QSize(14, 14)); // Kích thước icon

    refreshButton->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"  /* Nền trong suốt (ghi đè nền xanh global) */
        "   color: #133e87;"           /* Màu chữ xanh (ghi đè chữ trắng global) */
        "   font-weight: bold;"         /* Chữ đậm hơn */
        "   font-size: 13px;"
        "   border: none;"             /* Bỏ viền (ghi đè viền global) */
        "   text-align: left;"         /* Căn trái để icon và chữ nằm gọn */
        "   padding: 0px;"             /* Reset padding để nút gọn gàng hơn */
        "}"
        "QPushButton:hover {"
        "   background: transparent;"  /* Giữ nguyên nền trong suốt hoặc thêm màu nhạt nếu thích */
        "   text-decoration: underline;"         /* Gạch chân khi hover */
        "}"
    );
    
    refreshButton->setCursor(Qt::PointingHandCursor);
    // refreshButton->setMinimumWidth(140); // Có thể bỏ dòng này để nút tự co theo chữ
    
    headerRow->addWidget(refreshButton);
    topLayout->addLayout(headerRow);

    // Kết nối nút refresh
    connect(refreshButton, &QPushButton::clicked, this, &RoutesPage::refreshPage);

    // ========== KHUNG TÌM KIẾM TUYẾN BAY ==========
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

    // ← LAYOUT NGANG: Labels + Inputs + Nút tìm kiếm
    QHBoxLayout* filterRowLayout = new QHBoxLayout();
    filterRowLayout->setSpacing(12);

    // === Cột 1: Điểm đi ===
    QVBoxLayout* col1 = new QVBoxLayout();
    col1->setSpacing(6);
    fromSearchCombo_ = new AirportComboBox(airportManager_);
    fromSearchCombo_->setMinimumHeight(36);
    col1->addWidget(fromSearchCombo_);
    filterRowLayout->addLayout(col1, 1);

    // === Mũi tên giữa ===
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

    // === Cột 2: Điểm đến ===
    QVBoxLayout* col2 = new QVBoxLayout();
    col2->setSpacing(6);
    toSearchCombo_ = new AirportComboBox(airportManager_);
    toSearchCombo_->setMinimumHeight(36);
    col2->addWidget(toSearchCombo_);
    filterRowLayout->addLayout(col2, 1);

    // === Cột 3: Nút tìm kiếm (CÙNG HÀNG) ===
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

    // ========== STATUS + CRUD BAR (GỘP CHUNG) ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    // Giữ nguyên margin để căn lề khớp với các phần trên
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0); 
    tableHeaderLayout->setSpacing(10);

    // 1. Status Label
    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    tableHeaderLayout->addWidget(statusLabel_);

    // 2. Lò xo đẩy các nút sang phải (Quan trọng để thẳng hàng đẹp)
    tableHeaderLayout->addStretch(); 

    // 3. Các nút CRUD (Bên phải)
    // Khởi tạo nút
    addButton_ = new QPushButton("Thêm tuyến", this);
    editButton_ = new QPushButton("Sửa tuyến", this);
    deleteButton_ = new QPushButton("Xóa tuyến", this);

    // Style giữ nguyên như bạn yêu cầu
    QString crudStyle =
        "QPushButton { background:transparent; color: #133e87; border:1px solid #133e87; "
        "border-radius:6px; height:20px; padding:4px 10px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; color: white; }"; // Thêm color white khi hover cho rõ

    addButton_->setStyleSheet(crudStyle);
    editButton_->setStyleSheet(crudStyle);
    deleteButton_->setStyleSheet(crudStyle);

    // Add trực tiếp vào layout header, không cần layout phụ
    tableHeaderLayout->addWidget(addButton_);
    tableHeaderLayout->addWidget(editButton_);
    tableHeaderLayout->addWidget(deleteButton_);

    // Add Header vào Main Layout
    mainLayout->addWidget(tableHeader);

    // ====== BẢNG ======
    QWidget *tableBox = new QWidget(this);

    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setStyleSheet(
        "QTableView { background: white; border: 1px solid #133e87; border-radius: 6px; }"
    );
    tableView_->setItemDelegate(new BoldItemDelegate(this));
    
    // --- CẤU HÌNH GIAO DIỆN BẢNG (Giống SearchBookPage) ---
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    tableView_->verticalHeader()->setVisible(false); // Tắt cột số mặc định xấu
    tableView_->setAlternatingRowColors(true);       // Màu xen kẽ
    tableView_->setShowGrid(false);                  // Tắt lưới
    
    // --- XỬ LÝ SCROLLBAR ---
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Tắt thanh ngang
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);
}

void RoutesPage::setupModel()
{
    // Tăng lên 4 cột: STT, ID, Điểm đi, Điểm đến
    model_ = new QStandardItemModel(0, 4, this);
    model_->setHorizontalHeaderLabels({
        "STT", 
        "ID tuyến (FlightId)", 
        "Điểm đi", 
        "Điểm đến"
    });
    tableView_->setModel(model_);

    // --- CẤU HÌNH HEADER ---
    QHeaderView *header = tableView_->horizontalHeader();


    // 1. Cột STT (0): Cố định nhỏ
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView_->setColumnWidth(0, 50);

    // 2. Chọn cột cuối cùng ("Điểm đến") làm LÒ XO (Stretch) để lấp đầy bảng
    // Hoặc bạn có thể chọn cột "Điểm đi" (cột 2) tùy thích.
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
}

void RoutesPage::setupConnections()
{
    connect(searchByRouteBtn_, &QPushButton::clicked, this, &RoutesPage::onSearchByRoute);
    connect(addButton_, &QPushButton::clicked, this, &RoutesPage::onAddRoute);
    connect(editButton_, &QPushButton::clicked, this, &RoutesPage::onEditRoute);
    connect(deleteButton_, &QPushButton::clicked, this, &RoutesPage::onDeleteRoute);
}

void RoutesPage::refreshTable()
{
    model_->removeRows(0, model_->rowCount());
    const std::vector<Route*>& routes = flightManager_->getAllRoutes();
    
    for (int i = 0; i < routes.size(); ++i) {
        Route* route = routes[i];
        if (route) {
            QList<QStandardItem *> rowItems;
            
            // 0. STT
            rowItems << new QStandardItem(QString::number(i + 1)); 
            
            // 1. ID
            rowItems << new QStandardItem(QString::fromStdString(route->getRouteId()));
            
            // 2. Điểm đi
            rowItems << new QStandardItem(QString::fromStdString(airportManager_->getDisplayName(route->getDepartureAirport())));
            
            // 3. Điểm đến
            rowItems << new QStandardItem(QString::fromStdString(airportManager_->getDisplayName(route->getArrivalAirport())));
            
            // Canh giữa tất cả
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
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
        return;
    }

    int row = selected.first().row();
    QString routeId = model_->item(row, 1)->text();
    
    // ✅ LẤY IATA CODE từ model (không phải display name)
    // Giả sử cột 1, 2 là display name, cần parse IATA
    // Hoặc lưu IATA trong data role
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
        
        // === VALIDATION LAYER 1: UI Check ===
        
        // ✅ Check 1: Điểm đi và đến phải khác nhau
        if (newFrom == newTo) {
            QMessageBox::warning(this, "Dữ liệu không hợp lệ", 
                "Điểm đi và điểm đến phải khác nhau!");
            return;
        }
        
        // ✅ Check 2: Tính ID mới
        QString newRouteId = newFrom + "-" + newTo;
        
        // ✅ Check 3: Nếu không thay đổi gì
        if (newRouteId == routeId) {
            QMessageBox::information(this, "Không có thay đổi", 
                "Thông tin tuyến bay không thay đổi.");
            return;
        }
        
        // ✅ Check 4: Kiểm tra trùng với route khác
        Route* existingRoute = flightManager_->findRouteById(newRouteId.toStdString());
        if (existingRoute != nullptr) {
            QMessageBox::warning(this, "Dữ liệu không hợp lệ", 
                "Tuyến bay này đã tồn tại!");
            return;
        }
        
        // === VALIDATION PASSED - Thực hiện update ===
        
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
            // Nếu vẫn thất bại (không nên xảy ra vì đã validate)
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
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
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
        // ✅ SỬA: Gọi deleteRoute() thay vì deleteFlight()
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
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
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
                
                // STT (Dùng biến count)
                rowItems << new QStandardItem(QString::number(count));
                
                rowItems << new QStandardItem(QString::fromStdString(route->getRouteId()));
                rowItems << new QStandardItem(QString::fromStdString(airportManager_->getDisplayName(route->getDepartureAirport())));
                rowItems << new QStandardItem(QString::fromStdString(airportManager_->getDisplayName(route->getArrivalAirport())));
                
                for (auto* item : rowItems) item->setTextAlignment(Qt::AlignCenter);
                model_->appendRow(rowItems);
            }
        }
    }

    // ← CẬP NHẬT STATUS
    statusLabel_->setText(QString("Tìm thấy tuyến bay!"));
    statusLabel_->setStyleSheet("color: #2E7D32; font-size: 13px; font-weight: 650;");

    // QString criteria;
    // if (!fromIATA.empty()) criteria += QString("Từ: <b>%1</b>").arg(QString::fromStdString(fromIATA));
    // if (!toIATA.empty()) {
    //     if (!criteria.isEmpty()) criteria += " | ";
    //     criteria += QString("Đến: <b>%1</b>").arg(QString::fromStdString(toIATA));
    // }

    if (count == 0) {
        statusLabel_->setText(QString("Không tìm thấy tuyến bay phù hợp!"));
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
    }
}