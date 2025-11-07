#include "BookingDialog.h"
#include "entities/FlightInstance.h"
#include "core/FlightManager.h"
#include "core/SeatManager.h"
#include "entities/Seat.h"

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

BookingDialog::BookingDialog(FlightInstance* flightInstance, FlightManager* flightManager, QWidget *parent)
    : QDialog(parent),
      flightInstance_(flightInstance),
      flightManager_(flightManager),
      seatMapContainer_(nullptr),
      seatMapLayout_(nullptr),
      selectedSeatId_("")
{
    Q_ASSERT(flightInstance_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    
    setWindowTitle("Đặt vé máy bay");
    setMinimumWidth(700);
    setMinimumHeight(600);
    
    setupUi();
}

void BookingDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Tiêu đề
    auto *titleLabel = new QLabel("Thông tin đặt vé", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: 700; color: #123B7A;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Thông tin chuyến bay
    auto *flightGroup = new QGroupBox("Thông tin chuyến bay", this);
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
    mainLayout->addWidget(flightGroup);
    
    // Form thông tin hành khách
    auto *passengerGroup = new QGroupBox("Thông tin hành khách", this);
    auto *formLayout = new QFormLayout(passengerGroup);
    formLayout->setSpacing(12);
    
    // CCCD/ID (bắt buộc)
    passengerIdEdit_ = new QLineEdit(this);
    passengerIdEdit_->setPlaceholderText("Nhập CMND/CCCD");
    formLayout->addRow("CMND/CCCD: *", passengerIdEdit_);
    
    // Họ tên (tùy chọn)
    passengerNameEdit_ = new QLineEdit(this);
    passengerNameEdit_->setPlaceholderText("Nhập họ tên đầy đủ");
    formLayout->addRow("Họ và tên:", passengerNameEdit_);
    
    // Số điện thoại (tùy chọn)
    passengerPhoneEdit_ = new QLineEdit(this);
    passengerPhoneEdit_->setPlaceholderText("Nhập số điện thoại");
    formLayout->addRow("Số điện thoại:", passengerPhoneEdit_);
    
    // Số hộ chiếu (tùy chọn)
    passportNumberEdit_ = new QLineEdit(this);
    passportNumberEdit_->setPlaceholderText("Nhập số hộ chiếu (nếu có)");
    formLayout->addRow("Số hộ chiếu:", passportNumberEdit_);
    
    mainLayout->addWidget(passengerGroup);
    
    // Chọn hạng vé
    auto *classGroup = new QGroupBox("Chọn hạng vé", this);
    auto *classLayout = new QVBoxLayout(classGroup);
    
    classComboBox_ = new QComboBox(this);
    classComboBox_->addItem("Hạng phổ thông (Economy)", static_cast<int>(BookingClass::Economy));
    classComboBox_->addItem("Hạng thương gia (Business)", static_cast<int>(BookingClass::Business));
    classLayout->addWidget(classComboBox_);
    
    // Hiển thị giá vé
    fareLabel_ = new QLabel(this);
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
    
    mainLayout->addWidget(classGroup);
    
    // Sơ đồ ghế
    auto *seatGroup = new QGroupBox("Chọn ghế ngồi", this);
    auto *seatVLayout = new QVBoxLayout(seatGroup);
    
    // Thông tin hướng dẫn
    auto *seatInfoLabel = new QLabel("Vui lòng chọn ghế ngồi:", this);
    seatInfoLabel->setStyleSheet("font-size: 12px; color: #666;");
    seatVLayout->addWidget(seatInfoLabel);
    
    // Container cho sơ đồ ghế với scroll
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumHeight(250);
    
    seatMapContainer_ = new QWidget();
    seatMapLayout_ = new QGridLayout(seatMapContainer_);
    seatMapLayout_->setSpacing(5);
    seatMapContainer_->setLayout(seatMapLayout_);
    
    scrollArea->setWidget(seatMapContainer_);
    seatVLayout->addWidget(scrollArea);
    
    // Render sơ đồ ghế
    renderSeatMap();
    
    mainLayout->addWidget(seatGroup);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
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
        
        // Validate seat selection
        if (selectedSeatId_.isEmpty()) {
            QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng chọn ghế ngồi.");
            return;
        }
        
        // Book the seat
        SeatManager* seatManager = flightManager_->getSeatManager();
        if (seatManager && seatManager->bookSeat(selectedSeatId_.toStdString())) {
            seatManager->saveChanges();
            accept();
        } else {
            QMessageBox::warning(this, "Lỗi", "Không thể đặt ghế. Vui lòng thử lại.");
        }
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
    
    // Add column headers (A, B, C, D, E, F)
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
        
        // Add row header if new row
        if (row != currentRow) {
            currentRow = row;
            QLabel* rowLabel = new QLabel(QString::number(row));
            rowLabel->setAlignment(Qt::AlignCenter);
            rowLabel->setStyleSheet("font-weight: bold; padding: 5px;");
            seatMapLayout_->addWidget(rowLabel, layoutRow, 0);
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
            connect(seatBtn, &QPushButton::clicked, this, [this, seatBtn]() {
                QString seatId = seatBtn->property("seatId").toString();
                
                // Deselect previous seat
                if (!selectedSeatId_.isEmpty()) {
                    // Find and restore previous button style
                    for (int i = 0; i < seatMapLayout_->count(); ++i) {
                        QLayoutItem* layoutItem = seatMapLayout_->itemAt(i);
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
        
        seatMapLayout_->addWidget(seatBtn, layoutRow, col + 1);
        
        // Move to next row if we've filled all columns
        if (col == cols - 1) {
            layoutRow++;
        }
    }
    
    // Add legend
    layoutRow++;
    auto *legendLabel = new QLabel("Chú thích:", this);
    legendLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    seatMapLayout_->addWidget(legendLabel, layoutRow, 0, 1, cols + 1);
    
    layoutRow++;
    auto *legendLayout = new QHBoxLayout();
    
    auto *businessLegend = new QLabel("■ Hạng thương gia", this);
    businessLegend->setStyleSheet("color: #DAA520; font-weight: bold;");
    legendLayout->addWidget(businessLegend);
    
    auto *economyLegend = new QLabel("■ Hạng phổ thông", this);
    economyLegend->setStyleSheet("color: #32CD32; font-weight: bold;");
    legendLayout->addWidget(economyLegend);
    
    auto *bookedLegend = new QLabel("■ Đã đặt", this);
    bookedLegend->setStyleSheet("color: #A9A9A9; font-weight: bold;");
    legendLayout->addWidget(bookedLegend);
    
    legendLayout->addStretch();
    seatMapLayout_->addLayout(legendLayout, layoutRow, 0, 1, cols + 1);
}
