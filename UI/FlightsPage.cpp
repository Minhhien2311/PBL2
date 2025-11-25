#include "FlightsPage.h"
#include "utils/Sorting.h"
#include "core/FlightManager.h"
#include "core/SeatManager.h"
#include "core/AirportManager.h"
#include "entities/Flight.h"
#include "utils/Helpers.h"
#include "FlightDialog.h"
#include "AirportComboBox.h"
#include "BoldItemDelegate.h"
#include "PageRefresher.h"
#include <string>
#include <algorithm> // cho std::reverse

#include <QComboBox>
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
#include <QTimer>
#include <QCheckBox>

// Helper function để format tiền tệ Việt Nam
namespace {
    QString formatVietnamCurrency(int price) {
        QString priceStr = QString::number(price);
        QString result;
        int count = 0;
        
        for (int i = priceStr.length() - 1; i >= 0; --i) {
            if (count == 3) {
                result.prepend('.');
                count = 0;
            }
            result.prepend(priceStr[i]);
            count++;
        }
        
        return result + " VNĐ";
    }
}

FlightsPage::FlightsPage(FlightManager* flightManager, AirportManager* airportManager, QWidget *parent)
    : QWidget(parent),
      flightManager_(flightManager),
      airportManager_(airportManager)
{
    Q_ASSERT(flightManager_ != nullptr); 
    
    setupUi();
    setupModel();
    setupConnections();

    loadAllFlights(); // Thay vì refreshTable, dùng loadAllFlights cho đồng bộ tên
}

