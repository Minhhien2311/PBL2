#include "AdminBookingsPage.h"
#include "core/PassengerManager.h"
#include "core/BookingManager.h"
#include "core/FlightManager.h"
#include "core/AccountManager.h"
#include "core/AirportManager.h"
#include "core/SeatManager.h"
#include "entities/Booking.h"
#include "entities/Account.h"
#include "entities/Flight.h"
#include "entities/Seat.h"
#include "BookingDetailsDialog.h"
#include "ChangeBookingDialog.h"
#include "AirportComboBox.h"
#include "BoldItemDelegate.h"
#include "PageRefresher.h"
#include <string>
#include <iostream>

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
#include <QCalendarWidget>
#include <QString>
#include <QDialog>
#include <QGroupBox>
#include <QTextEdit>
#include <QItemSelection>
#include <QItemSelectionModel>

// Helper function format tiền (giống SearchBookPage)
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

AdminBookingsPage::AdminBookingsPage(BookingManager* bkManager,
                                     FlightManager* flManager,
                                     AccountManager* accManager,
                                     AirportManager* airportManager,
                                     PassengerManager* passengerManager,  // Thêm tham số
                                     QWidget *parent)
    : QWidget(parent),
      bookingManager_(bkManager),
      flightManager_(flManager),
      accountManager_(accManager),
      airportManager_(airportManager),
      passengerManager_(passengerManager)  // Khởi tạo thành viên mới
{
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(accountManager_ != nullptr);
    Q_ASSERT(passengerManager_ != nullptr);  // Thêm assert

    setupUi();
    setupModel();
    setupConnections();
    
    refreshTable(); // Tải dữ liệu lần đầu
}

void AdminBookingsPage::setupUi()
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

    // === Hàng 1: Nút Tải lại ===
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);

    headerRow->addStretch();

    // ← NÚT TẢI LẠI (góc phải trên)
    refreshButton = new QPushButton("Làm mới trang", top);
    
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

    // ========== HÀNG TÌM KIẾM (2 BOX NGANG) ==========
    QHBoxLayout* searchRowLayout = new QHBoxLayout();
    searchRowLayout->setSpacing(16);

    // ========== BOX 1: TÌM THEO MÃ ĐẶT CHỖ ==========
    QWidget* searchBox1 = new QWidget;
    QVBoxLayout* box1Layout = new QVBoxLayout(searchBox1);
    box1Layout->setContentsMargins(12, 12, 12, 12);
    box1Layout->setSpacing(8);
    
    searchBox1->setStyleSheet(
        "QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }"
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
        "QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }"
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

    // ================== STATUS + CÁC NÚT (GỘP CHUNG 1 HÀNG) ==================
    auto *tableHeader = new QWidget(this);
    auto *thLayout = new QHBoxLayout(tableHeader);
    // Căn lề 24px để thẳng hàng với nội dung bên trên
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(10);

    // 1. Status label
    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    thLayout->addWidget(statusLabel_);

    // 2. Lò xo đẩy các nút sang phải
    thLayout->addStretch();

    // 3. Các nút hành động
    viewDetailsBtn_ = new QPushButton("Xem chi tiết vé", this);
    cancelBookingBtn_ = new QPushButton("Hủy vé", this);
    changeBookingBtn_ = new QPushButton("Đổi vé", this);

    viewDetailsBtn_->setStyleSheet("background: #27C93F; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #1b4d1b; color: white;");
    changeBookingBtn_->setStyleSheet("background: #FFBD2E; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #8b1c1c; color: white;");
    cancelBookingBtn_->setStyleSheet("background: #FF5F57; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #b35c00; color: white;");
    
    // Set cursor
    viewDetailsBtn_->setCursor(Qt::PointingHandCursor);
    cancelBookingBtn_->setCursor(Qt::PointingHandCursor);
    changeBookingBtn_->setCursor(Qt::PointingHandCursor);

    // Add nút vào layout
    thLayout->addWidget(viewDetailsBtn_);
    thLayout->addWidget(changeBookingBtn_); // Thường "Đổi" để cạnh "Xem"
    thLayout->addWidget(cancelBookingBtn_); // "Hủy" để ngoài cùng hoặc tùy ý bạn sắp xếp

    // Add Header vào Main Layout
    mainLayout->addWidget(tableHeader);

    // ================== BẢNG ==================
    auto *tableBox = new QWidget(this);
    auto *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setStyleSheet(
        "QTableView { background:white; border:0px solid #133e87; }"
    );
    tableView_->setItemDelegate(new BoldItemDelegate(this));
    
    // --- CẤU HÌNH GIAO DIỆN BẢNG ---
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    tableView_->verticalHeader()->setVisible(false); // Tắt header dọc mặc định (số 1,2,3 xấu của Qt)
    tableView_->setAlternatingRowColors(true);       // Màu dòng xen kẽ
    tableView_->setShowGrid(false);                  // Tắt lưới mặc định

    // --- XỬ LÝ SCROLLBAR TRIỆT ĐỂ (Giống SearchBookPage) ---
    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Cấm thanh ngang
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);    // Thanh dọc tự động
    
    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);
}


