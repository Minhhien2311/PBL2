#include "SearchBookPage.h"
#include "utils/Sorting.h"
#include "utils/DateTime.h"

// Core & entities
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/AccountManager.h"
#include "core/AirportManager.h"
#include "entities/Flight.h"
#include "entities/Flight.h"
#include "entities/Account.h"
#include "BookingDialog.h"
#include "AirportComboBox.h"
#include "utils/Helpers.h"
#include "BoldItemDelegate.h"
#include "PageRefresher.h"
#include <string>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer> // Required for QTimer
#include <QCalendarWidget>
#include <QComboBox>
#include <QIntValidator>
#include <QLocale>

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

// Constants
namespace {
    constexpr int MAX_FLIGHT_PRICE = 99999999;  // Maximum flight price value for validation (business constraint)
}

SearchBookPage::SearchBookPage(FlightManager* flManager,
                               BookingManager* bkManager,
                               AccountManager* accManager,
                               AirportManager* airportManager,
                               PassengerManager* passengerManager,
                               QWidget* parent)
    : QWidget(parent),
      flightManager_(flManager),
      bookingManager_(bkManager),
      accountManager_(accManager),
      airportManager_(airportManager),
      passengerManager_(passengerManager)
{
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);

    setupUi();          // vẽ UI giống FlightsPage
    setupModel();       // tạo model bảng
    setupConnections(); // nối signal-slot
    loadAllFlights();  // nạp tất cả chuyến bay ban đầu
}

