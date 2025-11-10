#include "AgentBookingsPage.h"

// Includes
#include "core/BookingManager.h"
#include "core/FlightManager.h"
#include "core/AccountManager.h"
#include "core/AirportManager.h"
#include "core/SeatManager.h"
#include "entities/Booking.h"
#include "entities/Account.h"
#include "entities/FlightInstance.h"
#include "entities/Seat.h"
#include "BookingDetailsDialog.h" // Dialog xem chi tiết
#include "AirportComboBox.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QCalendarWidget> // Để set lịch popup
#include <QString>
#include <QDialog>
#include <QGroupBox>
#include <QTextEdit>

// <--- CẬP NHẬT CONSTRUCTOR: Nhận cả 3 manager
AgentBookingsPage::AgentBookingsPage(BookingManager* bkManager,
                                     FlightManager* flManager,
                                     AccountManager* accManager,
                                     AirportManager* airportManager,
                                     QWidget *parent)
    : QWidget(parent),
      bookingManager_(bkManager),
      flightManager_(flManager),
      accountManager_(accManager),
      airportManager_(airportManager)
{
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(accountManager_ != nullptr);

    setupUi();
    setupModel();
    setupConnections();
    
    refreshTable(); // Tải dữ liệu lần đầu
}

void AgentBookingsPage::setupUi()
{
    // style
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
        "tableTitle { font-size: 18px; font-weight: 600; }"
    );

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ================== TOP ==================
    auto *top = new QWidget(this);
    auto *topLayout = new QVBoxLayout(top);
    topLayout->setContentsMargins(24, 20, 24, 10);
    topLayout->setSpacing(10);

    // tiêu đề
    auto *title = new QLabel("Tìm thông tin đặt chỗ", this);
    title->setProperty("class", "PageTitle");
    topLayout->addWidget(title);

    // ====== KHỐI TÌM KIẾM DÙNG GRID ======
    QGridLayout *searchGrid = new QGridLayout;
    searchGrid->setHorizontalSpacing(18);
    searchGrid->setVerticalSpacing(10);

    // để căn giữa toàn bộ khối
    searchGrid->setColumnStretch(0, 0);
    searchGrid->setColumnStretch(1, 0);
    searchGrid->setColumnStretch(2, 0);
    searchGrid->setColumnStretch(3, 1);   // đẩy nhẹ về trái-phải

    // độ rộng các field
    int fieldWidth = 230;

    // ----- HÀNG 0 -----
    // (0,0) Mã đặt chỗ
    {
        QWidget *box = new QWidget;
        box->setFixedWidth(fieldWidth);
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(4);

        bookingIdSearchEdit_ = new QLineEdit;
        bookingIdSearchEdit_->setPlaceholderText("Nhập mã đặt chỗ");

        v->addWidget(bookingIdSearchEdit_);

        searchGrid->addWidget(box, 0, 0);
    }

    // (0,1) Điểm xuất phát
    {
        QWidget *box = new QWidget;
        box->setFixedWidth(fieldWidth);
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(4);

        fromSearchCombo_ = new AirportComboBox(airportManager_);

        v->addWidget(fromSearchCombo_);

        searchGrid->addWidget(box, 0, 1);
    }

    // (0,2) Điểm đến
    {
        QWidget *box = new QWidget;
        box->setFixedWidth(fieldWidth);
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(4);

        toSearchCombo_ = new AirportComboBox(airportManager_);

        v->addWidget(toSearchCombo_);

        searchGrid->addWidget(box, 0, 2);
    }

    // (0,3) Ngày đặt / khởi hành
    {
        QWidget *box = new QWidget;
        box->setFixedWidth(fieldWidth);
        QVBoxLayout *v = new QVBoxLayout(box);
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(4);

        dateSearchEdit_ = new QDateEdit(QDate::currentDate(), this);
        dateSearchEdit_->setCalendarPopup(true);
        dateSearchEdit_->setDisplayFormat("dd/MM/yyyy");

        v->addWidget(dateSearchEdit_);

        searchGrid->addWidget(box, 0, 3);
    }

    // ----- HÀNG 1 -----
    // (1,0) nút tìm theo ID  (thẳng với ô 1)
    searchButton_ = new QPushButton("Tìm theo mã đặt chỗ");
    searchButton_->setProperty("class", "SearchBtn");
    searchButton_->setMinimumHeight(40);
    searchGrid->addWidget(searchButton_, 1, 0);

    // (1,1)-(1,2) nút tìm theo lộ trình bay  (ghép 2 cột)
    QPushButton *routeBtn = new QPushButton("Tìm theo lộ trình bay");
    routeBtn->setProperty("class", "SearchBtn");
    routeBtn->setMinimumHeight(40);
    searchGrid->addWidget(routeBtn, 1, 1, 1, 2);  // row=1, col=1, rowspan=1, colspan=2

    // (1,3) nút tìm theo ngày khởi hành (thẳng với ô 4)
    QPushButton *dateBtn = new QPushButton("Tìm theo ngày khởi hành");
    dateBtn->setProperty("class", "SearchBtn");
    dateBtn->setMinimumHeight(40);
    searchGrid->addWidget(dateBtn, 1, 3);

    // đưa cả grid vào topLayout
    topLayout->addLayout(searchGrid);
    mainLayout->addWidget(top);


    // ================== TIÊU ĐỀ BẢNG ==================
    auto *tableHeader = new QWidget(this);
    auto *thLayout = new QHBoxLayout(tableHeader);
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(0);

    auto *tblTitle = new QLabel("Tất cả thông tin đặt chỗ", this);
    tblTitle->setObjectName("tableTitle");
    tblTitle->setProperty("class", "SectionTitle");
    thLayout->addWidget(tblTitle);
    thLayout->addStretch();

    refreshButton_ = new QPushButton("Tải lại tất cả");
    refreshButton_->setProperty("class", "SearchBtn");
    refreshButton_->setMinimumWidth(120);
    thLayout->addWidget(refreshButton_);

    mainLayout->addWidget(tableHeader);

    // ================== BẢNG ==================
    auto *tableBox = new QWidget(this);
    auto *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 6, 24, 0);

    tableView_ = new QTableView(this);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    // bật STT giống dashboard/routes
    tableView_->verticalHeader()->setVisible(true);
    tableView_->verticalHeader()->setMinimumWidth(32);
    tableView_->verticalHeader()->setDefaultSectionSize(30);

    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);

    // ================== NÚT DƯỚI ==================
    auto *bottom = new QWidget(this);
    auto *bottomLayout = new QHBoxLayout(bottom);
    bottomLayout->setContentsMargins(24, 16, 24, 20);
    bottomLayout->setSpacing(16);

    QString blueBtn =
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:10px; height:40px; padding:0 36px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }";

    viewDetailsBtn_ = new QPushButton("Xem chi tiết vé");
    viewDetailsBtn_->setStyleSheet(blueBtn);

    cancelBookingBtn_ = new QPushButton("Hủy vé");
    cancelBookingBtn_->setStyleSheet(blueBtn);

    changeBookingBtn_ = new QPushButton("Đổi vé");
    changeBookingBtn_->setStyleSheet(blueBtn);

    bottomLayout->addStretch();
    bottomLayout->addWidget(viewDetailsBtn_);
    bottomLayout->addWidget(cancelBookingBtn_);
    bottomLayout->addWidget(changeBookingBtn_);
    bottomLayout->addStretch();

    mainLayout->addWidget(bottom);
}


