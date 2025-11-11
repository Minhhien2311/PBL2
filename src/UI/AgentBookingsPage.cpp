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
        "Mã Đặt chỗ", "Mã Chuyến", "CCCD hành khách", 
        "Ngày giờ đặt", "Hạng vé", "Giá vé", "Trạng thái"
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