void AdminBookingsPage::setupModel()
{
    // Tăng lên 8 cột (Thêm STT vào đầu)
    model_ = new QStandardItemModel(0, 8, this);
    
    model_->setHorizontalHeaderLabels({
        "STT",              // Cột 0
        "Mã Đặt chỗ",       // Cột 1
        "Mã Chuyến",        // Cột 2
        "CCCD Khách",       // Cột 3
        "Ngày giờ đặt",     // Cột 4
        "Hạng vé",          // Cột 5
        "Giá vé",           // Cột 6
        "Trạng thái",        // Cột 7
        "Người đặt"          // Cột 8
    });

    tableView_->setModel(model_);

    // --- CẤU HÌNH HEADER (Logic co giãn) ---
    QHeaderView *header = tableView_->horizontalHeader();

    // 1. Mặc định tất cả co theo nội dung chữ
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 2. Chọn Cột làm LÒ XO (Stretch)
    // Ở đây ta chọn cột "Ngày giờ đặt" (Cột 4) hoặc "Mã Đặt chỗ" (Cột 1) để giãn ra
    // Chọn Cột 4 (Ngày giờ đặt) làm lò xo nhìn sẽ cân đối nhất
    header->setSectionResizeMode(QHeaderView::Stretch);

    // 3. Cố định cột STT (Cột 0) không cho giãn
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView_->setColumnWidth(0, 50);
}

void AdminBookingsPage::onSelectionChanged(const QItemSelection &selected, const QItemSelection & /*deselected*/)
{
    QModelIndexList sel = tableView_->selectionModel()->selectedRows();
    if (sel.isEmpty()) {
        viewDetailsBtn_->setEnabled(false);
        changeBookingBtn_->setEnabled(false);
        cancelBookingBtn_->setEnabled(false);
        statusLabel_->setText("");
        return;
    }

    // BookingId ở cột 1 theo layout hiện tại
    QString bookingId = model_->data(sel.first().siblingAtColumn(1)).toString();
    Booking* booking = bookingManager_->findBookingById(bookingId.toStdString());
    if (!booking) {
        viewDetailsBtn_->setEnabled(false);
        changeBookingBtn_->setEnabled(false);
        cancelBookingBtn_->setEnabled(false);
        statusLabel_->setText("Không tìm thấy đặt chỗ.");
        return;
    }

    viewDetailsBtn_->setEnabled(true);

    // Kiểm tra luật hủy/đổi qua BookingManager
    bool canCancel = bookingManager_->canCancelBooking(bookingId.toStdString(), *flightManager_);
    bool canChange = bookingManager_->canChangeBooking(bookingId.toStdString(), *flightManager_);

    // Chỉ cho thao tác nếu booking đang giữ chỗ (Issued) và luật cho phép
    bool isIssued = (booking->getStatus() == BookingStatus::Issued);
    cancelBookingBtn_->setEnabled(isIssued && canCancel);
    changeBookingBtn_->setEnabled(isIssued && canChange);

    // Hiển thị thông tin ngắn gọn trên statusLabel (hoặc tooltip)
    QString info;
    if (!canCancel) {
        std::string deadline = bookingManager_->getCancellationDeadline(bookingId.toStdString(), *flightManager_);
        info += QString("Không thể hủy. Hạn hủy: %1. ").arg(QString::fromStdString(deadline));
    }
    if (!canChange) {
        std::string deadline = bookingManager_->getChangeDeadline(bookingId.toStdString(), *flightManager_);
        info += QString("Không thể đổi. Hạn đổi: %1.").arg(QString::fromStdString(deadline));
    }
    if (info.isEmpty()) {
        statusLabel_->setText("Sẵn sàng thao tác (Có thể đổi/hủy nếu cần).");
        statusLabel_->setStyleSheet("color: #27C93F; font-weight:650; ");
    } else {
        statusLabel_->setText(info);
        statusLabel_->setStyleSheet("color: #FF5F57; font-weight:650; ");
    }

    // Thêm tooltip cho hàng (tùy chọn)
    // Đặt tooltip lên ô Mã Đặt chỗ để agent thấy nhanh
    QStandardItem* idItem = model_->item(sel.first().row(), 1);
    if (idItem) {
        idItem->setToolTip(info);
    }
}

