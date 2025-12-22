#include "AdminBookingsPage.h"

#include "AirportComboBox.h"
#include "BoldItemDelegate.h"
#include "BookingDetailsDialog.h"
#include "ChangeBookingDialog.h"
#include "PageRefresher.h"
#include "core/AccountManager.h"
#include "core/AirportManager.h"
#include "core/BookingManager.h"
#include "core/FlightManager.h"
#include "core/PassengerManager.h"
#include "core/SeatManager.h"
#include "entities/Account.h"
#include "entities/Booking.h"
#include "entities/Flight.h"
#include "entities/Seat.h"

#include <QCalendarWidget>
#include <QDateEdit>
#include <QDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QString>
#include <QTableView>
#include <QTextEdit>
#include <QVBoxLayout>

#include <iostream>
#include <string>

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
                                     PassengerManager* passengerManager,
                                     QWidget *parent)
    : QWidget(parent),
      bookingManager_(bkManager),
      flightManager_(flManager),
      accountManager_(accManager),
      airportManager_(airportManager),
      passengerManager_(passengerManager)
{
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(accountManager_ != nullptr);
    Q_ASSERT(passengerManager_ != nullptr);

    setupUi();
    setupModel();
    setupConnections();
    
    refreshTable();
}

void AdminBookingsPage::setupUi()
{
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
    
    // ========== TOP BAR ==========
    auto *top = new QWidget(this);
    auto *topLayout = new QVBoxLayout(top);
    topLayout->setContentsMargins(24, 20, 24, 10);
    topLayout->setSpacing(10);

    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);
    headerRow->addStretch();

    refreshButton = new QPushButton("Làm mới trang", top);
    refreshButton->setIcon(QIcon("C:/PBL2/assets/icons/reload.png"));
    refreshButton->setIconSize(QSize(14, 14));
    refreshButton->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #133e87;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   border: none;"
        "   text-align: left;"
        "   padding: 0px;"
        "}"
        "QPushButton:hover {"
        "   background: transparent;"
        "   text-decoration: underline;"
        "}"
    );
    refreshButton->setCursor(Qt::PointingHandCursor);
    headerRow->addWidget(refreshButton);
    topLayout->addLayout(headerRow);

    // ========== SEARCH ROW ==========
    QHBoxLayout* searchRowLayout = new QHBoxLayout();
    searchRowLayout->setSpacing(16);

    // Search Box 1: Booking ID
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

    // Search Box 2: Passenger ID
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

    topLayout->addLayout(searchRowLayout);
    mainLayout->addWidget(top);

    // ========== STATUS + BUTTONS ==========
    auto *tableHeader = new QWidget(this);
    auto *thLayout = new QHBoxLayout(tableHeader);
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(10);

    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    thLayout->addWidget(statusLabel_);

    thLayout->addStretch();

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

    thLayout->addWidget(viewDetailsBtn_);
    thLayout->addWidget(changeBookingBtn_);
    thLayout->addWidget(cancelBookingBtn_);

    mainLayout->addWidget(tableHeader);

    // ========== TABLE ==========
    auto *tableBox = new QWidget(this);
    auto *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setStyleSheet(
        "QTableView { background:white; border:0px solid #133e87; }"
    );
    tableView_->setItemDelegate(new BoldItemDelegate(this));
    
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    tableView_->verticalHeader()->setVisible(false);
    tableView_->setAlternatingRowColors(true);
    tableView_->setShowGrid(false);

    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);
}