void FlightsPage::setupUi()
{
    
    // style chung giống dashboard / routes
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:16px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; height:36px; font-weight:600; }"
        
        // Gom QComboBox và QLineEdit cùng chiều cao 30px (hoặc 36px cho đẹp) để cân đối với nút
        "QLineEdit, QComboBox { background:white; border:1px solid #608bc1; border-radius:4px; height:30px; padding-left:6px; }" 
        
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

    // === Hàng 1: Nút Tải lại ===
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);
    headerRow->addStretch();

    refreshButton = new QPushButton("Làm mới trang", topBar);
    refreshButton->setIcon(QIcon("C:/PBL2/assets/icons/reload.png")); 
    refreshButton->setIconSize(QSize(14, 14));
    refreshButton->setStyleSheet(
        "QPushButton { background: transparent; color: #133e87; font-weight: bold; font-size: 13px; border: none; text-align: left; padding: 0px; }"
        "QPushButton:hover { background: transparent; text-decoration: underline; }"
    );
    refreshButton->setCursor(Qt::PointingHandCursor);
    headerRow->addWidget(refreshButton);
    topLayout->addLayout(headerRow);

    // ========== HÀNG TÌM KIẾM ==========
    QHBoxLayout* searchRowLayout = new QHBoxLayout();
    searchRowLayout->setSpacing(16);

    // BOX 1: TRA CỨU NHANH
    QWidget* quickSearchBox = new QWidget;
    QVBoxLayout* qsLayout = new QVBoxLayout(quickSearchBox);
    qsLayout->setContentsMargins(15, 15, 15, 15);
    qsLayout->setSpacing(10);
    quickSearchBox->setStyleSheet("QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }");
    quickSearchBox->setMinimumWidth(250);

    idSearchEdit_ = new QLineEdit;
    idSearchEdit_->setPlaceholderText("VD: FI-000001");
    qsLayout->addWidget(idSearchEdit_);

    searchByIdBtn_ = new QPushButton("Tìm kiếm nhanh bằng ID");
    searchByIdBtn_->setProperty("class", "SearchBtn");
    searchByIdBtn_->setCursor(Qt::PointingHandCursor);
    searchByIdBtn_->setStyleSheet(
        "QPushButton { background:#4478BD; color:white; font-weight:600; border-radius:6px; height:36px; }"
        "QPushButton:hover { background:#365a9e; }"
    );
    qsLayout->addWidget(searchByIdBtn_);

    // Warning label for input errors
    warningLabel_1 = new QLabel(this);
    warningLabel_1->setStyleSheet("color: #FF0000; font-size: 11px; font-weight: 500; border: none; background: transparent;");
    quickSearchBox->layout()->addWidget(warningLabel_1);
    // Mặc định ẩn
    warningLabel_1->setVisible(false);

    quickSearchBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    searchRowLayout->addWidget(quickSearchBox, 0, Qt::AlignLeft);

    // BOX 2: TÌM KIẾM NÂNG CAO
    QWidget* advancedSearchBox = new QWidget;
    QVBoxLayout* asLayout = new QVBoxLayout(advancedSearchBox);
    asLayout->setContentsMargins(12, 12, 12, 12);
    asLayout->setSpacing(10);
    advancedSearchBox->setStyleSheet("QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }");

    QLabel* asTitle = new QLabel("🔎 Tìm kiếm nâng cao theo nhiều tiêu chí");
    asTitle->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    asLayout->addWidget(asTitle);

    QHBoxLayout* filterRowLayout = new QHBoxLayout();
    filterRowLayout->setSpacing(12);

    // Cột 1-4: Inputs
    auto addFilterCol = [&](QString title, QWidget* widget) {
        QVBoxLayout* col = new QVBoxLayout();
        col->setSpacing(6);
        QLabel* lbl = new QLabel(title);
        lbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
        col->addWidget(lbl);
        col->addWidget(widget);
        filterRowLayout->addLayout(col, 1);
    };

    fromSearchCombo_ = new AirportComboBox(airportManager_);
    addFilterCol("Điểm đi", fromSearchCombo_);

    toSearchCombo_ = new AirportComboBox(airportManager_);
    addFilterCol("Điểm đến", toSearchCombo_);

    dateSearchEdit_ = new QLineEdit(this);
    dateSearchEdit_->setPlaceholderText("DD/MM/YYYY");
    addFilterCol("Ngày khởi hành", dateSearchEdit_);

    airlineFilterCombo_ = new QComboBox(this);
    airlineFilterCombo_->addItem("Tùy chọn", "");
    std::vector<std::string> airlines = Helpers::loadAirlinesFromFile("C:/PBL2/data/airlines.txt");
    for (const std::string& airline : airlines) {
        airlineFilterCombo_->addItem(QString::fromStdString(airline), QString::fromStdString(airline));
    }
    addFilterCol("Hãng hàng không", airlineFilterCombo_);

    // Cột 5: Nút
    QVBoxLayout* col5 = new QVBoxLayout();
    col5->setSpacing(4);
    col5->addStretch();
    
    searchFilterBtn_ = new QPushButton("Tìm kiếm");
    searchFilterBtn_->setProperty("class", "SearchBtn");
    searchFilterBtn_->setMinimumWidth(110);
    searchFilterBtn_->setCursor(Qt::PointingHandCursor);
    searchFilterBtn_->setStyleSheet(
        "QPushButton { background:#4472C4; color:white; font-weight:600; border-radius:6px; padding: 0 16px; height:36px; }"
        "QPushButton:hover { background:#365a9e; }"
    );
    col5->addWidget(searchFilterBtn_);
    filterRowLayout->addLayout(col5);

    asLayout->addLayout(filterRowLayout);
    searchRowLayout->addWidget(advancedSearchBox, 2);

    // Warning label for input errors
    warningLabel_2 = new QLabel(this);
    warningLabel_2->setStyleSheet("color: #FF0000; font-size: 11px; font-weight: 500; border: none; background: transparent;");
    advancedSearchBox->layout()->addWidget(warningLabel_2);
    // Mặc định ẩn
    warningLabel_2->setVisible(false);

    topLayout->addLayout(searchRowLayout);
    mainLayout->addWidget(topBar);

    // ========== HEADER + CRUD ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0); 
    tableHeaderLayout->setSpacing(10);

    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    tableHeaderLayout->addWidget(statusLabel_);
    tableHeaderLayout->addStretch();

    showFutureOnlyCheckbox_ = new QCheckBox("Chỉ hiện chuyến chưa bay", this);
    showFutureOnlyCheckbox_->setStyleSheet(
        "QCheckBox { "
        "   color: #123B7A; "
        "   font-weight: 550; "
        "   font-size: 12px; "
        "}"
        "QCheckBox::indicator { "
        "   width: 12px; "
        "   height: 12px; "
        "   border: 2px solid #133e87; "
        "   border-radius: 6px; "
        "   background: white; "
        "}"
        "QCheckBox::indicator:checked { "
        "   background: #4472C4; "
        "   border-color: #4472C4; "
        "}"
    );
    showFutureOnlyCheckbox_->setCursor(Qt::PointingHandCursor);
    tableHeaderLayout->addWidget(showFutureOnlyCheckbox_);

    sortingCombo_ = new QComboBox(this);
    sortingCombo_->setStyleSheet(
        "QComboBox { "
        "   background: white; "
        "   border: 1px solid #1e3e87; "
        "   border-radius: 2px; "
        "   height: 20px; "
        "   padding-left: 6px; "
        "}"
        // Style cho phần danh sách xổ xuống
        "QComboBox QAbstractItemView { "
        "   background-color: white; "
        "   border: 1px solid #1e3e87; "
        "   selection-background-color: #4472C4; " // Màu nền khi di chuột vào item
        "   selection-color: white; "              // Màu chữ khi di chuột vào item
        "   outline: 0px; "                        // Bỏ đường viền nét đứt
        "}"
);
    sortingCombo_->setCursor(Qt::PointingHandCursor);
    sortingCombo_->addItem("Sắp xếp mặc định");
    sortingCombo_->addItem("Giá: Thấp đến Cao");
    sortingCombo_->addItem("Giá: Cao đến Thấp");
    sortingCombo_->addItem("Giờ bay: Sớm nhất");
    sortingCombo_->addItem("Hãng bay: A-Z");
    tableHeaderLayout->addWidget(sortingCombo_);

    addButton_ = new QPushButton("Thêm chuyến", this);
    editButton_ = new QPushButton("Sửa chuyến", this);
    deleteButton_ = new QPushButton("Xóa chuyến", this);

    addButton_->setStyleSheet("background: #27C93F; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #1b4d1b; color: white;");
    editButton_->setStyleSheet("background: #FFBD2E; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #b35c00; color: white;");
    deleteButton_->setStyleSheet("background: #FF5F57; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #8b1c1c; color: white;");

    tableHeaderLayout->addWidget(addButton_);
    tableHeaderLayout->addWidget(editButton_);
    tableHeaderLayout->addWidget(deleteButton_);
    mainLayout->addWidget(tableHeader);

    // ========== BẢNG ==========
    QWidget *tableBox = new QWidget(this);
    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setObjectName("TableView");
    tableView_->setStyleSheet("#TableView { background: white; border: 1px solid #133e87; }");
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

