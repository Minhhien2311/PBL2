#include "SearchBookPage.h"

// Core & entities
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/AccountManager.h"
#include "core/AirportManager.h"
#include "entities/FlightInstance.h"
#include "entities/Flight.h"
#include "entities/Account.h"
#include "DSA/DynamicArray.h"
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
        "QLineEdit, QDateEdit { background:white; border:1px solid #608bc1; "
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

    // Hàng tìm kiếm giống FlightsPage: 3 box
    QHBoxLayout *searchRow = new QHBoxLayout;
    searchRow->setSpacing(18);

    // Box 1: Tìm theo ID chuyến bay
    {
        QWidget *box = new QWidget;
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(5);

        idSearchEdit_ = new QLineEdit;
        idSearchEdit_->setPlaceholderText("Nhập mã chuyến bay (VN123)");
        searchByIdBtn_ = new QPushButton("Tìm theo mã chuyến bay");
        searchByIdBtn_->setProperty("class", "SearchBtn");

        v->addWidget(idSearchEdit_);
        v->addWidget(searchByIdBtn_);

        box->setMinimumWidth(200);
        searchRow->addWidget(box);
    }

    // Box 2: Lộ trình (từ – đến)
    {
        QWidget *box = new QWidget;
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(5);

        QWidget *routeRow = new QWidget;
        QHBoxLayout *routeHL = new QHBoxLayout(routeRow);
        routeHL->setContentsMargins(0,0,0,0);
        routeHL->setSpacing(6);

        fromSearchCombo_ = new AirportComboBox(airportManager_);
        toSearchCombo_ = new AirportComboBox(airportManager_);

        routeHL->addWidget(new QLabel("Từ:"));
        routeHL->addWidget(fromSearchCombo_, 1);
        routeHL->addWidget(new QLabel("→"));
        routeHL->addWidget(new QLabel("Đến:"));
        routeHL->addWidget(toSearchCombo_, 1);

        searchByRouteBtn_ = new QPushButton("Tìm theo lộ trình bay");
        searchByRouteBtn_->setProperty("class", "SearchBtn");

        v->addWidget(routeRow);
        v->addWidget(searchByRouteBtn_);

        searchRow->addWidget(box, 1);
    }

    // Box 3: Ngày khởi hành
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

    // ================== TIÊU ĐỀ BẢNG ==================
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *thLayout = new QHBoxLayout(tableHeader);
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(0);

    QLabel *tblTitle = new QLabel("Kết quả tìm kiếm chuyến bay", this);
    tblTitle->setProperty("class", "SectionTitle");
    thLayout->addWidget(tblTitle);
    thLayout->addStretch();

    // nút tải lại để agent test nhanh
    refreshBtn_ = new QPushButton("Tải lại tất cả");
    refreshBtn_->setProperty("class", "SearchBtn");
    thLayout->addWidget(refreshBtn_);

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
    // 3 nút tìm kiếm
    connect(searchByIdBtn_,   &QPushButton::clicked, this, &SearchBookPage::onSearchById);
    connect(searchByRouteBtn_,&QPushButton::clicked, this, &SearchBookPage::onSearchByRoute);
    connect(searchByDateBtn_, &QPushButton::clicked, this, &SearchBookPage::onSearchByDate);

    // tải lại
    connect(refreshBtn_, &QPushButton::clicked, this, &SearchBookPage::onRefreshAll);

    // đặt vé
    connect(bookButton_, &QPushButton::clicked, this, &SearchBookPage::onBookClicked);
}

// ================ CHỖ NẠP DỮ LIỆU VÀO BẢNG ================
void SearchBookPage::fillTable(const DynamicArray<FlightInstance*>& instances)
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
// Tìm theo ID
void SearchBookPage::onSearchById()
{
    QString id = idSearchEdit_->text();
    if (id.isEmpty()) {
        QMessageBox::information(this, "Thiếu dữ liệu", "Nhập mã chuyến bay cần tìm.");
        return;
    }

    // Tìm instance theo ID (flight number)
    FlightInstance* instance = flightManager_->findInstanceById(id.toStdString());
    
    if (instance) {
        DynamicArray<FlightInstance*> result;
        result.push_back(instance);
        fillTable(result);
    } else {
        model_->removeRows(0, model_->rowCount());
        QMessageBox::information(this, "Không tìm thấy", 
            QString("Không tìm thấy chuyến bay với mã: %1").arg(id));
    }
}

// Tìm theo lộ trình
void SearchBookPage::onSearchByRoute()
{
    std::string fromIATA = fromSearchCombo_->getSelectedIATA();
    std::string toIATA = toSearchCombo_->getSelectedIATA();

    if (fromIATA.empty() || toIATA.empty()) {
        QMessageBox::information(this, "Thiếu dữ liệu", "Vui lòng chọn điểm đi và điểm đến.");
        return;
    }

    // Tìm Flight theo route
    DynamicArray<Flight*> flights = flightManager_->findFlightByRoute(fromIATA, toIATA);

    if (flights.size() == 0) {
        model_->removeRows(0, model_->rowCount());
        QMessageBox::information(this, "Không tìm thấy", 
            QString("Không tìm thấy tuyến bay từ %1 đến %2").arg(QString::fromStdString(fromIATA), QString::fromStdString(toIATA)));
        return;
    }

    // Collect all instances for the found flights
    DynamicArray<FlightInstance*> allInstancesForRoute;
    for (int i = 0; i < flights.size(); ++i) {
        Flight* flight = flights[i];
        DynamicArray<FlightInstance*> instancesForFlight = flightManager_->findInstancesByFlightId(flight->getFlightId());
        for (int j = 0; j < instancesForFlight.size(); ++j) {
            allInstancesForRoute.push_back(instancesForFlight[j]);
        }
    }
    if (allInstancesForRoute.size() == 0) {
        model_->removeRows(0, model_->rowCount());
        QMessageBox::information(this, "Không có chuyến bay", 
            "Không có chuyến bay nào cho tuyến này.");
    } else {
        fillTable(allInstancesForRoute);
    }
}

// Tìm theo ngày
void SearchBookPage::onSearchByDate()
{
    QString date = dateSearchEdit_->date().toString("dd/MM/yyyy");
    std::string dateStr = date.toStdString();

    // Lọc tất cả instances theo ngày
    const DynamicArray<FlightInstance*>& allInstances = flightManager_->getAllInstances();
    DynamicArray<FlightInstance*> filtered;
    
    for (int i = 0; i < allInstances.size(); ++i) {
        FlightInstance* inst = allInstances[i];
        if (inst && inst->getDepartureDate() == dateStr) {
            filtered.push_back(inst);
        }
    }
    
    if (filtered.size() == 0) {
        model_->removeRows(0, model_->rowCount());
        QMessageBox::information(this, "Không có chuyến bay", 
            QString("Không có chuyến bay nào vào ngày %1").arg(date));
    } else {
        fillTable(filtered);
    }
}

// Tải lại tất cả
void SearchBookPage::onRefreshAll()
{
    fillTable(flightManager_->getAllInstances());
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
    BookingDialog dialog(instance, flightManager_, this);
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
