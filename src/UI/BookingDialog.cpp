#include "BookingDialog.h"
#include "entities/FlightInstance.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/AccountManager.h"
#include "core/SeatManager.h"
#include "entities/Seat.h"
#include "entities/Account.h"
#include "utils/DateTime.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QDateTime>
#include <QRegularExpression>

// Seat button style constants
namespace {
    const QString BUSINESS_AVAILABLE_STYLE = 
        "QPushButton { background: #FFD700; color: #000; border: 2px solid #DAA520; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #FFC700; }";
    
    const QString ECONOMY_AVAILABLE_STYLE = 
        "QPushButton { background: #90EE90; color: #000; border: 2px solid #32CD32; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #7FD87F; }";
    
    const QString BOOKED_STYLE = 
        "QPushButton { background: #D3D3D3; color: #666; border: 2px solid #A9A9A9; border-radius: 5px; }";
    
    const QString LOCKED_STYLE = 
        "QPushButton { background: #FF6B6B; color: #FFF; border: 2px solid #C92A2A; border-radius: 5px; }";
    
    const QString SELECTED_BORDER = "3px solid #FF0000";
    const QString NORMAL_BORDER = "2px solid";
}

BookingDialog::BookingDialog(FlightInstance* flightInstance, 
                             FlightManager* flightManager,
                             BookingManager* bookingManager,
                             AccountManager* accountManager,
                             QWidget *parent)
    : QDialog(parent),
      flightInstance_(flightInstance),
      flightManager_(flightManager),
      bookingManager_(bookingManager),
      accountManager_(accountManager),
      seatMapContainer_(nullptr),
      seatMapLayout_(nullptr),
      selectedSeatId_("")
{
    Q_ASSERT(flightInstance_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(accountManager_ != nullptr);
    
    setWindowTitle("Đặt vé máy bay");
    setMinimumWidth(700);
    setMinimumHeight(600);
    
    setupUi();
}

void BookingDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // === SCROLLABLE CONTENT AREA ===
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    
    // Tiêu đề
    auto *titleLabel = new QLabel("Thông tin đặt vé", contentWidget);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: 700; color: #123B7A;");
    titleLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titleLabel);
    
    // Thông tin chuyến bay
    auto *flightGroup = new QGroupBox("Thông tin chuyến bay", contentWidget);
    auto *flightLayout = new QVBoxLayout(flightGroup);
    
    QString flightInfo = QString("Mã chuyến: %1\n")
        .arg(QString::fromStdString(flightInstance_->getFlightNumber()));
    
    flightInfo += QString("Khởi hành: %1 %2\n")
        .arg(QString::fromStdString(flightInstance_->getDepartureDate()))
        .arg(QString::fromStdString(flightInstance_->getDepartureTime()));
    
    flightInfo += QString("Đến: %1 %2")
        .arg(QString::fromStdString(flightInstance_->getArrivalDate()))
        .arg(QString::fromStdString(flightInstance_->getArrivalTime()));
    
    auto *flightInfoLabel = new QLabel(flightInfo);
    flightLayout->addWidget(flightInfoLabel);
    contentLayout->addWidget(flightGroup);
    
    // Form thông tin hành khách
    auto *passengerGroup = new QGroupBox("Thông tin hành khách", contentWidget);
    auto *formLayout = new QFormLayout(passengerGroup);
    formLayout->setSpacing(12);
    
    // CCCD/ID (bắt buộc)
    passengerIdEdit_ = new QLineEdit(contentWidget);
    passengerIdEdit_->setPlaceholderText("Nhập CMND/CCCD");
    formLayout->addRow("CMND/CCCD: *", passengerIdEdit_);
    
    // Họ tên (tùy chọn)
    passengerNameEdit_ = new QLineEdit(contentWidget);
    passengerNameEdit_->setPlaceholderText("Nhập họ tên đầy đủ");
    formLayout->addRow("Họ và tên:", passengerNameEdit_);
    
    // Số điện thoại (tùy chọn)
    passengerPhoneEdit_ = new QLineEdit(contentWidget);
    passengerPhoneEdit_->setPlaceholderText("Nhập số điện thoại");
    formLayout->addRow("Số điện thoại:", passengerPhoneEdit_);
    
    // Số hộ chiếu (tùy chọn)
    passportNumberEdit_ = new QLineEdit(contentWidget);
    passportNumberEdit_->setPlaceholderText("Nhập số hộ chiếu (nếu có)");
    formLayout->addRow("Số hộ chiếu:", passportNumberEdit_);
    
    contentLayout->addWidget(passengerGroup);
    
    // Chọn hạng vé
    auto *classGroup = new QGroupBox("Chọn hạng vé", contentWidget);
    auto *classLayout = new QVBoxLayout(classGroup);
    
    classComboBox_ = new QComboBox(contentWidget);
    classComboBox_->addItem("Hạng phổ thông (Economy)", static_cast<int>(BookingClass::Economy));
    classComboBox_->addItem("Hạng thương gia (Business)", static_cast<int>(BookingClass::Business));
    classLayout->addWidget(classComboBox_);
    
    // Hiển thị giá vé
    fareLabel_ = new QLabel(contentWidget);
    fareLabel_->setStyleSheet("font-weight: 600; color: #2E7D32; font-size: 14px;");
    classLayout->addWidget(fareLabel_);
    
    // Cập nhật giá khi thay đổi hạng vé
    connect(classComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        BookingClass selectedClass = static_cast<BookingClass>(classComboBox_->itemData(index).toInt());
        int fare = (selectedClass == BookingClass::Economy) 
                   ? flightInstance_->getFareEconomy() 
                   : flightInstance_->getFareBusiness();
        
        // Format with thousand separators
        QString fareStr = QString::number(fare);
        int pos = fareStr.length() - 3;
        while (pos > 0) {
            fareStr.insert(pos, ',');
            pos -= 3;
        }
        fareLabel_->setText(QString("Giá vé: %1 VND").arg(fareStr));
    });
    
    // Trigger initial fare display
    classComboBox_->setCurrentIndex(0);
    
    contentLayout->addWidget(classGroup);
    
    // Sơ đồ ghế (NO separate scroll area)
    auto *seatGroup = new QGroupBox("Chọn ghế ngồi", contentWidget);
    auto *seatVLayout = new QVBoxLayout(seatGroup);
    
    // Thông tin hướng dẫn
    auto *seatInfoLabel = new QLabel("Vui lòng chọn ghế ngồi:", contentWidget);
    seatInfoLabel->setStyleSheet("font-size: 12px; color: #666;");
    seatVLayout->addWidget(seatInfoLabel);
    
    // Container cho sơ đồ ghế WITHOUT scroll
    seatMapContainer_ = new QWidget(contentWidget);
    seatMapLayout_ = new QGridLayout(seatMapContainer_);
    seatMapLayout_->setSpacing(5);
    seatMapContainer_->setLayout(seatMapLayout_);
    
    seatVLayout->addWidget(seatMapContainer_);
    
    // Render sơ đồ ghế
    renderSeatMap();
    
    contentLayout->addWidget(seatGroup);
    contentLayout->addStretch();
    
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
    
    // === FIXED BUTTONS AT BOTTOM ===
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(20, 10, 20, 20);
    buttonLayout->addStretch();
    
    auto *confirmButton = new QPushButton("Xác nhận đặt vé", this);
    confirmButton->setStyleSheet(
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:8px; padding:10px 30px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }"
    );
    confirmButton->setMinimumWidth(140);
    
    auto *cancelButton = new QPushButton("Hủy", this);
    cancelButton->setStyleSheet(
        "QPushButton { background:#999; color:white; border:none; "
        "border-radius:8px; padding:10px 30px; font-weight:600; }"
        "QPushButton:hover { background:#777; }"
    );
    cancelButton->setMinimumWidth(100);
    
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect buttons
    connect(confirmButton, &QPushButton::clicked, this, [this]() {
        // Validate required fields
        if (passengerIdEdit_->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập CMND/CCCD.");
            passengerIdEdit_->setFocus();
            return;
        }
        
        // Validate passenger ID format (9 or 12 digits for Vietnam ID/CCCD)
        QString passengerId = passengerIdEdit_->text().trimmed();
        QRegularExpression idRegex(R"(^\d{9}$|^\d{12}$)");
        if (!idRegex.match(passengerId).hasMatch()) {
            QMessageBox::warning(this, "Định dạng không hợp lệ", 
                "CMND/CCCD phải là 9 hoặc 12 chữ số.");
            passengerIdEdit_->setFocus();
            return;
        }
        
        // Validate seat selection
        if (selectedSeatId_.isEmpty()) {
            QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng chọn ghế ngồi.");
            return;
        }
        
        // Get seat manager
        SeatManager* seatManager = flightManager_->getSeatManager();
        if (!seatManager) {
            QMessageBox::critical(this, "Lỗi", "Không thể truy cập SeatManager.");
            return;
        }
        
        // FIRST: Try to book the seat - this is the critical operation
        if (!seatManager->bookSeat(selectedSeatId_.toStdString())) {
            QMessageBox::critical(this, "Lỗi", 
                "Không thể đặt ghế. Ghế có thể đã được đặt bởi người khác.");
            return;
        }
        
        // Get current user (agent) ID safely
        Account* currentUser = accountManager_->getCurrentUser();
        if (!currentUser) {
            seatManager->releaseSeat(selectedSeatId_.toStdString()); // Rollback
            QMessageBox::critical(this, "Lỗi", "Không xác định được người dùng.");
            return;
        }
        std::string agentId = currentUser->getId();
        
        // Get current booking class
        BookingClass bookingClass = static_cast<BookingClass>(classComboBox_->currentData().toInt());
        
        // Calculate fare
        int baseFare = (bookingClass == BookingClass::Economy) 
                       ? flightInstance_->getFareEconomy() 
                       : flightInstance_->getFareBusiness();
        
        // Get current date and time in correct format
        QString currentDateTime = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
        
        // Create new booking
        Booking* newBooking = new Booking(
            flightInstance_->getInstanceId(),
            agentId, // Fixed: use real agent ID
            passengerIdEdit_->text().trimmed().toStdString(),
            selectedSeatId_.toStdString(),
            currentDateTime.toStdString(), // Fixed: correct date format
            bookingClass,
            baseFare,
            BookingStatus::Issued
        );
        
        // SECOND: Save to file immediately (after seat is successfully booked)
        if (!bookingManager_->saveBookingToFile(newBooking)) {
            QMessageBox::critical(this, "Lỗi", 
                "Không thể lưu thông tin đặt vé. Vui lòng thử lại.");
            delete newBooking;
            // Rollback: Release the seat since booking save failed
            seatManager->releaseSeat(selectedSeatId_.toStdString());
            return;
        }
        
        // THIRD: Save seat changes to file
        if (!seatManager->saveChanges()) {
            QMessageBox::warning(this, "Cảnh báo",
                "Đặt vé thành công nhưng không cập nhật được sơ đồ ghế. "
                "Tình trạng ghế có thể không được cập nhật ngay trong hệ thống.");
            // Don't rollback booking - it's already saved
        }
        
        // Show success message with booking ID
        QMessageBox::information(this, "Thành công",
            QString("Đặt vé thành công!\n\nMã đặt chỗ: %1\nGhế: %2\nGiá vé: %3 VND")
            .arg(QString::fromStdString(newBooking->getBookingId()))
            .arg(selectedSeatId_)
            .arg(baseFare));
        
        accept();
    });
    
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString BookingDialog::getPassengerId() const
{
    return passengerIdEdit_->text().trimmed();
}