void SearchBookPage::setupUi()
{
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLineEdit, QDateEdit, QComboBox, QSpinBox { background:white; border:1px solid #608bc1; "
            "border-radius:4px; height:26px; padding-left:6px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; "
            "height:24px; font-weight:600; }"
    );

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    

    // ================== TOP BAR ==================
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 10);
    topLayout->setSpacing(14);
    topLayout->addStretch();

    // === Hàng 1: Nút Tải lại ===
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);
    headerRow->addStretch();

    QPushButton* refreshButton = new QPushButton("Làm mới trang", topBar);
    
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
    
    headerRow->addWidget(refreshButton);
    topLayout->addLayout(headerRow);

    // Kết nối nút refresh
    connect(refreshButton, &QPushButton::clicked, this, &SearchBookPage::refreshPage);

    // ========== KHUNG TÌM KIẾM (BO VIỀN TRẮNG) ==========
    QWidget* searchBox = new QWidget;
    QVBoxLayout* searchBoxLayout = new QVBoxLayout(searchBox);
    searchBoxLayout->setContentsMargins(12, 12, 12, 12);
    searchBoxLayout->setSpacing(10);
    
    searchBox->setStyleSheet(
        "QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }"
    );

    QLabel* searchTitle = new QLabel("🔎 Tìm kiếm chuyến bay theo nhiều tiêu chí");
    searchTitle->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    searchBoxLayout->addWidget(searchTitle);

    // === FILTER BAR LAYOUT ===
    QGridLayout* filterLayout = new QGridLayout();
    filterLayout->setHorizontalSpacing(12);
    filterLayout->setVerticalSpacing(8);

    // --- Hàng 0: Nhãn (Labels) ---
    QLabel* fromLbl = new QLabel("Điểm đi");
    fromLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(fromLbl, 0, 0);
    
    QLabel* toLbl = new QLabel("Điểm đến");
    toLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(toLbl, 0, 1);
    
    QLabel* dateLbl = new QLabel("Ngày khởi hành");
    dateLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(dateLbl, 0, 2);
    
    QLabel* airlineLbl = new QLabel("Hãng hàng không");
    airlineLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(airlineLbl, 0, 3);
    
    // Nhãn "Khoảng giá" span 3 cột (4, 5, 6)
    QLabel* priceLbl = new QLabel("Khoảng giá mong muốn");
    priceLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(priceLbl, 0, 4, 1, 3);

    // --- Hàng 1: Ô nhập liệu (Inputs) ---

    // From dropdown (Cột 0)
    fromSearchCombo_ = new AirportComboBox(airportManager_, this);
    fromSearchCombo_->setMinimumHeight(36);
    filterLayout->addWidget(fromSearchCombo_, 1, 0);

    // To dropdown (Cột 1)
    toSearchCombo_ = new AirportComboBox(airportManager_, this);
    toSearchCombo_->setMinimumHeight(36);
    filterLayout->addWidget(toSearchCombo_, 1, 1);

    // Date picker (Dùng QLineEdit)
    dateSearchEdit_ = new QLineEdit(this);
    dateSearchEdit_->setPlaceholderText("DD/MM/YYYY"); // Gợi ý định dạng
    dateSearchEdit_->setMinimumHeight(36);
    filterLayout->addWidget(dateSearchEdit_, 1, 2);


    // Airline dropdown
    airlineFilterCombo_ = new QComboBox(this);
    airlineFilterCombo_->addItem("Tùy chọn", "");
    std::vector<std::string> airlines = Helpers::loadAirlinesFromFile("C:/PBL2/data/airlines.txt");
    for (const std::string& airline : airlines) {
        airlineFilterCombo_->addItem(QString::fromStdString(airline), 
                                      QString::fromStdString(airline));
    }
    airlineFilterCombo_->setMinimumHeight(36);
    filterLayout->addWidget(airlineFilterCombo_, 1, 3);

    // Price min
    priceMinEdit_ = new QLineEdit(this);
    priceMinEdit_->setPlaceholderText("Tùy chọn");
    priceMinEdit_->setValidator(new QIntValidator(0, MAX_FLIGHT_PRICE, this));
    priceMinEdit_->setMinimumHeight(36);
    filterLayout->addWidget(priceMinEdit_, 1, 4);

    // Dash separator
    QLabel* dashLabel = new QLabel("—");
    dashLabel->setAlignment(Qt::AlignCenter);
    dashLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(dashLabel, 1, 5);

    // Price max
    priceMaxEdit_ = new QLineEdit(this);
    priceMaxEdit_->setPlaceholderText("Tùy chọn");
    priceMaxEdit_->setValidator(new QIntValidator(0, MAX_FLIGHT_PRICE, this));
    priceMaxEdit_->setMinimumHeight(36);
    filterLayout->addWidget(priceMaxEdit_, 1, 6);

    // Search button (Cột 7)
    QPushButton* searchBtn = new QPushButton("Tìm kiếm", this);
    searchBtn->setMinimumHeight(36);
    searchBtn->setMinimumWidth(110);
    searchBtn->setCursor(Qt::PointingHandCursor);
    searchBtn->setStyleSheet(
        "QPushButton { background-color: #4472C4; color: white; padding: 0 20px; "
        "border-radius: 6px; font-weight: 600; }"
        "QPushButton:hover { background-color: #365a9e; }"
    );
    filterLayout->addWidget(searchBtn, 1, 7);

    // Set column stretch
    filterLayout->setColumnStretch(0, 1);
    filterLayout->setColumnStretch(1, 1);
    filterLayout->setColumnStretch(2, 1);
    filterLayout->setColumnStretch(3, 1);
    filterLayout->setColumnStretch(7, 1);

    // Thêm layout vào searchBox
    searchBoxLayout->addLayout(filterLayout);

    // Warning label for input errors
    warningLabel_ = new QLabel(this);
    warningLabel_->setStyleSheet("color: #FF0000; font-size: 11px; font-weight: 500; border: none; background: transparent;");
    searchBoxLayout->addWidget(warningLabel_);
    // Mặc định ẩn
    warningLabel_->setVisible(false);

    // Thêm searchBox vào topLayout
    topLayout->addWidget(searchBox);

    // Connect search button
    connect(searchBtn, &QPushButton::clicked, this, &SearchBookPage::onSearchClicked);

    mainLayout->addWidget(topBar);

    // ========== TIÊU ĐỀ BẢNG + STATUS + NÚT ĐẶT VÉ (GỘP CHUNG 1 HÀNG) ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *thLayout = new QHBoxLayout(tableHeader);
    // Giữ margin để căn lề chuẩn với bên trên
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(10);

    // 1. Status label
    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    thLayout->addWidget(statusLabel_);

    // 2. Lò xo đẩy nút sang phải
    thLayout->addStretch();

    // 3. Nút chọn tiêu chí sắp xếp
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
    
    // Thêm các lựa chọn sắp xếp
    sortingCombo_->addItem("Sắp xếp mặc định");
    sortingCombo_->addItem("Giá: Thấp đến Cao");
    sortingCombo_->addItem("Giá: Cao đến Thấp");
    sortingCombo_->addItem("Giờ bay: Sớm nhất");
    sortingCombo_->addItem("Hãng bay: A-Z");

    thLayout->addWidget(sortingCombo_);

    // 4. Nút Đặt vé
    bookButton_ = new QPushButton("Đặt vé cho chuyến đã chọn", this);
    bookButton_->setCursor(Qt::PointingHandCursor);

    // Style gọn nhẹ (Ghost style) giống trang RoutesPage
    QString btnStyle =
        "QPushButton { background: #27C93F; color: white; border:1px solid #1fab33; "
        "border-radius:6px; height:20px; padding:4px 12px; font-weight:600; }"
        "QPushButton:hover { background: #1fab33; color: white; }";

    bookButton_->setStyleSheet(btnStyle);

    thLayout->addWidget(bookButton_);

    // Add Header vào Main Layout
    mainLayout->addWidget(tableHeader);

    // ================== BẢNG ==================
    QWidget *tableBox = new QWidget(this);
    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setStyleSheet(
        "QTableView { background:white; border:0px solid #133e87; }"
    );
    tableView_->setItemDelegate(new BoldItemDelegate(this));

    // --- Cấu hình cơ bản ---
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->verticalHeader()->setVisible(false);
    tableView_->setAlternatingRowColors(true);
    tableView_->setShowGrid(false); 

    // QUAN TRỌNG: Tắt hẳn scrollbar ngang (ép không cho hiện)
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);
}

