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
#include "ChangeBookingDialog.h"  // Dialog đổi vé
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

    // === Hàng 1: Tiêu đề + Nút Tải lại ===
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);

    auto *title = new QLabel("Tìm thông tin đặt chỗ", this);
    title->setProperty("class", "PageTitle");
    headerRow->addWidget(title);
    headerRow->addStretch();

    // ← NÚT TẢI LẠI (góc phải trên)
    refreshButton_ = new QPushButton("🔄 Tải lại tất cả");
    refreshButton_->setStyleSheet(
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:6px; height:32px; padding:0 16px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }"
    );
    refreshButton_->setCursor(Qt::PointingHandCursor);
    refreshButton_->setMinimumWidth(140);
    headerRow->addWidget(refreshButton_);

    topLayout->addLayout(headerRow);

    // ========== HÀNG TÌM KIẾM (2 BOX NGANG) ==========
    QHBoxLayout* searchRowLayout = new QHBoxLayout();
    searchRowLayout->setSpacing(16);

    // ========== BOX 1: TÌM THEO MÃ ĐẶT CHỖ ==========
    QWidget* searchBox1 = new QWidget;
    QVBoxLayout* box1Layout = new QVBoxLayout(searchBox1);
    box1Layout->setContentsMargins(12, 12, 12, 12);
    box1Layout->setSpacing(8);
    
    searchBox1->setStyleSheet(
        "QWidget { background: white; border: 1px solid #c2cfe2; border-radius: 6px; }"
    );

    QLabel* box1Title = new QLabel("🔍 Tra cứu theo mã đặt chỗ");
    box1Title->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    box1Layout->addWidget(box1Title);

    QHBoxLayout* box1Row = new QHBoxLayout();
    box1Row->setSpacing(10);
    
    bookingIdSearchEdit_ = new QLineEdit;
    bookingIdSearchEdit_->setPlaceholderText("Nhập mã đặt chỗ (VD: BKG-0000001)");
    bookingIdSearchEdit_->setMinimumHeight(36);
    box1Row->addWidget(bookingIdSearchEdit_, 1);

    searchButton_ = new QPushButton("Tìm kiếm");
    searchButton_->setProperty("class", "SearchBtn");
    searchButton_->setMinimumHeight(36);
    searchButton_->setMinimumWidth(110);
    searchButton_->setCursor(Qt::PointingHandCursor);
    searchButton_->setStyleSheet(
        "QPushButton { background:#4478BD; color:white; font-weight:600; "
        "border-radius:6px; padding: 0 16px; }"
        "QPushButton:hover { background:#365a9e; }"
    );
    box1Row->addWidget(searchButton_);

    box1Layout->addLayout(box1Row);

    searchRowLayout->addWidget(searchBox1, 1);

    // ========== BOX 2: TÌM THEO CCCD ==========
    QWidget* searchBox2 = new QWidget;
    QVBoxLayout* box2Layout = new QVBoxLayout(searchBox2);
    box2Layout->setContentsMargins(12, 12, 12, 12);
    box2Layout->setSpacing(8);
    
    searchBox2->setStyleSheet(
        "QWidget { background: white; border: 1px solid #c2cfe2; border-radius: 6px; }"
    );

    QLabel* box2Title = new QLabel("👤 Tra cứu theo CCCD khách hàng");
    box2Title->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    box2Layout->addWidget(box2Title);

    QHBoxLayout* box2Row = new QHBoxLayout();
    box2Row->setSpacing(10);
    
    passengerIdSearchEdit_ = new QLineEdit;
    passengerIdSearchEdit_->setPlaceholderText("Nhập CCCD khách hàng");
    passengerIdSearchEdit_->setMinimumHeight(36);
    box2Row->addWidget(passengerIdSearchEdit_, 1);

    searchByPassengerBtn_ = new QPushButton("Tìm kiếm");
    searchByPassengerBtn_->setProperty("class", "SearchBtn");
    searchByPassengerBtn_->setMinimumHeight(36);
    searchByPassengerBtn_->setMinimumWidth(110);
    searchByPassengerBtn_->setCursor(Qt::PointingHandCursor);
    searchByPassengerBtn_->setStyleSheet(
        "QPushButton { background:#4478BD; color:white; font-weight:600; "
        "border-radius:6px; padding: 0 16px; }"
        "QPushButton:hover { background:#365a9e; }"
    );
    box2Row->addWidget(searchByPassengerBtn_);

    box2Layout->addLayout(box2Row);

    searchRowLayout->addWidget(searchBox2, 1);

    // Thêm layout ngang vào topLayout
    topLayout->addLayout(searchRowLayout);
    mainLayout->addWidget(top);

    // ================== TIÊU ĐỀ BẢNG + STATUS ==================
    auto *tableHeader = new QWidget(this);
    auto *thLayout = new QHBoxLayout(tableHeader);
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(10);

    auto *tblTitle = new QLabel("📋 Kết quả tìm kiếm", this);
    tblTitle->setObjectName("tableTitle");
    tblTitle->setProperty("class", "SectionTitle");
    thLayout->addWidget(tblTitle);

    // Status label (hiển thị số kết quả)
    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 12px;");
    thLayout->addWidget(statusLabel_);

    thLayout->addStretch();

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
        "Mã Đặt chỗ", "Mã Chuyến", "CCCD hành khách", 
        "Ngày giờ đặt", "Hạng vé", "Giá vé", "Trạng thái"
    });
    tableView_->setModel(model_);
}