void FlightsPage::setupModel()
{
    // Giữ nguyên setupModel
    model_ = new QStandardItemModel(0, 9, this);
    model_->setHorizontalHeaderLabels({
        "STT", "Mã chuyến", "Mã tuyến", "Hãng hàng không", "Số hiệu",
        "Ngày khởi hành", "Giờ khởi hành", "Ghế trống", "Giá từ"
    });
    tableView_->setModel(model_);

    QHeaderView *header = tableView_->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView_->setColumnWidth(0, 50);
}

void FlightsPage::setupConnections()
{
    // Search
    connect(searchByIdBtn_, &QPushButton::clicked, this, &FlightsPage::onSearchById);
    connect(searchFilterBtn_, &QPushButton::clicked, this, &FlightsPage::onSearchFilter);

    // Sort & Refresh
    connect(sortingCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FlightsPage::onSortChanged);
    connect(refreshButton, &QPushButton::clicked, this, &FlightsPage::refreshPage);
    connect(showFutureOnlyCheckbox_, &QCheckBox::toggled, this, &FlightsPage::onFutureFilterChanged);

    // CRUD
    connect(addButton_, &QPushButton::clicked, this, &FlightsPage::onAddFlight);
    connect(editButton_, &QPushButton::clicked, this, &FlightsPage::onEditFlight);
    connect(deleteButton_, &QPushButton::clicked, this, &FlightsPage::onDeleteFlight);
}