void SearchBookPage::setupModel()
{
    // 8 cột thông tin chuyến bay
    // 1. Tạo Model và Header Labels
    model_ = new QStandardItemModel(0, 9, this);
    model_->setHorizontalHeaderLabels({
        "STT", "Mã Chuyến", "Mã tuyến", "Hãng hàng không", "Số hiệu",
        "Ngày khởi hành", "Giờ khởi hành", "Ghế trống", "Giá từ"
    });

    // 2. Gán model
    tableView_->setModel(model_);

    // 3. CẤU HÌNH HEADER (QUAN TRỌNG)
    QHeaderView *header = tableView_->horizontalHeader();

    // A. Mặc định cho tất cả co sát theo nội dung chữ
    header->setSectionResizeMode(QHeaderView::Stretch);

    // B. Cột STT (0): Cố định nhỏ
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView_->setColumnWidth(0, 50); // Tăng lên 50 xíu cho số 100 đỡ bị che
    
    // Lưu ý: KHÔNG setColumnWidth cho cột Stretch (nó tự tính)
}

void SearchBookPage::setupConnections()
{
    // đặt vé
    connect(bookButton_, &QPushButton::clicked, this, &SearchBookPage::onBookClicked);
    // Kết nối tín hiệu thay đổi lựa chọn
    connect(sortingCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &SearchBookPage::onSortChanged);
}

// ================ CHỖ NẠP DỮ LIỆU VÀO BẢNG ================
void SearchBookPage::fillTable(const std::vector<Flight*>& flights)
{
    model_->removeRows(0, model_->rowCount());

    for (int i = 0; i < flights.size(); ++i) {
        Flight* flight = flights[i];

        if (!flight) continue;
        QString airline = QString::fromStdString(flight->getAirline());

        // Format số tiền với dấu chấm phân cách hàng nghìn
        QString priceFormatted = formatVietnamCurrency(flight->getFareEconomy());

        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(i + 1))
            << new QStandardItem(QString::fromStdString(flight->getFlightId()))
            << new QStandardItem(QString::fromStdString(flight->getRouteId()))
            << new QStandardItem(airline)
            << new QStandardItem(QString::fromStdString(flight->getFlightNumber()))
            << new QStandardItem(QString::fromStdString(flight->getDepartureDate()))
            << new QStandardItem(QString::fromStdString(flight->getDepartureTime()))
            << new QStandardItem(QString::number(flight->getAvailableSeats()) + "/" + QString::number(flight->getTotalCapacity()))
            << new QStandardItem(priceFormatted);
        // Canh giữa tất cả các cột
        for (QStandardItem *item : row) {
        item->setTextAlignment(Qt::AlignCenter);
    }
        model_->appendRow(row);
    }
}