void AdminBookingsPage::setupConnections()
{
    // 2 nút tìm kiếm
    connect(searchButton_, &QPushButton::clicked, this, &AdminBookingsPage::onSearchByBookingId);
    connect(searchByPassengerBtn_, &QPushButton::clicked, this, &AdminBookingsPage::onSearchByPassengerId);
    
    // Các nút khác
    connect(refreshButton, &QPushButton::clicked, this, &AdminBookingsPage::refreshPage);
    connect(cancelBookingBtn_, &QPushButton::clicked, this, &AdminBookingsPage::onCancelBookingClicked);
    connect(viewDetailsBtn_, &QPushButton::clicked, this, &AdminBookingsPage::onViewDetailsClicked);
    connect(changeBookingBtn_, &QPushButton::clicked, this, &AdminBookingsPage::onChangeBookingClicked);

    connect(tableView_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AdminBookingsPage::onSelectionChanged);
}

// Hàm này tải (hoặc làm mới) TOÀN BỘ vé của Agent
void AdminBookingsPage::refreshTable()
{
    model_->removeRows(0, model_->rowCount());

    // 2. Lấy toàn bộ danh sách booking 
    std::vector<Booking*> allBookings = bookingManager_->getAllBookings();

    // 3. Hiển thị các booking
    for (Booking* booking : allBookings) {
        displayBooking(booking);
    }

    statusLabel_->setText(
        QString("Hiển thị tất cả %1 đặt chỗ").arg(allBookings.size())
    );
}

void AdminBookingsPage::onCancelBookingClicked()
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
    QModelIndex idIndex = selected.first().siblingAtColumn(1);
    QModelIndex statusIndex = selected.first().siblingAtColumn(7);  // Status column
    
    QString bookingId = model_->data(idIndex).toString();
    QString status = model_->data(statusIndex).toString();

    // 2. Kiểm tra xem đã hủy chưa
    if (status == "Đã hủy") {
        QMessageBox::warning(this, "Lỗi", "Vé này đã được hủy từ trước.");
        return;
    }

    // 3. Check if can cancel (time constraint)
    if (!bookingManager_->canCancelBooking(bookingId.toStdString(), *flightManager_)) {
        QString deadline = QString::fromStdString(
            bookingManager_->getCancellationDeadline(bookingId.toStdString(), *flightManager_)
        );
        QMessageBox::warning(this, "Không thể hủy", 
            QString("Không thể hủy vé này vì đã quá hạn.\nHạn hủy: %1").arg(deadline));
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

void AdminBookingsPage::onViewDetailsClicked()
{
    // 1. Lấy hàng đang chọn
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một vé để xem chi tiết.");
        return;
    }
    
    // 2. Lấy Booking ID từ cột đầu tiên
    QModelIndex idIndex = selected.first().siblingAtColumn(1);
    QString bookingId = model_->data(idIndex).toString();
    
    // 3. Tìm booking từ BookingManager
    Booking* booking = bookingManager_->findBookingById(bookingId.toStdString());
    
    if (!booking) {
        std::cout << "Booking not found for ID: " << bookingId.toStdString() << std::endl;
        return;
    }
    
    // 4. Hiển thị dialog chi tiết 
    BookingDetailsDialog dialog(booking, flightManager_, accountManager_, passengerManager_, this);
    dialog.exec();
}