// XỬ LÝ LỌC CHUYẾN CHƯA BAY
void FlightsPage::onFutureFilterChanged(bool checked)
{
    bool onlyFuture = checked;
    
    // Lấy danh sách flights theo filter
    const std::vector<Flight*> flights = 
        flightManager_->getFutureFlights(onlyFuture);
    
    currentFlights_ = flights;
    fillTable(currentFlights_);
    
    // Cập nhật status
    QString status = onlyFuture 
        ? QString("Hiển thị %1 chuyến bay chưa khởi hành").arg(flights.size())
        : QString("Hiển thị tất cả %1 chuyến bay").arg(flights.size());
    
    statusLabel_->setText(status);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    
    // Reset sort về mặc định
    sortingCombo_->setCurrentIndex(0);
}

// === HÀM QUAN TRỌNG: FILL TABLE (DÙNG CHUNG CHO TẤT CẢ) ===
void FlightsPage::fillTable(const std::vector<Flight*>& flights)
{
    model_->removeRows(0, model_->rowCount());
    SeatManager* seatManager = flightManager_->getSeatManager();
    
    // Biến đếm STT
    int stt = 1;

    for (Flight* flight : flights) {
        if (!flight) continue;

        // Tính toán ghế trống (Logic nghiệp vụ của bạn)
        seatManager->loadSeatMapFor(flight);
        int available = seatManager->getAvailableSeats();
        QString priceFormatted = formatVietnamCurrency(flight->getFareEconomy());

        QList<QStandardItem *> rowItems;
        
        // 0. STT
        rowItems << new QStandardItem(QString::number(stt++));
        // 1. ID
        rowItems << new QStandardItem(QString::fromStdString(flight->getFlightId()));
        // 2. Mã tuyến
        rowItems << new QStandardItem(QString::fromStdString(flight->getRouteId()));
        // 3. Hãng
        rowItems << new QStandardItem(QString::fromStdString(flight->getAirline()));
        // 4. Số hiệu
        rowItems << new QStandardItem(QString::fromStdString(flight->getFlightNumber()));
        // 5. Ngày đi
        rowItems << new QStandardItem(QString::fromStdString(flight->getDepartureDate()));
        // 6. Giờ đi
        rowItems << new QStandardItem(QString::fromStdString(flight->getDepartureTime()));
        // 7. Ghế
        rowItems << new QStandardItem(QString::number(available) + " / " + QString::number(flight->getTotalCapacity()));
        // 8. Giá
        rowItems << new QStandardItem(priceFormatted);
        
        // Canh giữa
        for (auto* item : rowItems) item->setTextAlignment(Qt::AlignCenter);

        model_->appendRow(rowItems);
    }
}

// === LOAD ALL FLIGHTS (Thay thế refreshTable cũ) ===
void FlightsPage::loadAllFlights()
{
    bool onlyFuture = showFutureOnlyCheckbox_->isChecked();
    
    const std::vector<Flight*> flights = 
        flightManager_->getFutureFlights(onlyFuture);
    
    currentFlights_ = flights;
    fillTable(currentFlights_);
    
    QString status = onlyFuture 
        ? QString("Hiển thị %1 chuyến bay chưa khởi hành").arg(flights.size())
        : QString("Hiển thị tất cả %1 chuyến bay").arg(flights.size());
    
    statusLabel_->setText(status);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
}

void FlightsPage::refreshPage() {
    PageRefresher::clearSearchFields(this);

    // Reset checkbox về unchecked
    showFutureOnlyCheckbox_->setChecked(false);
    sortingCombo_->setCurrentIndex(0);

    PageRefresher::executeRefresh([this]() {
        loadAllFlights();
    });
}