// ================ SLOT TÌM KIẾM ================
// Unified search method
void SearchBookPage::onSearchClicked()
{
    FlightManager::SearchCriteria criteria;
    criteria.fromIATA = fromSearchCombo_->getSelectedIATA();
    criteria.toIATA = toSearchCombo_->getSelectedIATA();

    bool hasFrom = !criteria.fromIATA.empty();
    bool hasTo = !criteria.toIATA.empty();

    // Nếu chỉ có From mà thiếu To, hoặc chỉ có To mà thiếu From
    if (hasFrom != hasTo) {
        warningLabel_->setText("Vui lòng chọn cả điểm đi và điểm đến để tìm kiếm.");
        warningLabel_->setVisible(true);
        QTimer::singleShot(4000, [this]() {
            warningLabel_->setVisible(false);
        });
        return; // Dừng lại, không tìm kiếm
    }
    
    // Date (optional) - chỉ filter nếu user chọn ngày cụ thể - Xử lý nhập text
    QString dateText = dateSearchEdit_->text().trimmed();
    if (!dateText.isEmpty()) {
        // Ép kiểu chuỗi nhập vào thành QDate
        QDate selectedDate = QDate::fromString(dateText, "dd/MM/yyyy");

        if (selectedDate.isValid()) {
            criteria.date = selectedDate.toString("dd/MM/yyyy").toStdString();
        } else {
            // Nếu nhập sai định dạng (ví dụ nhập chữ linh tinh)
            warningLabel_->setText("Vui lòng nhập ngày đúng định dạng: ngày/tháng/năm (ví dụ: 25/12/2025)");
            warningLabel_->setVisible(true);
            QTimer::singleShot(4000, [this]() {
                warningLabel_->setVisible(false);
            });
            return; // Dừng tìm kiếm
        }
    }
    
    // Airline (optional)
    if (airlineFilterCombo_->currentIndex() > 0) {
        criteria.airline = airlineFilterCombo_->currentData().toString().toStdString();
    }
    
    // Price range from QLineEdit (optional)
    QString minText = priceMinEdit_->text().trimmed();
    QString maxText = priceMaxEdit_->text().trimmed();
    
    if (!minText.isEmpty()) {
        bool ok;
        int minPrice = minText.toInt(&ok);
        if (ok && minPrice >= 0) {  // Allow 0 for free/promotional flights
            criteria.minPrice = minPrice;
        }
    }
    
    if (!maxText.isEmpty()) {
        bool ok;
        int maxPrice = maxText.toInt(&ok);
        if (ok && maxPrice >= 0) {  // Allow 0 for consistency
            criteria.maxPrice = maxPrice;
        }
    }
    
    // Search with criteria
    auto results = flightManager_->searchFlights(criteria);

    // LỌC THÊM CHỈ LẤY CHUYẾN TƯƠNG LAI
    time_t now = utils::DateTime::toUnix(utils::DateTime::nowUtc());
    std::vector<Flight*> futureResults;
    
    for (Flight* flight : results) {
        time_t flightTime = utils::DateTime::toUnix(
            utils::DateTime::fromDmYHm(
                flight->getDepartureDate(), 
                flight->getDepartureTime()
            )
        );
        if (flightTime >= now) {
            futureResults.push_back(flight);
        }
    }
    
    currentFlights_ = futureResults;
    // Display results
    fillTable(futureResults);
    
    // Update status
    statusLabel_->setText(
        QString("Tìm thấy %1 chuyến bay").arg(futureResults.size())
    );
}

// ================ ĐẶT VÉ ================
void SearchBookPage::onBookClicked()
{
    // Lấy dòng đang chọn
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Chọn một chuyến bay trước khi đặt vé.");
        return;
    }

    // Lấy flightId từ cột 0
    QString flightId = model_->itemFromIndex(selected.first().siblingAtColumn(1))->text();
    
    // Lấy thông tin chuyến bay
    Flight* flight = flightManager_->findFlightById(flightId.toStdString());
    if (!flight) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy chuyến bay.");
        return;
    }
    
    // Hiển thị dialog đặt vé (dialog handles everything internally)
    BookingDialog dialog(flight, flightManager_, bookingManager_, accountManager_, passengerManager_, airportManager_, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Booking already created and saved inside dialog!
        QMessageBox::information(this, "Thành công", "Đặt vé thành công!");
        
        // Reload table to show updated seat availability
        onSearchClicked();
    }
}

void SearchBookPage::loadAllFlights()
{
    // CHỈ LẤY CHUYẾN TƯƠNG LAI (onlyFuture = true)
    const std::vector<Flight*> flights = flightManager_->getFutureFlights(true);
    // LƯU KẾT QUẢ VÀO BIẾN THÀNH VIÊN
    currentFlights_ = flights;
    
    // Display them in the table
    fillTable(flights);
    
    // Update status label
    statusLabel_->setText(
        QString("Hiển thị tất cả %1 chuyến bay").arg(flights.size())
    );
}

// LOGIC CHO HÀM SORT
void SearchBookPage::onSortChanged(int index)
{
    if (currentFlights_.empty()) return;

    std::vector<Flight*> sortedList;

    switch(index) {
        case 0: // Mặc định (Theo ID hoặc thứ tự gốc)
            // Load lại theo thứ tự search ban đầu
            sortedList = currentFlights_; 
            break;

        case 1: // Giá: Thấp đến Cao
            sortedList = Sorting::sortByPrice(currentFlights_);
            break;

        case 2: // Giá: Cao đến Thấp
            // Sorting::sortByPrice trả về Thấp->Cao, ta đảo ngược vector lại
            sortedList = Sorting::sortByPrice(currentFlights_);
            std::reverse(sortedList.begin(), sortedList.end());
            break;

        case 3: // Giờ đi: Sớm nhất
            sortedList = Sorting::sortByArrivalTime(currentFlights_);
            break;
        case 4: // Hãng bay: A-Z
            sortedList = Sorting::sortByAirline(currentFlights_);
            break;
    }

    // Hiển thị lại bảng với danh sách đã sắp xếp
    fillTable(sortedList);
}

void SearchBookPage::refreshPage() {
    PageRefresher::clearSearchFields(this);
    PageRefresher::executeRefresh([this]() {
        loadAllFlights();
    });
}
