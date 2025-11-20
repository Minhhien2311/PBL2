#include "FlightsPage.h"
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
#include <QCalendarWidget> 

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


FlightsPage::FlightsPage(FlightManager* flightManager, AirportManager* airportManager, QWidget *parent)
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

void FlightsPage::setupUi()
{
    // style chung giống dashboard / routes
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:16px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; height:30px; font-weight:600; }"
        "QLineEdit { background:white; border:1px solid #608bc1; border-radius:4px; height:26px; padding-left:6px; }"
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
    topLayout->addStretch();

    // === Hàng 1: Nút Tải lại (DI CHUYỂN LÊN ĐÂY) ===
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);

    // ← NÚT TẢI LẠI (góc phải trên)
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
    connect(refreshButton, &QPushButton::clicked, this, &FlightsPage::refreshTable);

    // ========== HÀNG TÌM KIẾM (2 BOX NGANG) ==========
    QHBoxLayout* searchRowLayout = new QHBoxLayout();
    searchRowLayout->setSpacing(16);

    // ========== BOX 1: TRA CỨU NHANH (XẾP DỌC) ==========
    QWidget* quickSearchBox = new QWidget;
    
    // 1. Sử dụng QVBoxLayout để xếp các phần tử từ trên xuống dưới
    QVBoxLayout* qsLayout = new QVBoxLayout(quickSearchBox);
    qsLayout->setContentsMargins(15, 15, 15, 15); // Padding xung quanh
    qsLayout->setSpacing(10); // Khoảng cách giữa Title - Input - Button
    
    quickSearchBox->setStyleSheet(
        "QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }"
    );
    quickSearchBox->setMinimumWidth(250);

    // 3. Ô Input (Nằm giữa)
    idSearchEdit_ = new QLineEdit;
    idSearchEdit_->setPlaceholderText("VD: FI-000001");
    // Không setFixedWith để nó tự giãn theo chiều rộng của box (do tiêu đề quy định)
    qsLayout->addWidget(idSearchEdit_);

    // 4. Nút Tìm kiếm (Nằm dưới cùng)
    searchByIdBtn_ = new QPushButton("Tìm kiếm nhanh bằng ID");
    searchByIdBtn_->setProperty("class", "SearchBtn");
    searchByIdBtn_->setMinimumHeight(34);
    searchByIdBtn_->setCursor(Qt::PointingHandCursor);
    searchByIdBtn_->setStyleSheet(
        "QPushButton { background:#4478BD; color:white; font-weight:600; "
        "border-radius:6px; }" // Border radius đồng bộ
        "QPushButton:hover { background:#365a9e; }"
    );
    // Button mặc định sẽ giãn full chiều ngang trong QVBoxLayout, đúng ý bạn muốn
    qsLayout->addWidget(searchByIdBtn_);

    // 5. Xử lý Layout cha
    // Set Policy Maximum để box co lại vừa khít nội dung, không giãn ra vô tận
    quickSearchBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Add vào layout cha, căn trái (hoặc giữa tùy ý)
    searchRowLayout->addWidget(quickSearchBox, 0, Qt::AlignLeft);

    // ========== BOX 2: TÌM KIẾM NÂNG CAO ==========
    QWidget* advancedSearchBox = new QWidget;
    QVBoxLayout* asLayout = new QVBoxLayout(advancedSearchBox);
    asLayout->setContentsMargins(12, 12, 12, 12);
    asLayout->setSpacing(10);
    
    advancedSearchBox->setStyleSheet(
        "QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }"
    );

    QLabel* asTitle = new QLabel("🔎 Tìm kiếm nâng cao theo nhiều tiêu chí");
    asTitle->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    asLayout->addWidget(asTitle);

    // ← LAYOUT NGANG: Labels + Inputs + Nút tìm kiếm
    QHBoxLayout* filterRowLayout = new QHBoxLayout();
    filterRowLayout->setSpacing(12);

    // === Cột 1: Điểm đi ===
    QVBoxLayout* col1 = new QVBoxLayout();
    col1->setSpacing(6);
    QLabel* fromLabel = new QLabel("Điểm đi");
    fromLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");  // ← Bỏ viền
    col1->addWidget(fromLabel);
    fromSearchCombo_ = new AirportComboBox(airportManager_);
    col1->addWidget(fromSearchCombo_);
    filterRowLayout->addLayout(col1, 1);

    // === Cột 2: Điểm đến ===
    QVBoxLayout* col2 = new QVBoxLayout();
    col2->setSpacing(6);
    QLabel* toLabel = new QLabel("Điểm đến");
    toLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");  // ← Bỏ viền
    col2->addWidget(toLabel);
    toSearchCombo_ = new AirportComboBox(airportManager_);
    col2->addWidget(toSearchCombo_);
    filterRowLayout->addLayout(col2, 1);

    // === Cột 3: Ngày khởi hành ===
    QVBoxLayout* col3 = new QVBoxLayout();
    col3->setSpacing(6);
    QLabel* dateLabel = new QLabel("Ngày khởi hành");
    dateLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");  // ← Bỏ viền
    col3->addWidget(dateLabel);
    dateSearchEdit_ = new QDateEdit(this);
    dateSearchEdit_->setCalendarPopup(true);
    dateSearchEdit_->setDisplayFormat("dd/MM/yyyy");
    dateSearchEdit_->setSpecialValueText("Tùy chọn");
    QDate oneDayAgo = QDate::currentDate().addDays(-1);
    dateSearchEdit_->setMinimumDate(oneDayAgo);
    dateSearchEdit_->clear();
    col3->addWidget(dateSearchEdit_);
    filterRowLayout->addLayout(col3, 1);

    // === Cột 4: Hãng hàng không ===
    QVBoxLayout* col4 = new QVBoxLayout();
    col4->setSpacing(6);
    QLabel* airlineLabel = new QLabel("Hãng hàng không");
    airlineLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");  // ← Bỏ viền
    col4->addWidget(airlineLabel);
    airlineFilterCombo_ = new QComboBox(this);
    airlineFilterCombo_->addItem("Tùy chọn", "");
    std::vector<std::string> airlines = Helpers::loadAirlinesFromFile("C:/PBL2/data/airlines.txt");
    for (const std::string& airline : airlines) {
        airlineFilterCombo_->addItem(QString::fromStdString(airline), 
                                      QString::fromStdString(airline));
    }

    col4->addWidget(airlineFilterCombo_);
    filterRowLayout->addLayout(col4, 1);

    // === Cột 5: Nút tìm kiếm (CÙNG HÀNG) ===
    QVBoxLayout* col5 = new QVBoxLayout();
    col5->setSpacing(4);
    // Thêm label trống để căn nút với các input khác
    QLabel* emptyLabel = new QLabel(" ");  // ← Label trống để căn chỉnh
    emptyLabel->setStyleSheet("background: transparent; border: none;");
    col5->addWidget(emptyLabel);
    
    searchFilterBtn_ = new QPushButton("Tìm kiếm");  // ← Bỏ emoji
    searchFilterBtn_->setProperty("class", "SearchBtn");
    searchFilterBtn_->setMinimumWidth(110);
    searchFilterBtn_->setMinimumHeight(36);
    searchFilterBtn_->setCursor(Qt::PointingHandCursor);
    searchFilterBtn_->setStyleSheet(
        "QPushButton { background:#4472C4; color:white; font-weight:600; "
        "border-radius:6px; padding: 0 16px; }"
        "QPushButton:hover { background:#365a9e; }"
    );
    col5->addWidget(searchFilterBtn_);
    filterRowLayout->addLayout(col5);

    asLayout->addLayout(filterRowLayout);

    searchRowLayout->addWidget(advancedSearchBox, 2);

    topLayout->addLayout(searchRowLayout);
    mainLayout->addWidget(topBar);

    // ========== STATUS + CRUD BAR (GỘP CHUNG 1 HÀNG) ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    // Giữ nguyên margin header cũ
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0); 
    tableHeaderLayout->setSpacing(10);

    // 1. Status Label
    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    tableHeaderLayout->addWidget(statusLabel_);

    // 2. Lò xo đẩy các nút sang phải
    tableHeaderLayout->addStretch();

    // 3. Các nút CRUD
    addButton_ = new QPushButton("Thêm chuyến", this);
    editButton_ = new QPushButton("Sửa chuyến", this);
    deleteButton_ = new QPushButton("Xóa chuyến", this);

    // Style gọn nhẹ (Ghost style) để vừa vặn trên 1 hàng
    QString crudStyle =
        "QPushButton { background:transparent; color: #133e87; border:1px solid #133e87; "
        "border-radius:6px; height:20px; padding:4px 10px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; color: white; }";

    addButton_->setStyleSheet(crudStyle);
    editButton_->setStyleSheet(crudStyle);
    deleteButton_->setStyleSheet(crudStyle);

    // Add trực tiếp vào layout header
    tableHeaderLayout->addWidget(addButton_);
    tableHeaderLayout->addWidget(editButton_);
    tableHeaderLayout->addWidget(deleteButton_);

    // Add Header vào Main Layout
    mainLayout->addWidget(tableHeader);

    // ========== BẢNG ==========
    QWidget *tableBox = new QWidget(this);

    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    
    tblWrap->setContentsMargins(24, 10, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setObjectName("TableView");
    tableView_->setStyleSheet(
        "#TableView { background: white; border: 1px solid #133e87; }"
    );
    tableView_->setItemDelegate(new BoldItemDelegate(this));
    
    // --- STYLE CHUẨN ---
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    tableView_->verticalHeader()->setVisible(false); // Ẩn header dọc
    tableView_->setAlternatingRowColors(true);
    tableView_->setShowGrid(false);

    // --- TẮT SCROLLBAR NGANG ---
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);
}