void AgentBookingsPage::setupModel()
{
    model_ = new QStandardItemModel(0, 7, this);
    model_->setHorizontalHeaderLabels({
        "Mã Đặt chỗ", "Mã Chuyến", "Mã hành khách", 
        "Ngày đặt", "Hạng vé", "Giá vé", "Trạng thái"
    });
    tableView_->setModel(model_);
}

void AgentBookingsPage::setupConnections()
{
    connect(searchButton_, &QPushButton::clicked, this, &AgentBookingsPage::onSearchClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &AgentBookingsPage::refreshTable);
    connect(cancelBookingBtn_, &QPushButton::clicked, this, &AgentBookingsPage::onCancelBookingClicked);
    connect(viewDetailsBtn_, &QPushButton::clicked, this, &AgentBookingsPage::onViewDetailsClicked);
    connect(changeBookingBtn_, &QPushButton::clicked, this, &AgentBookingsPage::onChangeBookingClicked);
}

// Hàm này tải (hoặc làm mới) TOÀN BỘ vé của Agent
void AgentBookingsPage::refreshTable()
{
    model_->removeRows(0, model_->rowCount());

    // 1. Lấy ID của Agent đang đăng nhập
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        QMessageBox::warning(this, "Lỗi", "Không thể xác định người dùng. Vui lòng đăng nhập lại.");
        return;
    }
    std::string currentAgentId = currentUser->getId();

    // 2. Lấy danh sách booking của Agent này
    std::vector<Booking*> agentBookings = bookingManager_->getBookingsByAgentId(currentAgentId);

    // 3. Hiển thị các booking
    for (int i = 0; i < agentBookings.size(); ++i) {
        Booking* booking = agentBookings[i];
        
        if (booking) {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(QString::fromStdString(booking->getBookingId()));
            rowItems << new QStandardItem(QString::fromStdString(booking->getFlightInstanceId()));
            rowItems << new QStandardItem(QString::fromStdString(booking->getPassengerId()));
            rowItems << new QStandardItem(QString::fromStdString(booking->getBookingDate()));

            // Hạng vé
            QString classStr = (booking->getClass() == BookingClass::Economy) 
                              ? "Hạng phổ thông" : "Thương gia";
            rowItems << new QStandardItem(classStr);
            rowItems << new QStandardItem(QString::number(booking->getBaseFare()));

            // Trạng thái
            QString statusStr;
            if (booking->getStatus() == BookingStatus::Issued) {
                statusStr = "Đang giữ chỗ";
            } else if (booking->getStatus() == BookingStatus::Cancelled) {
                statusStr = "Đã hủy";
            } else {
                statusStr = "Đã đổi";
            }
            rowItems << new QStandardItem(statusStr);

            model_->appendRow(rowItems);
        }
    }
}