void AgentBookingsPage::setupConnections()
{
    // 2 nút tìm kiếm
    connect(searchButton_, &QPushButton::clicked, this, &AgentBookingsPage::onSearchByBookingId);
    connect(searchByPassengerBtn_, &QPushButton::clicked, this, &AgentBookingsPage::onSearchByPassengerId);
    
    // Các nút khác
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
        // QMessageBox::warning(this, "Lỗi", "Không thể xác định người dùng. Vui lòng đăng nhập lại.");
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

    statusLabel_->setText(
        QString("Hiển thị tất cả %1 đặt chỗ").arg(agentBookings.size())
    );
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
    QString bookingId = model_->data(selected.first().siblingAtColumn(0)).toString();
    QString status = model_->data(selected.first().siblingAtColumn(6)).toString();
    
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
    
    // 4. Open ChangeBookingDialog
    ChangeBookingDialog dialog(booking, bookingManager_, flightManager_, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshTable();
    }
}

// ========== HÀM HELPER: HIỂN THỊ 1 BOOKING ==========
void AgentBookingsPage::displayBooking(Booking* booking)
{
    if (!booking) return;
    
    QList<QStandardItem*> rowItems;
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

// ========== 1. TÌM THEO MÃ ĐẶT CHỖ ==========
void AgentBookingsPage::onSearchByBookingId()
{
    QString input = bookingIdSearchEdit_->text().trimmed();
    
    if (input.isEmpty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", 
            "Vui lòng nhập mã đặt chỗ cần tìm.");
        return;
    }
    
    std::string bookingId = input.toStdString();
    
    // Lấy thông tin Agent hiện tại
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        QMessageBox::warning(this, "Lỗi", "Không thể xác định người dùng. Vui lòng đăng nhập lại.");
        return;
    }
    std::string currentAgentId = currentUser->getId();
    
    // Tìm booking
    Booking* booking = bookingManager_->findBookingById(bookingId);
    
    // Xóa dữ liệu cũ trong bảng
    model_->removeRows(0, model_->rowCount());
    
    if (!booking) {
        QMessageBox::information(this, "Không tìm thấy", 
            QString("Không tìm thấy đặt chỗ với mã:\n%1\n\n"
                   "Vui lòng kiểm tra lại mã đặt chỗ.")
                .arg(input));
        return;
    }
    
    // Kiểm tra booking có thuộc về Agent này không
    if (booking->getAgentId() != currentAgentId) {
        QMessageBox::warning(this, "Không có quyền truy cập", 
            "Đặt chỗ này không thuộc về bạn.\n\n"
            "Bạn chỉ có thể xem các đặt chỗ do chính bạn tạo.");
        return;
    }
    
    // Hiển thị kết quả
    displayBooking(booking);

    // ← THÊM: Cập nhật status
    statusLabel_->setText("✅ Tìm thấy 1 đặt chỗ");
    
    QMessageBox::information(this, "Tìm thấy", 
        QString("Đã tìm thấy đặt chỗ: %1").arg(input));
}

// ========== 2. TÌM THEO CCCD KHÁCH HÀNG ==========
void AgentBookingsPage::onSearchByPassengerId()
{
    QString input = passengerIdSearchEdit_->text().trimmed();
    
    if (input.isEmpty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", 
            "Vui lòng nhập CCCD khách hàng cần tìm.");
        return;
    }
    
    std::string passengerId = input.toStdString();
    
    // Lấy thông tin Agent hiện tại
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        QMessageBox::warning(this, "Lỗi", "Không thể xác định người dùng. Vui lòng đăng nhập lại.");
        return;
    }
    std::string currentAgentId = currentUser->getId();
    
    // Lọc theo CCCD khách hàng
    std::vector<Booking*> results = bookingManager_->findBookingsByPassengerId(passengerId);
    
    // Xóa dữ liệu cũ trong bảng
    model_->removeRows(0, model_->rowCount());
    
    if (results.empty()) {
        QMessageBox::information(this, "Không tìm thấy", 
            QString("Không tìm thấy đặt chỗ nào cho CCCD:\n%1\n\n"
                   "Có thể:\n"
                   "• CCCD không đúng\n"
                   "• Khách hàng này chưa đặt vé với bạn")
                .arg(input));
        return;
    }
    
    // Hiển thị tất cả kết quả
    for (Booking* booking : results) {
        displayBooking(booking);
    }

    statusLabel_->setText(
    QString("🔍 Tìm thấy %1 đặt chỗ").arg(results.size())
    );
    
    QMessageBox::information(this, "Kết quả tìm kiếm", 
        QString("Tìm thấy %1 đặt chỗ cho CCCD:\n%2")
            .arg(results.size())
            .arg(input));
}

void AgentBookingsPage::refreshPage() {
    // Clear search fields
    bookingIdSearchEdit_->clear();
    passengerIdSearchEdit_->clear();
    
    // Reload table with current user's bookings
    refreshTable();
}
