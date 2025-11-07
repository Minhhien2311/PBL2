#include "SearchBookPage.h"

// Core & entities
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/AccountManager.h"
#include "core/AirportManager.h"
#include "entities/FlightInstance.h"
#include "entities/Flight.h"
#include "entities/Account.h"
#include "BookingDialog.h"
#include "AirportComboBox.h"

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
#include <QCalendarWidget>
#include <QComboBox>
#include <QIntValidator>

// Constants
namespace {
    constexpr int MAX_FLIGHT_PRICE = 99999999;  // Maximum flight price value for validation (business constraint)
}

SearchBookPage::SearchBookPage(FlightManager* flManager,
                               BookingManager* bkManager,
                               AccountManager* accManager,
                               AirportManager* airportManager,
                               QWidget* parent)
    : QWidget(parent),
      flightManager_(flManager),
      bookingManager_(bkManager),
      accountManager_(accManager),
      airportManager_(airportManager)
{
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);
    // accountManager_ có thể null nếu bạn chưa truyền – tùy bạn nối API sau

    setupUi();          // vẽ UI giống FlightsPage
    setupModel();       // tạo model bảng
    setupConnections(); // nối signal-slot
}

void SearchBookPage::setupUi()
{
    // --- style chung giống FlightsPage / AgentBookingsPage ---
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLineEdit, QDateEdit, QComboBox, QSpinBox { background:white; border:1px solid #608bc1; "
            "border-radius:4px; height:26px; padding-left:6px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; "
            "height:24px; font-weight:600; }"
        "QTableView { background:white; border:0px; }"
        "QHeaderView::section { background:#d5e2f2; padding:6px; border:1px solid #c2cfe2; }"
    );

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // ================== TOP BAR ==================
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 10);
    topLayout->setSpacing(14);

    // Tiêu đề
    QLabel *title = new QLabel("Tìm chuyến bay để đặt vé", this);
    title->setProperty("class", "PageTitle");
    topLayout->addWidget(title);

    // === FILTER BAR LAYOUT ===

    // Row 1: Labels (above inputs)
    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->setSpacing(15);
    labelLayout->addWidget(new QLabel("Từ"));
    labelLayout->addWidget(new QLabel("Đến"));
    labelLayout->addWidget(new QLabel("Ngày khởi hành"));
    labelLayout->addWidget(new QLabel("Hãng hàng không"));
    labelLayout->addWidget(new QLabel("Khoảng giá"));
    labelLayout->addStretch();
    topLayout->addLayout(labelLayout);

    // Row 2: Inputs + Search button (all inline, same row)
    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(15);

    // From dropdown
    fromSearchCombo_ = new AirportComboBox(airportManager_, this);
    inputLayout->addWidget(fromSearchCombo_);

    // To dropdown
    toSearchCombo_ = new AirportComboBox(airportManager_, this);
    inputLayout->addWidget(toSearchCombo_);

    // Date picker with placeholder
    dateSearchEdit_ = new QDateEdit(this);
    dateSearchEdit_->setCalendarPopup(true);
    dateSearchEdit_->setDisplayFormat("dd/MM/yyyy");
    dateSearchEdit_->setSpecialValueText("Tùy chọn");
    dateSearchEdit_->clear();  // Clear initial date to show placeholder
    inputLayout->addWidget(dateSearchEdit_);

    // Airline dropdown with placeholder
    airlineFilterCombo_ = new QComboBox(this);
    airlineFilterCombo_->addItem("Tùy chọn", "");  // Default placeholder
    airlineFilterCombo_->addItem("VietJet Air", "VietJet Air");
    airlineFilterCombo_->addItem("Vietnam Airlines", "Vietnam Airlines");
    airlineFilterCombo_->addItem("Bamboo Airways", "Bamboo Airways");
    airlineFilterCombo_->addItem("Vietravel Airlines", "Vietravel Airlines");
    inputLayout->addWidget(airlineFilterCombo_);

    // Price range - QLineEdit (plain text, no spin buttons)
    priceMinEdit_ = new QLineEdit(this);
    priceMinEdit_->setPlaceholderText("Tùy chọn");
    priceMinEdit_->setValidator(new QIntValidator(0, MAX_FLIGHT_PRICE, this));
    priceMinEdit_->setMaximumWidth(120);
    inputLayout->addWidget(priceMinEdit_);

    inputLayout->addWidget(new QLabel("—"));  // Dash separator

    priceMaxEdit_ = new QLineEdit(this);
    priceMaxEdit_->setPlaceholderText("Tùy chọn");
    priceMaxEdit_->setValidator(new QIntValidator(0, MAX_FLIGHT_PRICE, this));
    priceMaxEdit_->setMaximumWidth(120);
    inputLayout->addWidget(priceMaxEdit_);

    // Search button - inline, same row, smaller size
    QPushButton* searchBtn = new QPushButton("Tìm kiếm", this);  // Not uppercase
    searchBtn->setFixedHeight(fromSearchCombo_->sizeHint().height());  // Match input height
    searchBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #4472C4;"
        "  color: white;"
        "  font-size: 11pt;"
        "  padding: 5px 20px;"
        "  border-radius: 3px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #365a9e;"
        "}"
    );
    inputLayout->addWidget(searchBtn);

    inputLayout->addStretch();
    topLayout->addLayout(inputLayout);

    // Connect search button
    connect(searchBtn, &QPushButton::clicked, this, &SearchBookPage::onSearchClicked);

    mainLayout->addWidget(topBar);

    // ================== TIÊU ĐỀ BẢNG ==================
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *thLayout = new QHBoxLayout(tableHeader);
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(0);

    QLabel *tblTitle = new QLabel("Kết quả tìm kiếm chuyến bay", this);
    tblTitle->setProperty("class", "SectionTitle");
    thLayout->addWidget(tblTitle);
    thLayout->addStretch();

    // Add status label
    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 12px;");
    thLayout->addWidget(statusLabel_);

    mainLayout->addWidget(tableHeader);

    // ================== BẢNG ==================
    QWidget *tableBox = new QWidget(this);
    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 6, 24, 0);

    tableView_ = new QTableView(this);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);  // chọn nguyên hàng
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection); // chọn 1 chuyến
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Bật STT giống FlightsPage
    tableView_->verticalHeader()->setVisible(true);
    tableView_->verticalHeader()->setMinimumWidth(32);
    tableView_->verticalHeader()->setDefaultSectionSize(30);
    tableView_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Header ngang giãn đều – tránh lỗi dồn trái
    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);

    // ================== NÚT ĐẶT VÉ DƯỚI ==================
    QWidget *bottom = new QWidget(this);
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottom);
    bottomLayout->setContentsMargins(24, 16, 24, 20);
    bottomLayout->setSpacing(16);

    bookButton_ = new QPushButton("Đặt vé cho chuyến đã chọn", this);
    bookButton_->setStyleSheet(
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:10px; height:40px; padding:0 36px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }"
    );

    bottomLayout->addStretch();
    bottomLayout->addWidget(bookButton_);
    bottomLayout->addStretch();

    mainLayout->addWidget(bottom);
}