QString BookingDialog::getPassengerName() const
{
    return passengerNameEdit_->text().trimmed();
}

QString BookingDialog::getPassengerPhone() const
{
    return passengerPhoneEdit_->text().trimmed();
}

QString BookingDialog::getPassportNumber() const
{
    return passportNumberEdit_->text().trimmed();
}

BookingClass BookingDialog::getSelectedClass() const
{
    return static_cast<BookingClass>(classComboBox_->currentData().toInt());
}

QString BookingDialog::getSelectedSeatId() const
{
    return selectedSeatId_;
}

void BookingDialog::renderSeatMap()
{
    if (!seatMapLayout_ || !flightManager_ || !flightInstance_) {
        return;
    }

    // Clear existing seat buttons
    QLayoutItem* item;
    while ((item = seatMapLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Load seat map
    SeatManager* seatManager = flightManager_->getSeatManager();
    if (!seatManager) {
        return;
    }

    if (!seatManager->loadSeatMapFor(flightInstance_)) {
        QLabel* errorLabel = new QLabel("Không thể tải sơ đồ ghế");
        errorLabel->setStyleSheet("color: red;");
        seatMapLayout_->addWidget(errorLabel, 0, 0);
        return;
    }

    std::vector<Seat*>* seatMap = seatManager->getActiveSeatMap();
    if (!seatMap) {
        return;
    }

    int cols = seatManager->getSeatColumns();
    
    // Create a centered layout for the seat grid
    QHBoxLayout* centerLayout = new QHBoxLayout();
    centerLayout->addStretch(1); // Left padding for centering
    
    // Create a widget to hold the seat grid
    QWidget* seatGridWidget = new QWidget();
    QGridLayout* seatGridLayout = new QGridLayout(seatGridWidget);
    seatGridLayout->setSpacing(5);
    
    // Add column headers (A, B, C, D, E, F)
    for (int col = 0; col < cols; ++col) {
        QLabel* header = new QLabel(QString(QChar('A' + col)));
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet("font-weight: bold; padding: 5px;");
        seatGridLayout->addWidget(header, 0, col + 1);
    }

    // Render seats
    int currentRow = -1;
    int layoutRow = 1;
    
    for (int i = 0; i < seatMap->size(); ++i) {
        Seat* seat = (*seatMap)[i];
        if (!seat) continue;

        int row, col;
        std::tie(row, col) = seat->getCoordinates();
        
        // Add row header if new row (closer to seats with reduced width)
        if (row != currentRow) {
            currentRow = row;
            QLabel* rowLabel = new QLabel(QString::number(row));
            rowLabel->setFixedWidth(25); // Smaller width = closer to seats
            rowLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            rowLabel->setStyleSheet("font-weight: bold;");
            seatGridLayout->addWidget(rowLabel, layoutRow, 0);
        }

        // Create seat button
        QPushButton* seatBtn = new QPushButton(QString::fromStdString(seat->getId()));
        seatBtn->setFixedSize(50, 40);
        
        // Style based on status and type
        QString btnStyle;
        bool isClickable = false;
        
        if (seat->getStatus() == SeatStatus::Available) {
            isClickable = true;
            if (seat->getType() == SeatType::Business) {
                btnStyle = BUSINESS_AVAILABLE_STYLE;
            } else {
                btnStyle = ECONOMY_AVAILABLE_STYLE;
            }
        } else if (seat->getStatus() == SeatStatus::Booked) {
            btnStyle = BOOKED_STYLE;
        } else {
            btnStyle = LOCKED_STYLE;
        }
        
        seatBtn->setStyleSheet(btnStyle);
        seatBtn->setEnabled(isClickable);
        seatBtn->setProperty("originalStyle", btnStyle);
        seatBtn->setProperty("seatId", QString::fromStdString(seat->getId()));
        
        // Connect click event for available seats
        if (isClickable) {
            connect(seatBtn, &QPushButton::clicked, this, [this, seatGridLayout, seatBtn]() {
                QString seatId = seatBtn->property("seatId").toString();
                
                // Deselect previous seat
                if (!selectedSeatId_.isEmpty()) {
                    // Find and restore previous button style
                    for (int i = 0; i < seatGridLayout->count(); ++i) {
                        QLayoutItem* layoutItem = seatGridLayout->itemAt(i);
                        if (!layoutItem) continue;
                        
                        QPushButton* btn = qobject_cast<QPushButton*>(layoutItem->widget());
                        if (btn && btn->property("seatId").toString() == selectedSeatId_) {
                            // Restore original style
                            QString originalStyle = btn->property("originalStyle").toString();
                            btn->setStyleSheet(originalStyle);
                            break;
                        }
                    }
                }
                
                // Select new seat
                selectedSeatId_ = seatId;
                
                // Highlight selected seat
                QString originalStyle = seatBtn->property("originalStyle").toString();
                QString selectedStyle = originalStyle;
                selectedStyle.replace(NORMAL_BORDER, SELECTED_BORDER);
                seatBtn->setStyleSheet(selectedStyle);
            });
        }
        
        seatGridLayout->addWidget(seatBtn, layoutRow, col + 1);
        
        // Move to next row if we've filled all columns
        if (col == cols - 1) {
            layoutRow++;
        }
    }
    
    // Add legend to seat grid
    layoutRow++;
    auto *legendLabel = new QLabel("Chú thích:");
    legendLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    seatGridLayout->addWidget(legendLabel, layoutRow, 0, 1, cols + 1);
    
    layoutRow++;
    auto *legendWidget = new QWidget();
    auto *legendLayout = new QHBoxLayout(legendWidget);
    legendLayout->setContentsMargins(0, 0, 0, 0);
    
    auto *businessLegend = new QLabel("■ Hạng thương gia");
    businessLegend->setStyleSheet("color: #DAA520; font-weight: bold;");
    legendLayout->addWidget(businessLegend);
    
    auto *economyLegend = new QLabel("■ Hạng phổ thông");
    economyLegend->setStyleSheet("color: #32CD32; font-weight: bold;");
    legendLayout->addWidget(economyLegend);
    
    auto *bookedLegend = new QLabel("■ Đã đặt");
    bookedLegend->setStyleSheet("color: #A9A9A9; font-weight: bold;");
    legendLayout->addWidget(bookedLegend);
    
    legendLayout->addStretch();
    seatGridLayout->addWidget(legendWidget, layoutRow, 0, 1, cols + 1);
    
    // Add the seat grid to center layout
    centerLayout->addWidget(seatGridWidget);
    centerLayout->addStretch(1); // Right padding for centering
    
    // Add center layout to main seat map layout
    seatMapLayout_->addLayout(centerLayout, 0, 0);
}