void FlightsPage::setupModel()
{
    // Thêm STT vào đầu -> Tổng 9 cột
    model_ = new QStandardItemModel(0, 9, this);
    model_->setHorizontalHeaderLabels({
        "STT",              // 0
        "Mã chuyến",        // 1
        "Mã tuyến",         // 2
        "Hãng hàng không",  // 3 (Sẽ Stretch)
        "Số hiệu",          // 4
        "Ngày khởi hành",   // 5
        "Giờ khởi hành",    // 6
        "Ghế trống",        // 7
        "Giá từ"            // 8
    });
    tableView_->setModel(model_);

    QHeaderView *header = tableView_->horizontalHeader();
    
    // 1. Mặc định co gọn
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 2. STT cố định
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView_->setColumnWidth(0, 50);

    // 3. Hãng hàng không (Cột 3) làm LÒ XO
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(7, QHeaderView::Stretch);
    header->setSectionResizeMode(8, QHeaderView::Stretch);
}

void FlightsPage::setupConnections()
{
    // Kết nối các nút tìm kiếm
    connect(searchByIdBtn_, &QPushButton::clicked, this, &FlightsPage::onSearchById);
    connect(searchFilterBtn_, &QPushButton::clicked, this, &FlightsPage::onSearchFilter);

    // Kết nối CRUD
    connect(addButton_, &QPushButton::clicked, this, &FlightsPage::onAddFlight);
    connect(editButton_, &QPushButton::clicked, this, &FlightsPage::onEditFlight);
    connect(deleteButton_, &QPushButton::clicked, this, &FlightsPage::onDeleteFlight);
}

