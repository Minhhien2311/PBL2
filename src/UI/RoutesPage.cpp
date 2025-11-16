#include "RoutesPage.h"

// <--- Sửa lỗi: Include manager và các thư viện cần thiết
#include "core/FlightManager.h"
#include "core/AirportManager.h"
#include "entities/Flight.h" // Cần để đọc dữ liệu
#include "AirportComboBox.h"
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
    topLayout->setContentsMargins(24, 20, 24, 16);
    topLayout->setSpacing(16);

    // === Hàng 1: Tiêu đề + Nút Tải lại (DI CHUYỂN LÊN ĐÂY) ===
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);

    QLabel* title = new QLabel("Tìm tuyến bay", this);
    title->setProperty("class", "PageTitle");
    headerRow->addWidget(title);
    headerRow->addStretch();

    // ← NÚT TẢI LẠI (di chuyển từ dưới lên đây)
    QPushButton* refreshButton = new QPushButton("🔄 Tải lại tất cả", topBar);
    refreshButton->setStyleSheet(
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:6px; height:32px; padding:0 16px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }"
    );
    refreshButton->setCursor(Qt::PointingHandCursor);
    refreshButton->setMinimumWidth(140);
    headerRow->addWidget(refreshButton);

    topLayout->addLayout(headerRow);

    // Kết nối nút refresh
    connect(refreshButton, &QPushButton::clicked, this, &RoutesPage::refreshTable);

    // ========== KHUNG TÌM KIẾM TUYẾN BAY ==========
    QWidget* searchBox = new QWidget;
    QVBoxLayout* searchBoxLayout = new QVBoxLayout(searchBox);
    searchBoxLayout->setContentsMargins(12, 12, 12, 12);
    searchBoxLayout->setSpacing(10);
    
    searchBox->setStyleSheet(
        "QWidget { background: white; border: 1px solid #c2cfe2; border-radius: 6px; }"
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
    QLabel* fromLabel = new QLabel("Từ");
    fromLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    col1->addWidget(fromLabel);
    fromSearchCombo_ = new AirportComboBox(airportManager_);
    fromSearchCombo_->setMinimumHeight(36);
    col1->addWidget(fromSearchCombo_);
    filterRowLayout->addLayout(col1, 1);

    // === Mũi tên giữa ===
    QVBoxLayout* arrowCol = new QVBoxLayout();
    arrowCol->setSpacing(6);
    QLabel* emptyArrowLabel = new QLabel(" ");
    emptyArrowLabel->setStyleSheet("background: transparent; border: none;");
    arrowCol->addWidget(emptyArrowLabel);
    
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
    QLabel* toLabel = new QLabel("Đến");
    toLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    col2->addWidget(toLabel);
    toSearchCombo_ = new AirportComboBox(airportManager_);
    toSearchCombo_->setMinimumHeight(36);
    col2->addWidget(toSearchCombo_);
    filterRowLayout->addLayout(col2, 1);

    // === Cột 3: Hãng hàng không ===
    QVBoxLayout* col3 = new QVBoxLayout();
    col3->setSpacing(6);
    QLabel* airlineLabel = new QLabel("Hãng hàng không");
    airlineLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    col3->addWidget(airlineLabel);
    airlineSearchCombo_ = new QComboBox(this);
    airlineSearchCombo_->addItem("Tùy chọn", "");
    airlineSearchCombo_->addItem("VietJet Air", "VietJet Air");
    airlineSearchCombo_->addItem("Vietnam Airlines", "Vietnam Airlines");
    airlineSearchCombo_->addItem("Bamboo Airways", "Bamboo Airways");
    airlineSearchCombo_->addItem("Vietravel Airlines", "Vietravel Airlines");
    airlineSearchCombo_->setMinimumHeight(36);
    col3->addWidget(airlineSearchCombo_);
    filterRowLayout->addLayout(col3, 1);

    // === Cột 4: Nút tìm kiếm (CÙNG HÀNG) ===
    QVBoxLayout* col4 = new QVBoxLayout();
    col4->setSpacing(6);
    QLabel* emptyLabel = new QLabel(" ");
    emptyLabel->setStyleSheet("background: transparent; border: none;");
    col4->addWidget(emptyLabel);
    
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

    // ========== TIÊU ĐỀ BẢNG + STATUS ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0);
    tableHeaderLayout->setSpacing(10);

    QLabel* tableTitle = new QLabel("📋 Kết quả tìm kiếm", this);
    tableTitle->setObjectName("TableTitle");
    tableTitle->setProperty("class", "SectionTitle");
    tableHeaderLayout->addWidget(tableTitle);

    // ← THÊM STATUS LABEL
    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 12px;");
    tableHeaderLayout->addWidget(statusLabel_);

    tableHeaderLayout->addStretch();

    mainLayout->addWidget(tableHeader);

    // ====== BẢNG ======
    tableView_ = new QTableView(this);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    tableView_->verticalHeader()->setVisible(true);
    tableView_->verticalHeader()->setMinimumWidth(32);
    tableView_->verticalHeader()->setDefaultSectionSize(30);
    tableView_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    tableView_->horizontalHeader()->setStretchLastSection(false);
    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QWidget *tableWrapper = new QWidget(this);
    QVBoxLayout *tableWrapLayout = new QVBoxLayout(tableWrapper);
    tableWrapLayout->setContentsMargins(24, 10, 24, 0);
    tableWrapLayout->addWidget(tableView_);

    mainLayout->addWidget(tableWrapper, 1);

    // ====== CRUD BAR ======
    QWidget *crudBar = new QWidget(this);
    QHBoxLayout *crudLayout = new QHBoxLayout(crudBar);
    crudLayout->setContentsMargins(24, 16, 24, 20);
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
            rowItems << new QStandardItem(QString::fromStdString(route->getRouteId()))
                   << new QStandardItem(QString::fromStdString(route->getDepartureAirport()))
                   << new QStandardItem(QString::fromStdString(route->getArrivalAirport()));
            model_->appendRow(rowItems);
        }
    }

    // ← CẬP NHẬT STATUS
    statusLabel_->setText(QString("Hiển thị tất cả %1 tuyến bay").arg(routes.size()));
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
            QMessageBox::information(this, "Thành công", 
                QString("Đã thêm tuyến bay mới:\n\n"
                       "Từ: %1 → Đến: %2")
                    .arg(fromIATA, toIATA));
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

    int row = selected.first().row();
    QString routeId = model_->item(row, 0)->text();
    QString fromIATA = model_->item(row, 1)->text();
    QString toIATA = model_->item(row, 2)->text();

    RouteDialog dialog(airportManager_, routeId, fromIATA, toIATA, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString newFrom = dialog.getFromIATA();
        QString newTo = dialog.getToIATA();
        
        bool success = flightManager_->updateRoute(
            routeId.toStdString(),
            newFrom.toStdString(),
            newTo.toStdString()
        );
        
        if (success) {
            QMessageBox::information(this, "Thành công", 
                QString("Đã cập nhật tuyến bay:\n\n"
                       "Mã tuyến: %1\n"
                       "Hãng: %2\n"
                       "Lộ trình: %3 → %4")
                    .arg(routeId, newFrom, newTo));
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
        QMessageBox::No);

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

void RoutesPage::onSearchByRoute()
{
    std::string fromIATA = fromSearchCombo_->getSelectedIATA();
    std::string toIATA = toSearchCombo_->getSelectedIATA();
    QString selectedAirline = airlineSearchCombo_->currentData().toString();
    
    if (fromIATA.empty() && toIATA.empty() && selectedAirline.isEmpty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", 
            "Vui lòng chọn ít nhất một tiêu chí tìm kiếm:\n"
            "• Điểm đi\n"
            "• Điểm đến\n"
            "• Hãng hàng không");
        return;
    }

    const std::vector<Route*>& allRoutes = flightManager_->getAllRoutes();
    
    model_->removeRows(0, model_->rowCount());
    
    int count = 0;
    for (Route* route : allRoutes) {
        if (route) {
            bool matchFrom = fromIATA.empty() || (route->getDepartureAirport() == fromIATA);
            bool matchTo = toIATA.empty() || (route->getArrivalAirport() == toIATA);
            bool matchAirline = selectedAirline.isEmpty(); // Routes don't have airlines, so this filter is always true if not specified
            
            if (matchFrom && matchTo && matchAirline) {
                QList<QStandardItem*> rowItems;
                rowItems << new QStandardItem(QString::fromStdString(route->getRouteId()))
                         << new QStandardItem(QString::fromStdString(route->getDepartureAirport()))
                         << new QStandardItem(QString::fromStdString(route->getArrivalAirport()));
                model_->appendRow(rowItems);
                count++;
            }
        }
    }

    // ← CẬP NHẬT STATUS
    statusLabel_->setText(QString("🔍 Tìm thấy %1 tuyến bay").arg(count));

    QString criteria;
    if (!fromIATA.empty()) criteria += QString("Từ: <b>%1</b>").arg(QString::fromStdString(fromIATA));
    if (!toIATA.empty()) {
        if (!criteria.isEmpty()) criteria += " | ";
        criteria += QString("Đến: <b>%1</b>").arg(QString::fromStdString(toIATA));
    }
    if (!selectedAirline.isEmpty()) {
        if (!criteria.isEmpty()) criteria += " | ";
        criteria += QString("Hãng: <b>%1</b>").arg(selectedAirline);
    }

    if (count == 0) {
        QMessageBox::information(this, "Không tìm thấy", 
            QString("Không tìm thấy tuyến bay nào khớp với:<br>%1").arg(criteria));
    } else {
        QMessageBox::information(this, "Kết quả", 
            QString("Tìm thấy <b>%1</b> tuyến bay khớp với:<br>%2").arg(count).arg(criteria));
    }
}