void SearchBookPage::setupModel()
{
    // 7 cột giống bản cũ nhưng format kiểu FlightsPage
    model_ = new QStandardItemModel(0, 7, this);
    model_->setHorizontalHeaderLabels({
        "Mã Chuyến",          // instanceId
        "Mã Tuyến (FlightId)",// flightId
        "Ngày khởi hành",
        "Giờ khởi hành",
        "Ngày hạ cánh (dự kiến)",
        "Hãng hàng không",
        "Giá từ"
    });
    tableView_->setModel(model_);
}

void SearchBookPage::setupConnections()
{
    // đặt vé
    connect(bookButton_, &QPushButton::clicked, this, &SearchBookPage::onBookClicked);
}

// ================ CHỖ NẠP DỮ LIỆU VÀO BẢNG ================
void SearchBookPage::fillTable(const std::vector<FlightInstance*>& instances)
{
    model_->removeRows(0, model_->rowCount());

    for (int i = 0; i < instances.size(); ++i) {
        FlightInstance* inst = instances[i];
        if (!inst) continue;

        // Lấy thông tin hãng hàng không từ Flight
        QString airline = "N/A";
        Flight* flight = flightManager_->findFlightById(inst->getFlightId());
        if (flight) {
            airline = QString::fromStdString(flight->getAirline());
        }

        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(inst->getInstanceId()))
            << new QStandardItem(QString::fromStdString(inst->getFlightId()))
            << new QStandardItem(QString::fromStdString(inst->getDepartureDate()))
            << new QStandardItem(QString::fromStdString(inst->getDepartureTime()))
            << new QStandardItem(QString::fromStdString(inst->getArrivalDate()))
            << new QStandardItem(airline)
            << new QStandardItem(QString::number(inst->getFareEconomy()));
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
    
    if (criteria.fromIATA.empty() || criteria.toIATA.empty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn điểm đi và điểm đến");
        return;
    }
    
    // Date (optional)
    if (dateSearchEdit_->date().isValid() && 
        dateSearchEdit_->specialValueText() != dateSearchEdit_->text()) {
        criteria.date = dateSearchEdit_->date().toString("dd/MM/yyyy").toStdString();
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
    
    // Display results
    fillTable(results);
    
    // Update status
    statusLabel_->setText(
        QString("Tìm thấy %1 chuyến bay").arg(results.size())
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

    // Lấy instanceId từ cột 0
    QString instanceId = model_->itemFromIndex(selected.first().siblingAtColumn(0))->text();
    
    // Lấy thông tin chuyến bay
    FlightInstance* instance = flightManager_->findInstanceById(instanceId.toStdString());
    if (!instance) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy chuyến bay.");
        return;
    }
    
    // Hiển thị dialog đặt vé
    BookingDialog dialog(instance, flightManager_, bookingManager_, this);
    if (dialog.exec() != QDialog::Accepted) {
        return; // User cancelled
    }
    
    // Lấy thông tin từ dialog
    QString passengerId = dialog.getPassengerId();
    BookingClass bkClass = dialog.getSelectedClass();
    
    // 3. Lấy ID của agent hiện tại
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        QMessageBox::warning(this, "Lỗi", "Không thể xác định người dùng. Vui lòng đăng nhập lại.");
        return;
    }
    std::string currentAgentId = currentUser->getId();
    
    // 4. Lấy giá vé từ FlightInstance
    int fare = (bkClass == BookingClass::Economy) 
               ? instance->getFareEconomy() 
               : instance->getFareBusiness();

    // 5. Tạo booking
    Booking* newBk = bookingManager_->createNewBooking(
        *flightManager_,
        instanceId.toStdString(),
        currentAgentId,
        passengerId.toStdString(),
        bkClass,
        fare,
        *flightManager_->getSeatManager()
    );

    if (newBk) {
        QMessageBox::information(this, "Thành công",
            QString("Đặt vé thành công!\n\nMã đặt chỗ: %1\nHành khách: %2\nGiá vé: %3 VND")
            .arg(QString::fromStdString(newBk->getBookingId()))
            .arg(passengerId)
            .arg(fare));
        // Sau khi đặt vé thì nên reload để cập nhật số ghế
        fillTable(flightManager_->getAllInstances());
    } else {
        QMessageBox::critical(this, "Thất bại",
            "Không đặt được vé. Có thể do hết ghế hoặc thông tin không hợp lệ.");
    }
}