void FlightsPage::refreshTable()
{
    model_->removeRows(0, model_->rowCount());
    const std::vector<Flight*>& flights = flightManager_->getAllFlights();
    SeatManager* seatManager = flightManager_->getSeatManager();
    
    for (int i = 0; i < flights.size(); ++i) {
        Flight* flight = flights[i];
        if (flight) { 
            seatManager->loadSeatMapFor(flight);
            
            QString priceFormatted = formatVietnamCurrency(flight->getFareEconomy());

            QList<QStandardItem *> rowItems;
            
            // 0. STT
            rowItems << new QStandardItem(QString::number(i + 1));
            
            // 1. ID Chuyến
            rowItems << new QStandardItem(QString::fromStdString(flight->getFlightId()));
            
            // 2. Mã tuyến
            rowItems << new QStandardItem(QString::fromStdString(flight->getRouteId()));
            
            // 3. Hãng hàng không
            rowItems << new QStandardItem(QString::fromStdString(flight->getAirline()));
            
            // 4. Số hiệu
            rowItems << new QStandardItem(QString::fromStdString(flight->getFlightNumber()));
            
            // 5,6. Khởi hành
            rowItems << new QStandardItem(QString::fromStdString(flight->getDepartureDate()));
            rowItems << new QStandardItem(QString::fromStdString(flight->getDepartureTime()));
            
            // 7. Ghế
            int available = seatManager->getAvailableSeats();
            rowItems << new QStandardItem(QString::number(available) + " / " + QString::number(flight->getTotalCapacity()));

            // 8. Giá vé
            rowItems << new QStandardItem(priceFormatted);
            
            // Canh giữa
            for (auto* item : rowItems) item->setTextAlignment(Qt::AlignCenter);

            model_->appendRow(rowItems);
        }
    }
    statusLabel_->setText(QString("Hiển thị tất cả %1 chuyến bay").arg(flights.size()));
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
}