// === XỬ LÝ TÌM KIẾM (Đã dùng fillTable) ===
void FlightsPage::onSearchById()
{
    QString flightId = idSearchEdit_->text().trimmed();
    if (flightId.isEmpty()) {
        warningLabel_1->setText("Vui lòng nhập mã chuyến bay để tìm kiếm!");
        warningLabel_1->setVisible(true);
        QTimer::singleShot(4000, [this]() { warningLabel_1->setVisible(false); });
        return;
    }

    Flight* flight = flightManager_->findFlightById(flightId.toStdString());
    currentFlights_.clear();
    
    if (flight) {
        currentFlights_.push_back(flight);
        fillTable(currentFlights_); // Dùng hàm chung
        statusLabel_->setText(QString("Tìm thấy 1 chuyến bay với mã <b>%1</b>").arg(flightId));
        statusLabel_->setStyleSheet("color: #27C93F; font-size: 13px; font-weight: 650;");
    } else {
        fillTable(currentFlights_); // Bảng rỗng
        statusLabel_->setText(QString("Không tìm thấy chuyến bay mã <b>%1</b>!").arg(flightId));
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
    }
}

void FlightsPage::onSearchFilter()
{
    FlightManager::SearchCriteria criteria;
    criteria.fromIATA = fromSearchCombo_->getSelectedIATA();
    criteria.toIATA = toSearchCombo_->getSelectedIATA();

    // --- LOGIC MỚI: BẮT BUỘC CẢ 2 HOẶC KHÔNG CẦN CẢ 2 ---
    bool hasFrom = !criteria.fromIATA.empty();
    bool hasTo = !criteria.toIATA.empty();

    // Nếu chỉ có From mà thiếu To, hoặc chỉ có To mà thiếu From
    if (hasFrom != hasTo) {
        warningLabel_2->setText("Vui lòng chọn cả điểm đi và điểm đến để tìm kiếm.");
        warningLabel_2->setVisible(true);
        QTimer::singleShot(4000, [this]() {
            warningLabel_2->setVisible(false);
        });
        return; // Dừng lại, không tìm kiếm
    }
    
    QString dateText = dateSearchEdit_->text().trimmed();
    if (!dateText.isEmpty()) {
        QDate selectedDate = QDate::fromString(dateText, "dd/MM/yyyy");
        if (selectedDate.isValid()) {
            criteria.date = selectedDate.toString("dd/MM/yyyy").toStdString();
        } else {
            warningLabel_2->setText("Ngày khởi hành không hợp lệ (dd/MM/yyyy)!");
            warningLabel_2->setVisible(true);
            QTimer::singleShot(4000, [this]() { warningLabel_2->setVisible(false); });
            return;
        }
    }
    
    if (airlineFilterCombo_->currentIndex() > 0) {
        criteria.airline = airlineFilterCombo_->currentData().toString().toStdString();
    }
    
    auto results = flightManager_->searchFlights(criteria);
    currentFlights_ = results; // Lưu kết quả
    
    fillTable(currentFlights_); // Dùng hàm chung
    
    if (results.empty()) {
        statusLabel_->setText("Không tìm thấy chuyến bay phù hợp.");
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
    } else {
        statusLabel_->setText(QString("Tìm thấy %1 chuyến bay phù hợp.").arg(results.size()));
        statusLabel_->setStyleSheet("color: #27C93F; font-size: 13px; font-weight: 650;");
    }
}

// === LOGIC SORT (Đã hoạt động tốt nhờ currentFlights_) ===
void FlightsPage::onSortChanged(int index)
{
    if (currentFlights_.empty()) return;

    std::vector<Flight*> sortedList;

    switch(index) {
        case 0: // Mặc định
            sortedList = currentFlights_; 
            break;
        case 1: // Giá tăng
            sortedList = Sorting::sortByPrice(currentFlights_);
            break;
        case 2: // Giá giảm
            sortedList = Sorting::sortByPrice(currentFlights_);
            std::reverse(sortedList.begin(), sortedList.end());
            break;
        case 3: // Giờ sớm nhất
            sortedList = Sorting::sortByArrivalTime(currentFlights_); 
            break;
        case 4: // Hãng A-Z
            sortedList = Sorting::sortByAirline(currentFlights_);
            break;
    }

    fillTable(sortedList);    
}