void AdminBookingsPage::onChangeBookingClicked()
{
    // 1. Get selected row
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        statusLabel_->setText("Vui lòng chọn một vé để đổi!");
        statusLabel_->setStyleSheet("color: #FF5F57;"); // Màu đỏ
        return;
    }
    
    // Get Booking ID and Status
    QString bookingId = model_->data(selected.first().siblingAtColumn(1)).toString();
    QString status = model_->data(selected.first().siblingAtColumn(7)).toString();
    
    // 2. Validate status is Issued
    if (status != "Đang giữ chỗ") {
        statusLabel_->setText("Chỉ có thể đổi vé có trạng thái 'Đang giữ chỗ'.");
        statusLabel_->setStyleSheet("color: #FF5F57;"); // Màu đỏ
        return;
    }

    // Kiểm tra luật đổi
    if (!bookingManager_->canChangeBooking(bookingId.toStdString(), *flightManager_)) {
        QString deadline = QString::fromStdString(
            bookingManager_->getChangeDeadline(bookingId.toStdString(), *flightManager_)
        );
        QMessageBox::warning(this, "Không thể đổi", 
            QString("Không thể đổi vé này vì đã quá hạn.\nHạn đổi: %1").arg(deadline));
        return;
    }
    
    // 3. Find booking
    Booking* booking = bookingManager_->findBookingById(bookingId.toStdString());
    if (!booking) {
        std::cout << "Booking not found for ID: " << bookingId.toStdString() << std::endl;
        return;
    }
    
    // 4. Open ChangeBookingDialog
    ChangeBookingDialog dialog(booking, bookingManager_, flightManager_, airportManager_, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshTable();
    }
}

// ========== HÀM HELPER: HIỂN THỊ 1 BOOKING ==========
void AdminBookingsPage::displayBooking(Booking* booking)
{
    if (!booking) return;
    
    // Tính số thứ tự dựa trên số dòng hiện có
    int stt = model_->rowCount() + 1;

    QList<QStandardItem*> rowItems;
    
    // 0. STT
    rowItems << new QStandardItem(QString::number(stt));
    
    // 1. Mã Đặt chỗ
    rowItems << new QStandardItem(QString::fromStdString(booking->getBookingId()));
    
    // 2. Mã Chuyến
    rowItems << new QStandardItem(QString::fromStdString(booking->getFlightId()));
    
    // 3. CCCD
    rowItems << new QStandardItem(QString::fromStdString(booking->getPassengerId()));
    
    // 4. Ngày giờ đặt
    rowItems << new QStandardItem(QString::fromStdString(booking->getBookingDate()));
    
    // 5. Hạng vé
    QString classStr = (booking->getClass() == BookingClass::Economy) 
                      ? "Phổ thông" : "Thương gia";
    rowItems << new QStandardItem(classStr);
    
    // 6. Giá vé (Format đẹp)
    rowItems << new QStandardItem(formatVietnamCurrency(booking->getBaseFare()));

    // 7. Trạng thái
    QString statusStr;
    QColor statusColor;

    if (booking->getStatus() == BookingStatus::Issued) {
        statusStr = "Đang giữ chỗ";
        statusColor = QColor("#27C93F"); // Xanh lá đậm
    } else if (booking->getStatus() == BookingStatus::Cancelled) {
        statusStr = "Đã hủy";
        statusColor = QColor("#FF5F57"); // Đỏ đậm
    } else {
        statusStr = "Đã đổi";
        statusColor = QColor("#FFBD2E"); // Cam đậm (cho trạng thái khác)
    }
    rowItems << new QStandardItem(statusStr);
    rowItems.last()->setForeground(statusColor);

    // 8. Agent đặt vé
    rowItems << new QStandardItem(QString::fromStdString(booking->getAgentId()));
    
    // CANH GIỮA TẤT CẢ
    for (QStandardItem *item : rowItems) {
        item->setTextAlignment(Qt::AlignCenter);
    }

    // sau khi tạo rowItems và trước model_->appendRow(rowItems);
    QString cancelDeadline = QString::fromStdString(
        bookingManager_->getCancellationDeadline(booking->getBookingId(), *flightManager_));
    QString changeDeadline = QString::fromStdString(
        bookingManager_->getChangeDeadline(booking->getBookingId(), *flightManager_));
    QString tip = QString("Hạn hủy: %1\nHạn đổi: %2").arg(cancelDeadline, changeDeadline);
    for (QStandardItem *item : rowItems) item->setToolTip(tip);

    model_->appendRow(rowItems);
}