void FlightsPage::refreshPage() {
    PageRefresher::clearSearchFields(this);
    PageRefresher::executeRefresh([this]() {
        refreshTable();
    });
}

// === XỬ LÝ CRUD ===
void FlightsPage::onAddFlight()
{
    FlightDialog dialog(flightManager_, airportManager_, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString flightId = dialog.getFlightId();
        QString airline = dialog.getAirline();
        QString flightNumber = dialog.getFlightNumber();
        QString depDate = dialog.getDepartureDate();
        QString depTime = dialog.getDepartureTime();
        QString arrDate = dialog.getArrivalDate();
        QString arrTime = dialog.getArrivalTime();
        int totalCapacity = dialog.getTotalCapacity();
        int fareEconomy = dialog.getFareEconomy();
        int fareBusiness = dialog.getFareBusiness();
        
        // Gọi hàm createNewflight với ĐẦY ĐỦ tham số
        bool success = flightManager_->createNewFlight(
            flightId.toStdString(),
            airline.toStdString(),
            flightNumber.toStdString(),
            depDate.toStdString(),
            depTime.toStdString() + ":00",  // Thêm giây
            arrDate.toStdString(),
            arrTime.toStdString() + ":00",
            totalCapacity,  // Giả sử lúc tạo mới, availableSeats = totalCapacity
            totalCapacity,
            fareEconomy,
            fareBusiness
        );
        
        if (success) {
            // Refresh the table to show the new flight
            refreshTable();

            QMessageBox::information(this, "Thành công", 
                QString("Đã thêm chuyến bay:\n\n"
                       "Mã chuyến: %1\n"
                       "Hãng hàng không: %2\n"
                       "Số hiệu: %3\n"
                       "Tuyến: %4 → %5\n"
                       "Khởi hành: %6 %7\n"
                       "Hạ cánh: %8 %9\n"
                       "Sức chứa: %10 ghế\n"
                       "Giá VT: %11 VNĐ | Giá TC: %12 VNĐ")
                    .arg(flightId)
                    .arg(airline)
                    .arg(flightNumber)
                    .arg(dialog.getFromIATA(), dialog.getToIATA())
                    .arg(depDate, depTime, arrDate, arrTime)
                    .arg(totalCapacity)
                    .arg(fareEconomy)
                    .arg(fareBusiness));
            refreshTable();
        } else {
            QMessageBox::critical(this, "Thất bại", 
                "Không thể thêm chuyến bay.\n\n"
                "Có thể do:\n"
                "• Chuyến bay đã tồn tại\n"
                "• Lỗi lưu dữ liệu");
        }
    }
}