// === CRUD (Chỉ cần gọi loadAllFlights sau khi xong) ===
void FlightsPage::onAddFlight()
{
    FlightDialog dialog(flightManager_, airportManager_, this);
    if (dialog.exec() == QDialog::Accepted) {
        // ... (Logic lấy dữ liệu từ dialog giữ nguyên) ...
        QString flightId = dialog.getFlightId();
        // ...
        
        bool success = flightManager_->createNewFlight(
            dialog.getFlightId().toStdString(),
            dialog.getAirline().toStdString(),
            dialog.getFlightNumber().toStdString(),
            dialog.getDepartureDate().toStdString(),
            dialog.getDepartureTime().toStdString() + ":00",
            dialog.getArrivalDate().toStdString(),
            dialog.getArrivalTime().toStdString() + ":00",
            dialog.getTotalCapacity(),
            dialog.getTotalCapacity(),
            dialog.getFareEconomy(),
            dialog.getFareBusiness()
        );
        
        if (success) {
            loadAllFlights(); // Refresh lại toàn bộ
            QMessageBox::information(this, "Thành công", "Đã thêm chuyến bay: " + flightId);
        } else {
            QMessageBox::critical(this, "Thất bại", "Không thể thêm chuyến bay.");
        }
    }
}

void FlightsPage::onEditFlight()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        statusLabel_->setText("Vui lòng chọn một chuyến bay để sửa.");
        return;
    }
    QString flightId = model_->item(selected.first().row(), 1)->text();
    Flight* flight = flightManager_->findFlightById(flightId.toStdString());
    if (!flight) return;
    
    // ... (Logic tạo dialog giữ nguyên) ...
    Route* route = flightManager_->findRouteById(flight->getRouteId());
    FlightDialog dialog(flightManager_, airportManager_,
                       flightId,
                       QString::fromStdString(flight->getFlightNumber()),
                       QString::fromStdString(flight->getAirline()),
                       QString::fromStdString(route->getDepartureAirport()),
                       QString::fromStdString(route->getArrivalAirport()),
                       QString::fromStdString(flight->getDepartureDate()),
                       QString::fromStdString(flight->getDepartureTime()),
                       QString::fromStdString(flight->getArrivalDate()),
                       QString::fromStdString(flight->getArrivalTime()),
                       flight->getAvailableSeats(),
                       flight->getTotalCapacity(),
                       flight->getFareEconomy(),
                       flight->getFareBusiness(),
                       this);

    if (dialog.exec() == QDialog::Accepted) {
        Flight updatedFlight(
            dialog.getRouteId().toStdString(),
            dialog.getAirline().toStdString(),
            dialog.getFlightNumber().toStdString(),
            dialog.getDepartureDate().toStdString(),
            (dialog.getDepartureTime() + ":00").toStdString(),
            dialog.getArrivalDate().toStdString(),
            (dialog.getArrivalTime() + ":00").toStdString(),
            dialog.getAvailableSeats(),
            dialog.getTotalCapacity(),
            dialog.getFareEconomy(),
            dialog.getFareBusiness()
        );
        updatedFlight.overrideIdForLoad(flightId.toStdString());
        
        if (flightManager_->updateFlight(flightId.toStdString(), updatedFlight)) {
            QMessageBox::information(this, "Thành công", "Đã cập nhật chuyến bay: " + flightId);
            loadAllFlights(); // Refresh
        }
    }
}

void FlightsPage::onDeleteFlight()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    QString flightId = model_->item(selected.first().row(), 1)->text();
    auto reply = QMessageBox::question(this, "Xác nhận xóa", 
        "Bạn có chắc chắn muốn xóa chuyến bay " + flightId + "?", 
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (flightManager_->deleteFlight(flightId.toStdString())) {
            QMessageBox::information(this, "Thành công", "Đã xóa chuyến bay.");
            loadAllFlights(); // Refresh
        } else {
            QMessageBox::critical(this, "Lỗi", "Không thể xóa chuyến bay.");
        }
    }
}