void AdminBookingsPage::setupModel()
{
    model_ = new QStandardItemModel(0, 8, this);
    
    model_->setHorizontalHeaderLabels({
        "STT",
        "Mã Đặt chỗ",
        "Mã Chuyến",
        "CCCD Khách",
        "Ngày giờ đặt",
        "Hạng vé",
        "Giá vé",
        "Trạng thái",
        "Người đặt"
    });

    tableView_->setModel(model_);

    QHeaderView *header = tableView_->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(QHeaderView::Stretch);
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

    bool canCancel = bookingManager_->canCancelBooking(bookingId.toStdString(), *flightManager_);
    bool canChange = bookingManager_->canChangeBooking(bookingId.toStdString(), *flightManager_);

    bool isIssued = (booking->getStatus() == BookingStatus::Issued);
    cancelBookingBtn_->setEnabled(isIssued && canCancel);
    changeBookingBtn_->setEnabled(isIssued && canChange);

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

// ========== CONNECTIONS ==========
void AdminBookingsPage::setupConnections()
{
    connect(searchButton_, &QPushButton::clicked, this, &AdminBookingsPage::onSearchByBookingId);
    connect(searchByPassengerBtn_, &QPushButton::clicked, this, &AdminBookingsPage::onSearchByPassengerId);
    
    connect(refreshButton, &QPushButton::clicked, this, &AdminBookingsPage::refreshPage);
    connect(cancelBookingBtn_, &QPushButton::clicked, this, &AdminBookingsPage::onCancelBookingClicked);
    connect(viewDetailsBtn_, &QPushButton::clicked, this, &AdminBookingsPage::onViewDetailsClicked);
    connect(changeBookingBtn_, &QPushButton::clicked, this, &AdminBookingsPage::onChangeBookingClicked);

    connect(tableView_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AdminBookingsPage::onSelectionChanged);
}

// ========== SLOTS ==========
void AdminBookingsPage::refreshTable()
{
    model_->removeRows(0, model_->rowCount());

    std::vector<Booking*> allBookings = bookingManager_->getAllBookings();

    for (Booking* booking : allBookings) {
        displayBooking(booking);
    }

    statusLabel_->setText(
        QString("Hiển thị tất cả %1 đặt chỗ").arg(allBookings.size())
    );
}

void AdminBookingsPage::onCancelBookingClicked()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một vé để hủy.");
        return;
    }
    
    QModelIndex idIndex = selected.first().siblingAtColumn(1);
    QModelIndex statusIndex = selected.first().siblingAtColumn(7);
    
    QString bookingId = model_->data(idIndex).toString();
    QString status = model_->data(statusIndex).toString();

    if (status == "Đã hủy") {
        QMessageBox::warning(this, "Lỗi", "Vé này đã được hủy từ trước.");
        return;
    }

    if (!bookingManager_->canCancelBooking(bookingId.toStdString(), *flightManager_)) {
        QString deadline = QString::fromStdString(
            bookingManager_->getCancellationDeadline(bookingId.toStdString(), *flightManager_)
        );
        QMessageBox::warning(this, "Không thể hủy", 
            QString("Không thể hủy vé này vì đã quá hạn.\nHạn hủy: %1").arg(deadline));
        return;
    }

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
    
    bool success = bookingManager_->cancelBooking(*flightManager_, 
                                                  *flightManager_->getSeatManager(), 
                                                  bookingId.toStdString());

    if (success) {
        QMessageBox::information(this, "Thành công", 
            QString("Đã hủy vé thành công!\n\n"
                   "Mã đặt chỗ: %1\n"
                   "Ghế đã được trả lại hệ thống.\n"
                   "Trạng thái đã được cập nhật.").arg(bookingId));
        refreshTable();
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
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một vé để xem chi tiết.");
        return;
    }
    
    QModelIndex idIndex = selected.first().siblingAtColumn(1);
    QString bookingId = model_->data(idIndex).toString();
    
    Booking* booking = bookingManager_->findBookingById(bookingId.toStdString());
    
    if (!booking) {
        std::cout << "Booking not found for ID: " << bookingId.toStdString() << std::endl;
        return;
    }
    
    BookingDetailsDialog dialog(booking, flightManager_, accountManager_, passengerManager_, this);
    dialog.exec();
}

void AdminBookingsPage::onChangeBookingClicked()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        statusLabel_->setText("Vui lòng chọn một vé để đổi!");
        statusLabel_->setStyleSheet("color: #FF5F57;");
        return;
    }
    
    QString bookingId = model_->data(selected.first().siblingAtColumn(1)).toString();
    QString status = model_->data(selected.first().siblingAtColumn(7)).toString();
    
    if (status != "Đang giữ chỗ") {
        statusLabel_->setText("Chỉ có thể đổi vé có trạng thái 'Đang giữ chỗ'.");
        statusLabel_->setStyleSheet("color: #FF5F57;");
        return;
    }

    if (!bookingManager_->canChangeBooking(bookingId.toStdString(), *flightManager_)) {
        QString deadline = QString::fromStdString(
            bookingManager_->getChangeDeadline(bookingId.toStdString(), *flightManager_)
        );
        QMessageBox::warning(this, "Không thể đổi", 
            QString("Không thể đổi vé này vì đã quá hạn.\nHạn đổi: %1").arg(deadline));
        return;
    }
    
    Booking* booking = bookingManager_->findBookingById(bookingId.toStdString());
    if (!booking) {
        std::cout << "Booking not found for ID: " << bookingId.toStdString() << std::endl;
        return;
    }
    
    ChangeBookingDialog dialog(booking, bookingManager_, flightManager_, airportManager_, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshTable();
    }
}