void FlightsPage::onEditFlight()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        statusLabel_->setText("Vui lòng chọn một chuyến bay để sửa.");
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
        return;
    }

    int row = selected.first().row();
    QString flightId = model_->item(row, 1)->text();
    
    Flight* flight = flightManager_->findFlightById(flightId.toStdString());
    if (!flight) {
        statusLabel_->setText("Không tìm thấy chuyến bay!");
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
        return;
    }

    if (flight->getAvailableSeats() < flight->getTotalCapacity()) {
        statusLabel_->setText("Chuyến bay đã có hành khách đặt chỗ, không thể sửa.");
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
        return;
    }
    
    // Lấy thông tin tuyến từ FlightManager, có thể rò rỉ bộ nhớ nếu không quản lý tốt
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
        // Tạo Flight mới với thông tin cập nhật
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
        
        bool success = flightManager_->updateFlight(flightId.toStdString(), updatedFlight);
        
        if (success) {
            QMessageBox::information(this, "Thành công", 
                QString("Đã cập nhật chuyến bay: %1").arg(flightId));
            refreshTable();
        } else {
            QMessageBox::critical(this, "Thất bại", "Không thể cập nhật chuyến bay.");
        }
    }
}

void FlightsPage::onDeleteFlight()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        statusLabel_->setText("Vui lòng chọn một chuyến bay để xóa.");
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
        return;
    }

    QString flightId = model_->item(selected.first().row(), 1)->text();

    auto reply = QMessageBox::question(this, "⚠️ Xác nhận xóa chuyến bay", 
        QString("Bạn có chắc chắn muốn xóa chuyến bay <b>%1</b>?<br><br>"
               "<font color='red'><b>Cảnh báo:</b></font><br>"
               "• Tất cả booking liên quan sẽ bị ảnh hưởng<br>"
               "• Hành động này <b>KHÔNG THỂ</b> hoàn tác")
            .arg(flightId), 
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        bool success = flightManager_->deleteFlight(flightId.toStdString());
        
        if (success) {
            QMessageBox::information(this, "✅ Xóa thành công", 
                QString("Đã xóa chuyến bay mã <b>%1</b>").arg(flightId));
            refreshTable();
        } else {
            QMessageBox::critical(this, "❌ Xóa thất bại", 
                QString("Không thể xóa chuyến bay mã <b>%1</b>.").arg(flightId));
        }
    }
}

// === XỬ LÝ TÌM KIẾM ===
void FlightsPage::onSearchById()
{
    QString flightId = idSearchEdit_->text().trimmed();
    
    if (flightId.isEmpty()) {
        statusLabel_->setText("Vui lòng nhập mã chuyến bay để tìm kiếm!");
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
        return;
    }

    Flight* flight = flightManager_->findFlightById(flightId.toStdString());
    
    if (!flight) {
        // Xóa bảng nếu không tìm thấy
        model_->removeRows(0, model_->rowCount());
        statusLabel_->setText(QString("Không tìm thấy chuyến bay mã <b>%1</b>!").arg(flightId));
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
        return;
    }

    // Reset bảng
    model_->removeRows(0, model_->rowCount());
    
    SeatManager* seatManager = flightManager_->getSeatManager();
    seatManager->loadSeatMapFor(flight);
    int availableSeats = seatManager->getAvailableSeats();
    QString priceFormatted = formatVietnamCurrency(flight->getFareEconomy());
    
    QList<QStandardItem*> rowItems;
    
    // --- BẮT ĐẦU KHỚP CỘT VỚI SETUPMODEL ---
    
    // 0. STT (Vì tìm theo ID chỉ ra 1 kết quả nên STT luôn là 1)
    rowItems << new QStandardItem("1");

    // 1. ID Chuyến
    rowItems << new QStandardItem(QString::fromStdString(flight->getFlightId()));
    
    // 2. Mã tuyến
    rowItems << new QStandardItem(QString::fromStdString(flight->getRouteId()));
    
    // 3. Hãng hàng không (Cột này sẽ Stretch)
    rowItems << new QStandardItem(QString::fromStdString(flight->getAirline()));
    
    // 4. Số hiệu
    rowItems << new QStandardItem(QString::fromStdString(flight->getFlightNumber()));
    
    // 5. Ngày khởi hành
    rowItems << new QStandardItem(QString::fromStdString(flight->getDepartureDate()));
    
    // 6. Giờ khởi hành
    rowItems << new QStandardItem(QString::fromStdString(flight->getDepartureTime()));
    
    // 7. Ghế trống
    rowItems << new QStandardItem(QString::number(flight->getAvailableSeats()) + " / " + QString::number(flight->getTotalCapacity()));

    // 8. Giá vé
    rowItems << new QStandardItem(priceFormatted);

    // --- CANH GIỮA TOÀN BỘ ---
    for (QStandardItem *item : rowItems) {
        item->setTextAlignment(Qt::AlignCenter);
    }

    model_->appendRow(rowItems);

    statusLabel_->setText(QString("Tìm thấy 1 chuyến bay với mã <b>%1</b>").arg(flightId));
    statusLabel_->setStyleSheet("color: #2E7D32; font-size: 13px; font-weight: 650;");
}