void AgentBookingsPage::onSearchClicked()
{
    // Lấy thông tin tìm kiếm
    std::string bookingId = bookingIdSearchEdit_->text().toStdString();
    std::string date = dateSearchEdit_->date().toString("dd/MM/yyyy").toStdString();
    
    // Nếu không có gì để tìm, hiển thị tất cả
    if (bookingId.empty() && dateSearchEdit_->date() == QDate::currentDate()) {
        refreshTable();
        return;
    }
    
    // 1. Lấy ID của Agent đang đăng nhập
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        QMessageBox::warning(this, "Lỗi", "Không thể xác định người dùng. Vui lòng đăng nhập lại.");
        return;
    }
    std::string currentAgentId = currentUser->getId();
    
    // 2. Lấy toàn bộ booking của Agent
    std::vector<Booking*> agentBookings = bookingManager_->getBookingsByAgentId(currentAgentId);
    
    // 3. Lọc theo điều kiện tìm kiếm
    model_->removeRows(0, model_->rowCount());
    
    for (int i = 0; i < agentBookings.size(); ++i) {
        Booking* booking = agentBookings[i];
        if (!booking) continue;
        
        // Lọc theo Booking ID nếu có nhập
        if (!bookingId.empty()) {
            if (booking->getBookingId().find(bookingId) == std::string::npos) {
                continue; // Không khớp, bỏ qua
            }
        }
        
        // Lọc theo ngày nếu không phải ngày hiện tại (mặc định)
        if (dateSearchEdit_->date() != QDate::currentDate()) {
            std::string bookingDate = booking->getBookingDate();
            // bookingDate format: "DD/MM/YYYY HH:MM:SS" hoặc "YYYY-MM-DD HH:MM:SS"
            if (bookingDate.find(date) == std::string::npos) {
                continue; // Không khớp, bỏ qua
            }
        }
        
        // Nếu qua được tất cả bộ lọc, thêm vào bảng
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(QString::fromStdString(booking->getBookingId()));
        rowItems << new QStandardItem(QString::fromStdString(booking->getFlightInstanceId()));
        rowItems << new QStandardItem(QString::fromStdString(booking->getPassengerId()));
        rowItems << new QStandardItem(QString::fromStdString(booking->getBookingDate()));

        // Hạng vé
        QString classStr = (booking->getClass() == BookingClass::Economy) 
                          ? "Hạng phổ thông" : "Thương gia";
        rowItems << new QStandardItem(classStr);
        rowItems << new QStandardItem(QString::number(booking->getBaseFare()));

        // Trạng thái
        QString statusStr;
        if (booking->getStatus() == BookingStatus::Issued) {
            statusStr = "Đang giữ chỗ";
        } else if (booking->getStatus() == BookingStatus::Cancelled) {
            statusStr = "Đã hủy";
        } else {
            statusStr = "Đã đổi";
        }
        rowItems << new QStandardItem(statusStr);

        model_->appendRow(rowItems);
    }
}