// ========== HELPER FUNCTIONS ==========
void AdminBookingsPage::displayBooking(Booking* booking)
{
    if (!booking) return;
    
    int stt = model_->rowCount() + 1;

    QList<QStandardItem*> rowItems;
    
    rowItems << new QStandardItem(QString::number(stt));
    rowItems << new QStandardItem(QString::fromStdString(booking->getBookingId()));
    rowItems << new QStandardItem(QString::fromStdString(booking->getFlightId()));
    rowItems << new QStandardItem(QString::fromStdString(booking->getPassengerId()));
    rowItems << new QStandardItem(QString::fromStdString(booking->getBookingDate()));
    
    QString classStr = (booking->getClass() == BookingClass::Economy) 
                      ? "Phổ thông" : "Thương gia";
    rowItems << new QStandardItem(classStr);
    
    rowItems << new QStandardItem(formatVietnamCurrency(booking->getBaseFare()));

    QString statusStr;
    QColor statusColor;

    if (booking->getStatus() == BookingStatus::Issued) {
        statusStr = "Đang giữ chỗ";
        statusColor = QColor("#27C93F");
    } else if (booking->getStatus() == BookingStatus::Cancelled) {
        statusStr = "Đã hủy";
        statusColor = QColor("#FF5F57");
    } else {
        statusStr = "Đã đổi";
        statusColor = QColor("#FFBD2E");
    }
    rowItems << new QStandardItem(statusStr);
    rowItems.last()->setForeground(statusColor);

    rowItems << new QStandardItem(QString::fromStdString(booking->getAgentId()));
    
    for (QStandardItem *item : rowItems) {
        item->setTextAlignment(Qt::AlignCenter);
    }

    QString cancelDeadline = QString::fromStdString(
        bookingManager_->getCancellationDeadline(booking->getBookingId(), *flightManager_));
    QString changeDeadline = QString::fromStdString(
        bookingManager_->getChangeDeadline(booking->getBookingId(), *flightManager_));
    QString tip = QString("Hạn hủy: %1\nHạn đổi: %2").arg(cancelDeadline, changeDeadline);
    for (QStandardItem *item : rowItems) item->setToolTip(tip);

    model_->appendRow(rowItems);
}

// ========== SEARCH FUNCTIONS ==========
void AdminBookingsPage::onSearchByBookingId()
{
    QString input = bookingIdSearchEdit_->text().trimmed();
    
    if (input.isEmpty()) {
        statusLabel_->setText("Vui lòng nhập mã đặt chỗ cần tìm!");
        statusLabel_->setStyleSheet("color: #FF5F57;");
        return;
    }
    
    std::string bookingId = input.toStdString();
    
    Booking* booking = bookingManager_->findBookingById(bookingId);
    
    model_->removeRows(0, model_->rowCount());
    
    if (!booking) {
        statusLabel_->setText("Không tìm thấy vé với mã " + input + "!");
        statusLabel_->setStyleSheet("color: #FF5F57;");
        return;
    }
    
    displayBooking(booking);

    statusLabel_->setText("Tìm thấy 1 vé với mã " + input + "!");
    statusLabel_->setStyleSheet("color: #27C93F;");
}


void AdminBookingsPage::onSearchByPassengerId()
{
    QString input = passengerIdSearchEdit_->text().trimmed();
    
    if (input.isEmpty()) {
        statusLabel_->setText("Vui lòng nhập CCCD khách hàng cần tìm!");
        statusLabel_->setStyleSheet("color: #FF5F57;");
        return;
    }
    
    std::string passengerId = input.toStdString();
    
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        std::cout << "Không thể xác định người dùng. Vui lòng đăng nhập lại." << std::endl;
        return;
    }
    std::string currentAgentId = currentUser->getId();
    
    std::vector<Booking*> results = bookingManager_->findBookingsByPassengerId(passengerId);
    
    model_->removeRows(0, model_->rowCount());
    
    if (results.empty()) {
        statusLabel_->setText("Không tìm thấy vé với CCCD " + input + "!");
        statusLabel_->setStyleSheet("color: #FF5F57;");
        return;
    }
    
    for (Booking* booking : results) {
        displayBooking(booking);
    }

    statusLabel_->setText(
        QString("Tìm thấy %1 vé với CCCD %2").arg(results.size()).arg(input)
    );
    statusLabel_->setStyleSheet("color: #27C93F;");
}

void AdminBookingsPage::refreshPage() {
    PageRefresher::clearSearchFields(this);
    PageRefresher::executeRefresh([this]() {
        refreshTable();
    });
}
