#include "ChangeBookingDialog.h"
#include "entities/Flight.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/SeatManager.h"
#include "entities/Seat.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QScreen> // Required for QScreen
#include <QButtonGroup>

// Seat button style constants (matching BookingDialog)
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

ChangeBookingDialog::ChangeBookingDialog(Booking* currentBooking,
                                         BookingManager* bookingManager,
                                         FlightManager* flightManager,
                                         QWidget *parent)
    : QDialog(parent),
      currentBooking_(currentBooking),
      bookingManager_(bookingManager),
      flightManager_(flightManager),
      currentFlight_(nullptr),
      selectedNewFlight_(nullptr),
      seatMapContainer_(nullptr),
      seatMapLayout_(nullptr),
      selectedSeatId_("")
{
    Q_ASSERT(currentBooking_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    
    setWindowTitle("Thông tin đổi vé");
    setMinimumWidth(700);
    setMinimumHeight(600);

    // Giới hạn chiều cao tối đa = 90% màn hình (để không che taskbar)
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    int maxHeight = static_cast<int>(screenGeometry.height() * 0.9);
    int maxWidth = static_cast<int>(screenGeometry.width() * 0.8);

    setMaximumHeight(maxHeight);
    setMaximumWidth(maxWidth);

    // Center dialog on screen
    resize(700, qMin(700, maxHeight));
    move(screenGeometry.center() - rect().center());
    
    // Load current flight instance
    currentFlight_ = flightManager_->findFlightById(currentBooking_->getFlightId());
    
    setupUi();
}

void ChangeBookingDialog::setupUi()
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
    contentLayout->setSpacing(15);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    
    // Tiêu đề
    auto *titleLabel = new QLabel("THÔNG TIN ĐỔI VÉ", contentWidget);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #123B7A;");
    titleLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titleLabel);
    
    // === THÔNG TIN VÉ HIỆN TẠI ===
    auto *currentGroup = new QGroupBox("Thông tin vé hiện tại", contentWidget);
    auto *currentLayout = new QVBoxLayout(currentGroup);
    
    QString currentInfo = QString("Mã đặt chỗ: %1\n"
                                 "Chuyến bay: %2\n"
                                 "Ghế: %3 (%4)\n"
                                 "Giá vé: %4 VND")
        .arg(QString::fromStdString(currentBooking_->getBookingId()))
        .arg(currentFlight_ ? QString::fromStdString(currentFlight_->getFlightNumber()) : "N/A")
        .arg(QString::fromStdString(currentBooking_->getSeatID()))
        .arg(currentBooking_->getBaseFare());
    
    auto *currentInfoLabel = new QLabel(currentInfo);
    currentLayout->addWidget(currentInfoLabel);
    contentLayout->addWidget(currentGroup);
    
    // === CHUYẾN BAY MỚI ===
    auto *newFlightGroup = new QGroupBox("Chuyến bay mới", contentWidget);
    auto *newFlightLayout = new QVBoxLayout(newFlightGroup);
    
    // Mã chuyến bay + nút xem thông tin trên cùng 1 hàng
    auto *flightIdLayout = new QHBoxLayout();
    flightIdLayout->addWidget(new QLabel("Mã chuyến bay:"));
    newFlightIdEdit_ = new QLineEdit(contentWidget);
    newFlightIdEdit_->setPlaceholderText("Nhập mã chuyến bay (VD: FI-110)");
    flightIdLayout->addWidget(newFlightIdEdit_, 1);
    
    auto *viewFlightBtn = new QPushButton("Xem thông tin", contentWidget);
    viewFlightBtn->setStyleSheet("background:#4478BD; color:white; padding:8px 15px; border-radius:4px;");
    flightIdLayout->addWidget(viewFlightBtn);
    newFlightLayout->addLayout(flightIdLayout);
    
    // Hiển thị thông tin chuyến bay
    flightInfoText_ = new QTextEdit(contentWidget);
    flightInfoText_->setReadOnly(true);
    flightInfoText_->setMaximumHeight(120);
    flightInfoText_->setPlaceholderText("Thông tin chuyến bay sẽ hiển thị ở đây...");
    newFlightLayout->addWidget(flightInfoText_);
    
    contentLayout->addWidget(newFlightGroup);
    
    // === CHỌN HẠNG VÉ ===
    auto *classGroup = new QGroupBox("Chọn hạng vé", contentWidget);
    auto *classLayout = new QVBoxLayout(classGroup); // ← Đổi từ HBoxLayout → VBoxLayout

    economyRadio_ = new QRadioButton("Hạng phổ thông (Economy)", contentWidget);
    businessRadio_ = new QRadioButton("Hạng thương gia (Business)", contentWidget);

    // Set default based on current booking
    if (currentBooking_->getClass() == BookingClass::Economy) {
        economyRadio_->setChecked(true);
    } else {
        businessRadio_->setChecked(true);
    }

    // Group radio buttons
    auto *classButtonGroup = new QButtonGroup(contentWidget);
    classButtonGroup->addButton(economyRadio_);
    classButtonGroup->addButton(businessRadio_);

    classLayout->addWidget(economyRadio_);
    classLayout->addWidget(businessRadio_);

    // Hiển thị giá vé
    classFareLabel_ = new QLabel(contentWidget); // ← Cần thêm member variable
    classFareLabel_->setStyleSheet("font-weight: 600; color: #2E7D32; font-size: 14px; margin-top: 5px;");
    classLayout->addWidget(classFareLabel_);
    
    contentLayout->addWidget(classGroup);
    
    // === SƠ ĐỒ GHẾ ===
    auto *seatGroup = new QGroupBox("Chọn ghế", contentWidget);
    auto *seatVLayout = new QVBoxLayout(seatGroup);
    
    auto *seatInfoLabel = new QLabel("Vui lòng chọn ghế ngồi:", contentWidget);
    seatInfoLabel->setStyleSheet("font-size: 12px; color: #666;");
    seatVLayout->addWidget(seatInfoLabel);
    
    // Container cho sơ đồ ghế (inline, không tạo dialog riêng)
    seatMapContainer_ = new QWidget(contentWidget);
    seatMapLayout_ = new QGridLayout(seatMapContainer_);
    seatMapLayout_->setSpacing(5);
    seatMapContainer_->setLayout(seatMapLayout_);
    
    seatVLayout->addWidget(seatMapContainer_);
    
    contentLayout->addWidget(seatGroup);
    
    // === GIÁ CHÊNH LỆCH ===
    priceDiffLabel_ = new QLabel("", contentWidget);
    priceDiffLabel_->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    priceDiffLabel_->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(priceDiffLabel_);
    
    contentLayout->addStretch();
    
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
    
    // === BUTTONS ===
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(20, 10, 20, 20);
    buttonLayout->addStretch();
    
    confirmBtn_ = new QPushButton("Xác nhận đổi vé", this);
    confirmBtn_->setStyleSheet("background:#5886C0; color:white; padding:10px 30px; border-radius:8px; font-weight:bold;");
    confirmBtn_->setEnabled(false); // Disable until flight and seat are selected
    
    auto *cancelBtn = new QPushButton("Hủy", this);
    cancelBtn->setStyleSheet("background:#999; color:white; padding:10px 30px; border-radius:8px;");
    
    buttonLayout->addWidget(confirmBtn_);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    // === CONNECTIONS ===
    connect(viewFlightBtn, &QPushButton::clicked, this, &ChangeBookingDialog::onViewFlightInfoClicked);
    connect(economyRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
    connect(businessRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
    connect(confirmBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onConfirmClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ChangeBookingDialog::onViewFlightInfoClicked()
{
    QString flightId = newFlightIdEdit_->text().trimmed();
    if (flightId.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập mã chuyến bay.");
        return;
    }
    
    selectedNewFlight_ = flightManager_->findFlightById(flightId.toStdString());
    if (!selectedNewFlight_) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy chuyến bay.");
        flightInfoText_->clear();
        selectedSeatId_ = "";
        confirmBtn_->setEnabled(false);
        return;
    }
    
    // Display flight info
    QString info = QString("Số hiệu: %1\n"
                          "Ngày khởi hành: %2 %3\n"
                          "Giá Economy: %4 VND\n"
                          "Giá Business: %5 VND")
        .arg(QString::fromStdString(selectedNewFlight_->getFlightNumber()))
        .arg(QString::fromStdString(selectedNewFlight_->getDepartureDate()))
        .arg(QString::fromStdString(selectedNewFlight_->getDepartureTime()))
        .arg(selectedNewFlight_->getFareEconomy())
        .arg(selectedNewFlight_->getFareBusiness());
    
    flightInfoText_->setText(info);
    
    // Render seat map
    renderSeatMap();
    
    // Update price difference
    updatePriceDifference();
}

void ChangeBookingDialog::onClassChanged()
{
    // Re-render seat map when class changes
    if (selectedNewFlight_) {
        renderSeatMap();
        updatePriceDifference();
    }
}

void ChangeBookingDialog::renderSeatMap()
{
    if (!selectedNewFlight_) return;
    
    // Clear existing seat map
    QLayoutItem* item;
    while ((item = seatMapLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    selectedSeatId_ = "";
    confirmBtn_->setEnabled(false);
    
    // Load seat map
    SeatManager* seatManager = flightManager_->getSeatManager();
    if (!seatManager || !seatManager->loadSeatMapFor(selectedNewFlight_)) {
        QMessageBox::warning(this, "Lỗi", "Không thể tải sơ đồ ghế.");
        return;
    }
    
    std::vector<Seat*>* seatMap = seatManager->getActiveSeatMap();
    if (!seatMap) return;
    
    int cols = seatManager->getSeatColumns();
    
    // Get selected class
    BookingClass selectedClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;
    
    // Add column headers
    for (int col = 0; col < cols; ++col) {
        QLabel* header = new QLabel(QString(QChar('A' + col)));
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet("font-weight: bold; padding: 5px;");
        seatMapLayout_->addWidget(header, 0, col + 1);
    }
    
    // Render seats
    int currentRow = -1;
    int layoutRow = 1;
    
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
            seatMapLayout_->addWidget(rowLabel, layoutRow, 0);
        }
        
        QPushButton* seatBtn = new QPushButton(QString::fromStdString(seat->getId()));
        seatBtn->setFixedSize(50, 40);
        
        QString btnStyle;
        bool isClickable = false;
        
        // Check if seat matches selected class and is available
        bool matchesClass = (selectedClass == BookingClass::Economy && seat->getType() == SeatType::Economy) ||
                           (selectedClass == BookingClass::Business && seat->getType() == SeatType::Business);
        
        if (seat->getStatus() == SeatStatus::Available && matchesClass) {
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
        
        if (isClickable) {
            connect(seatBtn, &QPushButton::clicked, [this, seatBtn]() {
                QString seatId = seatBtn->property("seatId").toString();
                
                // Deselect previous
                if (!selectedSeatId_.isEmpty()) {
                    for (int i = 0; i < seatMapLayout_->count(); ++i) {
                        QLayoutItem* layoutItem = seatMapLayout_->itemAt(i);
                        if (!layoutItem) continue;
                        
                        QPushButton* btn = qobject_cast<QPushButton*>(layoutItem->widget());
                        if (btn && btn->property("seatId").toString() == selectedSeatId_) {
                            QString originalStyle = btn->property("originalStyle").toString();
                            btn->setStyleSheet(originalStyle);
                            break;
                        }
                    }
                }
                
                // Select new
                selectedSeatId_ = seatId;
                QString originalStyle = seatBtn->property("originalStyle").toString();
                QString selectedStyle = originalStyle;
                selectedStyle.replace(NORMAL_BORDER, SELECTED_BORDER);
                seatBtn->setStyleSheet(selectedStyle);
                
                // Enable confirm button
                confirmBtn_->setEnabled(true);
            });
        }
        
        seatMapLayout_->addWidget(seatBtn, layoutRow, col + 1);
        
        if (col == cols - 1) {
            layoutRow++;
        }
    }
}

void ChangeBookingDialog::updatePriceDifference()
{
    if (!selectedNewFlight_) {
        priceDiffLabel_->clear();
        classFareLabel_->clear();
        return;
    }
    
    BookingClass selectedClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;
    
    int newFare = (selectedClass == BookingClass::Economy) 
                 ? selectedNewFlight_->getFareEconomy() 
                 : selectedNewFlight_->getFareBusiness();
    int oldFare = currentBooking_->getBaseFare();
    int priceDiff = newFare - oldFare;
    
    // Format with thousand separators
    auto formatFare = [](int fare) -> QString {
        QString fareStr = QString::number(fare);
        int pos = fareStr.length() - 3;
        while (pos > 0) {
            fareStr.insert(pos, ',');
            pos -= 3;
        }
        return fareStr;
    };
    
    // Hiển thị giá vé hạng đã chọn
    classFareLabel_->setText(QString("Giá vé: %1 VND").arg(formatFare(newFare)));
    
    // Hiển thị chênh lệch
    if (priceDiff > 0) {
        priceDiffLabel_->setText(QString("⚠️ Phí phát sinh thêm: %1 VND").arg(formatFare(priceDiff)));
        priceDiffLabel_->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px; color: #0000FF;");
    } else if (priceDiff < 0) {
        priceDiffLabel_->setText(QString("✅ Hoàn trả: %1 VND").arg(formatFare(-priceDiff)));
        priceDiffLabel_->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px; color: #FF0000;");
    } else {
        priceDiffLabel_->setText("Không có chênh lệch giá");
        priceDiffLabel_->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px; color: #008000;");
    }
}

void ChangeBookingDialog::onConfirmClicked()
{
    if (!selectedNewFlight_ || selectedSeatId_.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn chuyến bay và ghế.");
        return;
    }

    // ✅ THÊM: Get selected class
    BookingClass selectedClass = economyRadio_->isChecked() 
                                ? BookingClass::Economy 
                                : BookingClass::Business;
    
    // Confirm change
    auto reply = QMessageBox::question(this, "Xác nhận đổi vé",
        QString("Bạn có chắc chắn muốn đổi vé?\n\n"
               "Chuyến bay mới: %1\n"
               "Ghế mới: %2\n\n"
               "⚠️ Hành động này KHÔNG THỂ hoàn tác.")
            .arg(QString::fromStdString(selectedNewFlight_->getFlightNumber()))
            .arg(selectedSeatId_),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::No) {
        return;
    }
    
    // Call changeBooking
    SeatManager* seatManager = flightManager_->getSeatManager();
    bool success = bookingManager_->changeBooking(
        *flightManager_,
        *seatManager,
        currentBooking_->getBookingId(),
        selectedNewFlight_->getFlightId(),
        selectedSeatId_.toStdString(),
        selectedClass
    );
    
    if (success) {
        QMessageBox::information(this, "Thành công",
            QString("Đã đổi vé thành công!\n\n"
                   "Mã đặt chỗ: %1\n"
                   "Chuyến bay mới: %2\n"
                   "Ghế mới: %3")
                .arg(QString::fromStdString(currentBooking_->getBookingId()))
                .arg(QString::fromStdString(selectedNewFlight_->getFlightNumber()))
                .arg(selectedSeatId_));
        
        accept();
    } else {
        QMessageBox::critical(this, "Thất bại",
            "Đổi vé thất bại.\n\n"
            "Có thể do:\n"
            "- Ghế đã được đặt\n"
            "- Lỗi lưu dữ liệu\n"
            "- Vé không hợp lệ\n\n"
            "Vui lòng thử lại hoặc liên hệ hỗ trợ.");
    }
}