void FlightsPage::onSearchFilter()
{
    FlightManager::SearchCriteria criteria;
    
    // Lấy dữ liệu từ UI
    criteria.fromIATA = fromSearchCombo_->getSelectedIATA();
    criteria.toIATA = toSearchCombo_->getSelectedIATA();
    
    if (criteria.fromIATA.empty() || criteria.toIATA.empty()) {
        statusLabel_->setText("Vui lòng chọn cả điểm đi và điểm đến!");
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
    }
    
    // Lấy ngày (nếu user chọn)
    QDate selectedDate = dateSearchEdit_->date();
    // Kiểm tra nếu ngày hợp lệ và không phải ngày quá khứ mặc định
    if (selectedDate.isValid() && selectedDate > QDate::currentDate().addDays(-1)) {
        criteria.date = selectedDate.toString("dd/MM/yyyy").toStdString();
    }
    
    // Lấy hãng bay
    if (airlineFilterCombo_->currentIndex() > 0) {
        criteria.airline = airlineFilterCombo_->currentData().toString().toStdString();
    }
    
    // Gọi Manager tìm kiếm
    auto results = flightManager_->searchFlights(criteria);
    
    // Xóa dữ liệu cũ
    model_->removeRows(0, model_->rowCount());
    
    SeatManager* seatManager = flightManager_->getSeatManager();
    
    // Biến đếm số thứ tự
    int stt = 1;

    for (Flight* inst : results) {
        if (inst) {
            seatManager->loadSeatMapFor(inst);
            int availableSeats = seatManager->getAvailableSeats();
            QString priceFormatted = formatVietnamCurrency(inst->getFareEconomy());
            
            QList<QStandardItem*> rowItems;
            
            // --- BẮT ĐẦU KHỚP CỘT ---
            
            // 0. STT (Tăng dần)
            rowItems << new QStandardItem(QString::number(stt++));
            
            // 1. ID
            rowItems << new QStandardItem(QString::fromStdString(inst->getFlightId()));
            
            // 2. Mã tuyến
            rowItems << new QStandardItem(QString::fromStdString(inst->getRouteId()));
            
            // 3. Hãng
            rowItems << new QStandardItem(QString::fromStdString(inst->getAirline()));
            
            // 4. Số hiệu
            rowItems << new QStandardItem(QString::fromStdString(inst->getFlightNumber()));
            
            // 5. Ngày đi
            rowItems << new QStandardItem(QString::fromStdString(inst->getDepartureDate()));
            
            // 6. Giờ đi
            rowItems << new QStandardItem(QString::fromStdString(inst->getDepartureTime()));
            
            // 7. Ghế
            rowItems << new QStandardItem(QString::number(availableSeats) + " / " + QString::number(inst->getTotalCapacity()));

            // 8. Giá vé
            rowItems << new QStandardItem(priceFormatted);
            
            // --- CANH GIỮA ---
            for (QStandardItem *item : rowItems) {
                item->setTextAlignment(Qt::AlignCenter);
            }

            model_->appendRow(rowItems);
        }
    }
    
    // Cập nhật nhãn trạng thái
    if (results.empty()) {
        statusLabel_->setText("Không tìm thấy chuyến bay phù hợp.");
        statusLabel_->setStyleSheet("color: #C62828; font-size: 13px; font-weight: 650;");
    } else {
        statusLabel_->setText(QString("Tìm thấy %1 chuyến bay phù hợp.").arg(results.size()));
        statusLabel_->setStyleSheet("color: #2E7D32; font-size: 13px; font-weight: 650;");
    }
}