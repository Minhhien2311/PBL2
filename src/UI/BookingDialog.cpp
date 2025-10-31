#include "BookingDialog.h"
#include "entities/FlightInstance.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>

BookingDialog::BookingDialog(FlightInstance* flightInstance, QWidget *parent)
    : QDialog(parent),
      flightInstance_(flightInstance)
{
    Q_ASSERT(flightInstance_ != nullptr);
    
    setWindowTitle("Đặt vé máy bay");
    setMinimumWidth(500);
    
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