void AgentBookingsPage::onCancelBookingClicked()
{
    // 1. Lấy hàng đang chọn
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một vé để hủy.");
        return;
    }
    
    // Lấy Booking ID (cột 0) và Trạng thái (cột 6)
    // Note: Column indices match setupModel() header order:
    // 0=Mã Đặt chỗ, 1=Mã Chuyến, 2=Mã hành khách, 3=Ngày đặt, 4=Hạng vé, 5=Giá vé, 6=Trạng thái
    QModelIndex idIndex = selected.first().siblingAtColumn(0);
    QModelIndex statusIndex = selected.first().siblingAtColumn(6);  // Status column
    
    QString bookingId = model_->data(idIndex).toString();
    QString status = model_->data(statusIndex).toString();

    // 2. Kiểm tra xem đã hủy chưa
    if (status == "Đã hủy") {
        QMessageBox::warning(this, "Lỗi", "Vé này đã được hủy từ trước.");
        return;
    }

    // 3. Check if can cancel (time constraint)
    if (!bookingManager_->canCancelBooking(bookingId.toStdString(), 
                                          *flightManager_)) {
        QString deadline = QString::fromStdString(
            bookingManager_->getCancellationDeadline(bookingId.toStdString(), 
                                                    *flightManager_)
        );
        
        QMessageBox::warning(this, "Không thể hủy", 
            QString("Không thể hủy vé này.\n\n"
                   "Lý do: Đã quá hạn hủy vé.\n"
                   "Hạn hủy: %1\n\n"
                   "Vui lòng liên hệ bộ phận hỗ trợ.").arg(deadline));
        return;
    }

    // 4. Xác nhận
    auto reply = QMessageBox::question(this, "Xác nhận hủy vé", 
        QString("Bạn có chắc chắn muốn hủy vé %1?\n\n"
               "⚠️ Lưu ý:\n"
               "- Ghế sẽ được trả lại hệ thống\n"
               "- Trạng thái đặt chỗ sẽ chuyển sang 'Đã hủy'\n"
               "- Hành động này KHÔNG THỂ hoàn tác").arg(bookingId), 
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }
    
    // 5. Gọi API hủy vé
    bool success = bookingManager_->cancelBooking(*flightManager_, 
                                                  *flightManager_->getSeatManager(), 
                                                  bookingId.toStdString());

    // 6. Xử lý kết quả
    if (success) {
        QMessageBox::information(this, "Thành công", 
            QString("Đã hủy vé thành công!\n\n"
                   "Mã đặt chỗ: %1\n"
                   "Ghế đã được trả lại hệ thống.\n"
                   "Trạng thái đã được cập nhật.").arg(bookingId));
        refreshTable(); // Tải lại bảng
    } else {
        QMessageBox::critical(this, "Thất bại", 
            "Hủy vé thất bại.\n\n"
            "Có thể do:\n"
            "- Quá sát giờ bay\n"
            "- Lỗi lưu dữ liệu\n"
            "- Vé không tồn tại\n\n"
            "Vui lòng thử lại hoặc liên hệ hỗ trợ.");
    }
}

void AgentBookingsPage::onViewDetailsClicked()
{
    // 1. Lấy hàng đang chọn
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một vé để xem chi tiết.");
        return;
    }
    
    // 2. Lấy Booking ID từ cột đầu tiên
    QModelIndex idIndex = selected.first().siblingAtColumn(0);
    QString bookingId = model_->data(idIndex).toString();
    
    // 3. Tìm booking từ BookingManager
    Booking* booking = bookingManager_->findBookingById(bookingId.toStdString());
    
    if (!booking) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy thông tin đặt chỗ.");
        return;
    }
    
    // 4. Hiển thị dialog chi tiết
    BookingDetailsDialog dialog(booking, flightManager_, accountManager_, this);
    dialog.exec();
}

