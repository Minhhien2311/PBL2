#include "SearchBookPage.h"

// Core & entities
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/AccountManager.h"
#include "entities/FlightInstance.h"
#include "DSA/DynamicArray.h"

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
                               QWidget* parent)
    : QWidget(parent),
      flightManager_(flManager),
      bookingManager_(bkManager),
      accountManager_(accManager)
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

        fromSearchEdit_ = new QLineEdit;
        fromSearchEdit_->setPlaceholderText("Nhập điểm đi");
        toSearchEdit_ = new QLineEdit;
        toSearchEdit_->setPlaceholderText("Nhập điểm đến");

        routeHL->addWidget(fromSearchEdit_);
        routeHL->addWidget(new QLabel("–"));
        routeHL->addWidget(toSearchEdit_);

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

        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(inst->getInstanceId()))
            << new QStandardItem(QString::fromStdString(inst->getFlightId()))
            << new QStandardItem(QString::fromStdString(inst->getDepartureDate()))
            << new QStandardItem(QString::fromStdString(inst->getDepartureTime()))
            << new QStandardItem(QString::fromStdString(inst->getArrivalDate()))
            << new QStandardItem(QString::number(inst->getFareEconomy()));
        model_->appendRow(row);
    }
}

// ================ SLOT TÌM KIẾM ================
// Tìm theo ID
void SearchBookPage::onSearchById()
{
    // --- [CHỖ NỐI API] ---
    // TODO: viết hàm trong FlightManager:
    // DynamicArray<FlightInstance*> list = flightManager_->findInstancesById(id);
    // fillTable(list);
    QString id = idSearchEdit_->text();
    if (id.isEmpty()) {
        QMessageBox::information(this, "Thiếu dữ liệu", "Nhập ID chuyến cần tìm.");
        return;
    }

    QMessageBox::information(this, "WIP",
        "Tìm theo ID chưa được nối với core. Tạm thời hiển thị tất cả để bạn test UI.");
    fillTable(flightManager_->getAllInstances());
    // --- [HẾT CHỖ NỐI API] ---
}

// Tìm theo lộ trình
void SearchBookPage::onSearchByRoute()
{
    // --- [CHỖ NỐI API] ---
    // TODO: cần hàm trong FlightManager:
    // flightManager_->findInstancesByRoute(from, to);
    std::string from = fromSearchEdit_->text().toStdString();
    std::string to   = toSearchEdit_->text().toStdString();

    if (from.empty() || to.empty()) {
        QMessageBox::information(this, "Thiếu dữ liệu", "Nhập đủ điểm đi và điểm đến.");
        return;
    }

    QMessageBox::information(this, "WIP",
        "Tìm theo lộ trình chưa được nối core. Tạm thời hiển thị tất cả.");
    fillTable(flightManager_->getAllInstances());
    // --- [HẾT CHỖ NỐI API] ---
}

// Tìm theo ngày
void SearchBookPage::onSearchByDate()
{
    // --- [CHỖ NỐI API] ---
    // TODO: cần hàm:
    // flightManager_->findInstancesByDate(dateString);
    QString date = dateSearchEdit_->date().toString("dd/MM/yyyy");

    QMessageBox::information(this, "WIP",
        "Tìm theo ngày chưa được nối core. Tạm thời hiển thị tất cả.");
    Q_UNUSED(date);
    fillTable(flightManager_->getAllInstances());
    // --- [HẾT CHỖ NỐI API] ---
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

    // --- [CHỖ NỐI API] ---
    // 1. Hỏi Passenger ID
    bool okId;
    QString passengerId = QInputDialog::getText(this, "Thông tin hành khách",
        "Nhập CMND/CCCD (Passenger ID):", QLineEdit::Normal, "", &okId);
    if (!okId || passengerId.isEmpty()) return;

    // 2. Hỏi hạng vé
    bool okClass;
    QStringList classes = { "Phổ thông (Economy)", "Thương gia (Business)" };
    QString classChoice = QInputDialog::getItem(this, "Chọn hạng vé",
        "Hạng vé:", classes, 0, false, &okClass);
    if (!okClass) return;

    BookingClass bkClass = (classChoice == classes[0])
                            ? BookingClass::Economy
                            : BookingClass::Business;

    // 3. *** CHỖ NỐI API THẬT ***
    // Bạn nên viết 1 hàm trong BookingManager kiểu:
    // Booking* createBookingFromAgent(FlightManager& fm,
    //                                 const std::string& instanceId,
    //                                 const std::string& passengerId,
    //                                 BookingClass cls,
    //                                 int baseFare,
    //                                 Account* currentAgent);
    // Rồi tại đây gọi nó.
    int fakeFare = 1500000; // tạm

    Booking* newBk = bookingManager_->createNewBooking(
        *flightManager_,
        instanceId.toStdString(),
        accountManager_->getCurrentUser()->getId(),
        passengerId.toStdString(),
        bkClass,
        fakeFare
    );

    if (newBk) {
        QMessageBox::information(this, "Thành công",
            QString("Đặt vé thành công. Mã đặt chỗ: %1")
            .arg(QString::fromStdString(newBk->getBookingId())));
        // Sau khi đặt vé thì nên reload để cập nhật số ghế
        fillTable(flightManager_->getAllInstances());
    } else {
        QMessageBox::critical(this, "Thất bại",
            "Không đặt được vé. Bạn tự nối thêm điều kiện (hết ghế, sai ID, ...).");
    }
    // --- [HẾT CHỖ NỐI API] ---
}