// ========== 1. TÌM THEO MÃ ĐẶT CHỖ ==========
void AdminBookingsPage::onSearchByBookingId()
{
    QString input = bookingIdSearchEdit_->text().trimmed();
    
    if (input.isEmpty()) {
        statusLabel_->setText("Vui lòng nhập mã đặt chỗ cần tìm!");
        statusLabel_->setStyleSheet("color: #FF5F57;"); // Màu đỏ
        return;
    }
    
    std::string bookingId = input.toStdString();
    
    // Tìm booking
    Booking* booking = bookingManager_->findBookingById(bookingId);
    
    // Xóa dữ liệu cũ trong bảng
    model_->removeRows(0, model_->rowCount());
    
    if (!booking) {
        statusLabel_->setText("Không tìm thấy vé với mã " + input + "!");
        statusLabel_->setStyleSheet("color: #FF5F57;"); // Màu đỏ
        return;
    }
    
    // Hiển thị kết quả
    displayBooking(booking);

    statusLabel_->setText("Tìm thấy 1 vé với mã " + input + "!");
    statusLabel_->setStyleSheet("color: #27C93F;"); // Màu xanh lá
}

// ========== 2. TÌM THEO CCCD KHÁCH HÀNG ==========
void AdminBookingsPage::onSearchByPassengerId()
{
    QString input = passengerIdSearchEdit_->text().trimmed();
    
    if (input.isEmpty()) {
        statusLabel_->setText("Vui lòng nhập CCCD khách hàng cần tìm!");
        statusLabel_->setStyleSheet("color: #FF5F57;"); // Màu đỏ
        return;
    }
    
    std::string passengerId = input.toStdString();
    
    // Lấy thông tin Agent hiện tại
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        std::cout << "Không thể xác định người dùng. Vui lòng đăng nhập lại." << std::endl;
        return;
    }
    std::string currentAgentId = currentUser->getId();
    
    // Lọc theo CCCD khách hàng
    std::vector<Booking*> results = bookingManager_->findBookingsByPassengerId(passengerId);
    
    // Xóa dữ liệu cũ trong bảng
    model_->removeRows(0, model_->rowCount());
    
    if (results.empty()) {
        statusLabel_->setText("Không tìm thấy vé với CCCD " + input + "!");
        statusLabel_->setStyleSheet("color: #FF5F57;"); // Màu đỏ
        return;
    }
    
    // Hiển thị tất cả kết quả
    for (Booking* booking : results) {
        displayBooking(booking);
    }

    statusLabel_->setText(
    QString("🔍 Tìm thấy %1 đặt chỗ").arg(results.size())
    );
    
    statusLabel_->setText(
        QString("Tìm thấy %1 vé với CCCD %2").arg(results.size()).arg(input)
    );
    statusLabel_->setStyleSheet("color: #27C93F;"); // Màu xanh lá
}

void AdminBookingsPage::refreshPage() {
    PageRefresher::clearSearchFields(this);
    PageRefresher::executeRefresh([this]() {
        refreshTable();
    });
}