void AgentBookingsPage::onChangeBookingClicked()
{
    // 1. Get selected row
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một vé để đổi.");
        return;
    }
    
    // Get Booking ID and Status
    QModelIndex idIndex = selected.first().siblingAtColumn(0);
    QModelIndex statusIndex = selected.first().siblingAtColumn(6);
    
    QString bookingId = model_->data(idIndex).toString();
    QString status = model_->data(statusIndex).toString();
    
    // 2. Validate status is Issued
    if (status != "Đang giữ chỗ") {
        QMessageBox::warning(this, "Lỗi", "Chỉ có thể đổi vé có trạng thái 'Đang giữ chỗ'.");
        return;
    }
    
    // 3. Find booking
    Booking* booking = bookingManager_->findBookingById(bookingId.toStdString());
    if (!booking) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy thông tin đặt chỗ.");
        return;
    }
    
    // 4. Create change booking dialog
    QDialog* changeDialog = new QDialog(this);
    changeDialog->setWindowTitle("Đổi vé máy bay");
    changeDialog->setMinimumWidth(600);
    changeDialog->setMinimumHeight(500);
    
    auto* mainLayout = new QVBoxLayout(changeDialog);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    auto* titleLabel = new QLabel("THÔNG TIN ĐỔI VÉ", changeDialog);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #123B7A;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Current booking info
    auto* currentGroup = new QGroupBox("Thông tin vé hiện tại", changeDialog);
    auto* currentLayout = new QVBoxLayout(currentGroup);
    
    FlightInstance* currentInstance = flightManager_->findInstanceById(booking->getFlightInstanceId());
    QString currentInfo = QString("Mã đặt chỗ: %1\nChuyến bay: %2\nGhế: %3\nGiá vé: %4 VND")
        .arg(QString::fromStdString(booking->getBookingId()))
        .arg(currentInstance ? QString::fromStdString(currentInstance->getFlightNumber()) : "N/A")
        .arg(QString::fromStdString(booking->getSeatID()))
        .arg(booking->getBaseFare());
    
    auto* currentInfoLabel = new QLabel(currentInfo);
    currentLayout->addWidget(currentInfoLabel);
    mainLayout->addWidget(currentGroup);
    
    // New flight input
    auto* newFlightGroup = new QGroupBox("Chuyến bay mới", changeDialog);
    auto* newFlightLayout = new QVBoxLayout(newFlightGroup);
    
    auto* flightIdLayout = new QHBoxLayout();
    flightIdLayout->addWidget(new QLabel("Mã chuyến bay mới:"));
    auto* flightIdEdit = new QLineEdit(changeDialog);
    flightIdEdit->setPlaceholderText("Nhập mã chuyến bay (VD: FI-110)");
    flightIdLayout->addWidget(flightIdEdit);
    newFlightLayout->addLayout(flightIdLayout);
    
    // Flight info display
    auto* flightInfoText = new QTextEdit(changeDialog);
    flightInfoText->setReadOnly(true);
    flightInfoText->setMaximumHeight(100);
    flightInfoText->setPlaceholderText("Thông tin chuyến bay sẽ hiển thị ở đây...");
    newFlightLayout->addWidget(flightInfoText);
    
    auto* viewFlightBtn = new QPushButton("Xem thông tin chuyến bay", changeDialog);
    viewFlightBtn->setStyleSheet("background:#4478BD; color:white; padding:8px; border-radius:4px;");
    newFlightLayout->addWidget(viewFlightBtn);
    
    mainLayout->addWidget(newFlightGroup);
    
    // Seat selection
    auto* seatGroup = new QGroupBox("Chọn ghế mới", changeDialog);
    auto* seatLayout = new QVBoxLayout(seatGroup);
    
    auto* selectedSeatEdit = new QLineEdit(changeDialog);
    selectedSeatEdit->setReadOnly(true);
    selectedSeatEdit->setPlaceholderText("Chưa chọn ghế");
    seatLayout->addWidget(selectedSeatEdit);
    
    auto* selectSeatBtn = new QPushButton("Chọn ghế", changeDialog);
    selectSeatBtn->setStyleSheet("background:#4478BD; color:white; padding:8px; border-radius:4px;");
    selectSeatBtn->setEnabled(false); // Disable until flight is selected
    seatLayout->addWidget(selectSeatBtn);
    
    mainLayout->addWidget(seatGroup);
    
    // Price difference
    auto* priceDiffLabel = new QLabel("", changeDialog);
    priceDiffLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    priceDiffLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(priceDiffLabel);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    auto* confirmBtn = new QPushButton("Xác nhận đổi vé", changeDialog);
    confirmBtn->setStyleSheet("background:#5886C0; color:white; padding:10px 30px; border-radius:8px; font-weight:bold;");
    confirmBtn->setEnabled(false); // Disable until seat is selected
    
    auto* cancelBtn = new QPushButton("Hủy", changeDialog);
    cancelBtn->setStyleSheet("background:#999; color:white; padding:10px 30px; border-radius:8px;");
    
    buttonLayout->addWidget(confirmBtn);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    // Store selected flight instance and seat for validation
    FlightInstance* selectedInstance = nullptr;
    QString selectedSeatId = "";
    
    // Connect view flight button
    connect(viewFlightBtn, &QPushButton::clicked, [=, &selectedInstance]() mutable {
        QString flightId = flightIdEdit->text().trimmed();
        if (flightId.isEmpty()) {
            QMessageBox::warning(changeDialog, "Lỗi", "Vui lòng nhập mã chuyến bay.");
            return;
        }
        
        selectedInstance = flightManager_->findInstanceById(flightId.toStdString());
        if (!selectedInstance) {
            QMessageBox::warning(changeDialog, "Lỗi", "Không tìm thấy chuyến bay.");
            flightInfoText->clear();
            selectSeatBtn->setEnabled(false);
            return;
        }
        
        // Display flight info
        QString info = QString("Số hiệu: %1\nNgày khởi hành: %2 %3\nNgày hạ cánh: %4 %5\nGiá Economy: %6 VND\nGiá Business: %7 VND")
            .arg(QString::fromStdString(selectedInstance->getFlightNumber()))
            .arg(QString::fromStdString(selectedInstance->getDepartureDate()))
            .arg(QString::fromStdString(selectedInstance->getDepartureTime()))
            .arg(QString::fromStdString(selectedInstance->getArrivalDate()))
            .arg(QString::fromStdString(selectedInstance->getArrivalTime()))
            .arg(selectedInstance->getFareEconomy())
            .arg(selectedInstance->getFareBusiness());
        
        flightInfoText->setText(info);
        selectSeatBtn->setEnabled(true);
        
        // Calculate price difference
        int newFare = (booking->getClass() == BookingClass::Economy) 
                     ? selectedInstance->getFareEconomy() 
                     : selectedInstance->getFareBusiness();
        int oldFare = booking->getBaseFare();
        int priceDiff = newFare - oldFare;
        
        if (priceDiff > 0) {
            priceDiffLabel->setText(QString("⚠️ Phí phát sinh thêm: %1 VND").arg(priceDiff));
            priceDiffLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px; color: blue;");
        } else if (priceDiff < 0) {
            priceDiffLabel->setText(QString("✅ Hoàn trả: %1 VND").arg(-priceDiff));
            priceDiffLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px; color: red;");
        } else {
            priceDiffLabel->setText("Không có chênh lệch giá");
            priceDiffLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px; color: green;");
        }
    });
    
    // Connect select seat button
    connect(selectSeatBtn, &QPushButton::clicked, [=, &selectedInstance, &selectedSeatId]() mutable {
        if (!selectedInstance) {
            QMessageBox::warning(changeDialog, "Lỗi", "Vui lòng chọn chuyến bay trước.");
            return;
        }
        
        // Create seat selection dialog (reusing logic from BookingDialog)
        QDialog* seatDialog = new QDialog(changeDialog);
        seatDialog->setWindowTitle("Chọn ghế");
        seatDialog->setMinimumWidth(500);
        seatDialog->setMinimumHeight(400);
        
        auto* seatMainLayout = new QVBoxLayout(seatDialog);
        
        // Load seat map
        SeatManager* seatManager = flightManager_->getSeatManager();
        if (!seatManager || !seatManager->loadSeatMapFor(selectedInstance)) {
            QMessageBox::warning(changeDialog, "Lỗi", "Không thể tải sơ đồ ghế.");
            delete seatDialog;
            return;
        }
        
        std::vector<Seat*>* seatMap = seatManager->getActiveSeatMap();
        if (!seatMap) {
            delete seatDialog;
            return;
        }
        
        int cols = seatManager->getSeatColumns();
        
        // Create seat grid
        auto* seatGridLayout = new QGridLayout();
        seatGridLayout->setSpacing(5);
        
        // Add column headers
        for (int col = 0; col < cols; ++col) {
            QLabel* header = new QLabel(QString(QChar('A' + col)));
            header->setAlignment(Qt::AlignCenter);
            header->setStyleSheet("font-weight: bold; padding: 5px;");
            seatGridLayout->addWidget(header, 0, col + 1);
        }
        
        // Render seats
        int currentRow = -1;
        int layoutRow = 1;
        QString tempSelectedSeat = "";
        
        for (int i = 0; i < seatMap->size(); ++i) {
            Seat* seat = (*seatMap)[i];
            if (!seat) continue;
            
            int row, col;
            std::tie(row, col) = seat->getCoordinates();
            
            if (row != currentRow) {
                currentRow = row;
                QLabel* rowLabel = new QLabel(QString::number(row + 1));
                rowLabel->setFixedWidth(25);
                rowLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
                rowLabel->setStyleSheet("font-weight: bold;");
                seatGridLayout->addWidget(rowLabel, layoutRow, 0);
            }
            
            QPushButton* seatBtn = new QPushButton(QString::fromStdString(seat->getId()));
            seatBtn->setFixedSize(50, 40);
            
            QString btnStyle;
            bool isClickable = false;
            
            if (seat->getStatus() == SeatStatus::Available) {
                isClickable = true;
                if (seat->getType() == SeatType::Business) {
                    btnStyle = "QPushButton { background: #FFD700; color: #000; border: 2px solid #DAA520; border-radius: 5px; font-weight: bold; }"
                              "QPushButton:hover { background: #FFC700; }";
                } else {
                    btnStyle = "QPushButton { background: #90EE90; color: #000; border: 2px solid #32CD32; border-radius: 5px; font-weight: bold; }"
                              "QPushButton:hover { background: #7FD87F; }";
                }
            } else if (seat->getStatus() == SeatStatus::Booked) {
                btnStyle = "QPushButton { background: #D3D3D3; color: #666; border: 2px solid #A9A9A9; border-radius: 5px; }";
            } else {
                btnStyle = "QPushButton { background: #FF6B6B; color: #FFF; border: 2px solid #C92A2A; border-radius: 5px; }";
            }
            
            seatBtn->setStyleSheet(btnStyle);
            seatBtn->setEnabled(isClickable);
            seatBtn->setProperty("originalStyle", btnStyle);
            seatBtn->setProperty("seatId", QString::fromStdString(seat->getId()));
            
            if (isClickable) {
                connect(seatBtn, &QPushButton::clicked, [seatGridLayout, seatBtn, &tempSelectedSeat]() {
                    QString seatId = seatBtn->property("seatId").toString();
                    
                    // Deselect previous
                    if (!tempSelectedSeat.isEmpty()) {
                        for (int i = 0; i < seatGridLayout->count(); ++i) {
                            QLayoutItem* layoutItem = seatGridLayout->itemAt(i);
                            if (!layoutItem) continue;
                            
                            QPushButton* btn = qobject_cast<QPushButton*>(layoutItem->widget());
                            if (btn && btn->property("seatId").toString() == tempSelectedSeat) {
                                QString originalStyle = btn->property("originalStyle").toString();
                                btn->setStyleSheet(originalStyle);
                                break;
                            }
                        }
                    }
                    
                    // Select new
                    tempSelectedSeat = seatId;
                    QString originalStyle = seatBtn->property("originalStyle").toString();
                    QString selectedStyle = originalStyle;
                    selectedStyle.replace("2px solid", "3px solid #FF0000");
                    seatBtn->setStyleSheet(selectedStyle);
                });
            }
            
            seatGridLayout->addWidget(seatBtn, layoutRow, col + 1);
            
            if (col == cols - 1) {
                layoutRow++;
            }
        }
        
        seatMainLayout->addLayout(seatGridLayout);
        
        // OK/Cancel buttons
        auto* seatBtnLayout = new QHBoxLayout();
        auto* okBtn = new QPushButton("OK", seatDialog);
        auto* cancelSeatBtn = new QPushButton("Hủy", seatDialog);
        seatBtnLayout->addStretch();
        seatBtnLayout->addWidget(okBtn);
        seatBtnLayout->addWidget(cancelSeatBtn);
        seatMainLayout->addLayout(seatBtnLayout);
        
        connect(okBtn, &QPushButton::clicked, [seatDialog, &tempSelectedSeat, &selectedSeatId, selectedSeatEdit]() {
            if (!tempSelectedSeat.isEmpty()) {
                selectedSeatId = tempSelectedSeat;
                selectedSeatEdit->setText(selectedSeatId);
                seatDialog->accept();
            } else {
                QMessageBox::warning(seatDialog, "Lỗi", "Vui lòng chọn ghế.");
            }
        });
        
        connect(cancelSeatBtn, &QPushButton::clicked, seatDialog, &QDialog::reject);
        
        seatDialog->exec();
        delete seatDialog;
        
        // Enable confirm button if seat is selected
        if (!selectedSeatId.isEmpty()) {
            confirmBtn->setEnabled(true);
        }
    });
    
    // Connect confirm button
    connect(confirmBtn, &QPushButton::clicked, [=, &selectedInstance, &selectedSeatId, this, changeDialog]() {
        if (!selectedInstance || selectedSeatId.isEmpty()) {
            QMessageBox::warning(changeDialog, "Lỗi", "Vui lòng chọn chuyến bay và ghế.");
            return;
        }
        
        // Confirm change
        auto reply = QMessageBox::question(changeDialog, "Xác nhận đổi vé",
            QString("Bạn có chắc chắn muốn đổi vé?\n\n"
                   "Chuyến bay mới: %1\n"
                   "Ghế mới: %2\n\n"
                   "⚠️ Hành động này KHÔNG THỂ hoàn tác.")
                .arg(QString::fromStdString(selectedInstance->getFlightNumber()))
                .arg(selectedSeatId),
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::No) {
            return;
        }
        
        // Call changeBooking
        SeatManager* seatManager = flightManager_->getSeatManager();
        bool success = bookingManager_->changeBooking(
            *flightManager_,
            *seatManager,
            bookingId.toStdString(),
            selectedInstance->getInstanceId(),
            selectedSeatId.toStdString()
        );
        
        if (success) {
            QMessageBox::information(changeDialog, "Thành công",
                QString("Đã đổi vé thành công!\n\n"
                       "Mã đặt chỗ: %1\n"
                       "Chuyến bay mới: %2\n"
                       "Ghế mới: %3")
                    .arg(bookingId)
                    .arg(QString::fromStdString(selectedInstance->getFlightNumber()))
                    .arg(selectedSeatId));
            
            changeDialog->accept();
            refreshTable(); // Refresh the booking table
        } else {
            QMessageBox::critical(changeDialog, "Thất bại",
                "Đổi vé thất bại.\n\n"
                "Có thể do:\n"
                "- Ghế đã được đặt\n"
                "- Lỗi lưu dữ liệu\n"
                "- Vé không hợp lệ\n\n"
                "Vui lòng thử lại hoặc liên hệ hỗ trợ.");
        }
    });
    
    connect(cancelBtn, &QPushButton::clicked, changeDialog, &QDialog::reject);
    
    changeDialog->exec();
    delete changeDialog;
}

/**
 * @brief Refresh page when shown or when user changes
 */
void AgentBookingsPage::refreshPage() {
    // Clear search fields
    bookingIdSearchEdit_->clear();
    fromSearchCombo_->setCurrentIndex(0);
    toSearchCombo_->setCurrentIndex(0);
    dateSearchEdit_->setDate(QDate::currentDate());
    
    // Reload table with current user's bookings
    refreshTable();
}
