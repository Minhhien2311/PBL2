#include "BookingDetailsDialog.h"
#include "entities/Booking.h"
#include "core/FlightManager.h"
#include "core/AccountManager.h"
#include "core/PassengerManager.h"  // Include PassengerManager
#include "entities/Flight.h"
#include "entities/Route.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFrame>

BookingDetailsDialog::BookingDetailsDialog(Booking* booking, 
                                           FlightManager* flightManager,
                                           AccountManager* accountManager,
                                           PassengerManager* passengerManager,
                                           QWidget *parent)
    : QDialog(parent),
      booking_(booking),
      flightManager_(flightManager),
      accountManager_(accountManager),
      passengerManager_(passengerManager)
{
    Q_ASSERT(booking_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(passengerManager_ != nullptr);
    
    setWindowTitle("Chi tiết thông tin đặt chỗ");
    setMinimumWidth(500);
    
    setupUi();
}

void BookingDetailsDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Tiêu đề
    auto *titleLabel = new QLabel("Chi tiết thông tin đặt chỗ", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: 700; color: #123B7A; margin-bottom: 16px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Thông tin Đặt chỗ
    auto *bookingGroup = createBookingInfoGroup();
    mainLayout->addWidget(bookingGroup);
    
    // Đường phân cách
    mainLayout->addWidget(createSeparator());
    
    // Thông tin Chuyến bay
    auto *flightGroup = createFlightInfoGroup();
    mainLayout->addWidget(flightGroup);
    
    // Đường phân cách
    mainLayout->addWidget(createSeparator());
    
    // Thông tin Hành khách
    auto *passengerGroup = createPassengerInfoGroup();
    mainLayout->addWidget(passengerGroup);
    
    // Nút đóng
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    auto *closeButton = new QPushButton("Đóng", this);
    closeButton->setStyleSheet(
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:8px; padding:10px 30px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }"
    );
    closeButton->setMinimumWidth(120);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
}

QGroupBox* BookingDetailsDialog::createBookingInfoGroup()
{
    auto *group = new QGroupBox("Thông tin đặt chỗ", this);
    group->setStyleSheet("QGroupBox { font-weight: bold; color: #123B7A; }");
    
    auto *layout = new QGridLayout(group);
    layout->setVerticalSpacing(8);
    layout->setHorizontalSpacing(20);
    
    int row = 0;
    
    // Mã đặt chỗ
    layout->addWidget(new QLabel("Mã đặt chỗ", this), row, 0);
    bookingIdLabel_ = new QLabel(QString::fromStdString(booking_->getBookingId()));
    bookingIdLabel_->setStyleSheet("font-weight: 600;");
    layout->addWidget(bookingIdLabel_, row++, 1);
    
    // Thời gian đặt
    layout->addWidget(new QLabel("Thời gian đặt", this), row, 0);
    bookingDateLabel_ = new QLabel(QString::fromStdString(booking_->getBookingDate()));
    layout->addWidget(bookingDateLabel_, row++, 1);
    
    // Hạng vé
    layout->addWidget(new QLabel("Hạng vé", this), row, 0);
    QString classStr = (booking_->getClass() == BookingClass::Economy) 
                       ? "Hạng phổ thông" : "Thương gia";
    classLabel_ = new QLabel(classStr);
    layout->addWidget(classLabel_, row++, 1);
    
    // Giá vé
    layout->addWidget(new QLabel("Giá vé", this), row, 0);
    QString fareStr = QString::number(booking_->getBaseFare()) + " VND";
    fareLabel_ = new QLabel(fareStr);
    fareLabel_->setStyleSheet("font-weight: 600; color: #2E7D32;");
    layout->addWidget(fareLabel_, row++, 1);
    
    // Trạng thái
    layout->addWidget(new QLabel("Trạng thái", this), row, 0);
    QString statusStr = (booking_->getStatus() == BookingStatus::Issued) 
                        ? "Đang giữ chỗ" : 
                      (booking_->getStatus() == BookingStatus::Cancelled) ? "Đã hủy" : "Đã đổi";
    statusLabel_ = new QLabel(statusStr);
    QString statusColor = (booking_->getStatus() == BookingStatus::Issued) 
                          ? "#2E7D32" : "#C62828";
    statusLabel_->setStyleSheet(QString("font-weight: 600; color: %1;").arg(statusColor));
    layout->addWidget(statusLabel_, row++, 1);
    
    return group;
}

QGroupBox* BookingDetailsDialog::createFlightInfoGroup()
{
    auto *group = new QGroupBox("Thông tin chuyến bay", this);
    group->setStyleSheet("QGroupBox { font-weight: bold; color: #123B7A; }");
    
    auto *layout = new QGridLayout(group);
    layout->setVerticalSpacing(8);
    layout->setHorizontalSpacing(20);
    
    int row = 0;
    
    // Lấy thông tin chuyến bay
    Flight* flight = flightManager_->findFlightById(booking_->getFlightId());
    if (flight) {
        Route* route = flightManager_->findRouteById(flight->getRouteId());
        
        // Số hiệu chuyến bay
        layout->addWidget(new QLabel("Số hiệu chuyến bay", this), row, 0);
        auto *flightNumberLabel = new QLabel(QString::fromStdString(flight->getFlightNumber()));
        flightNumberLabel->setStyleSheet("font-weight: 600;");
        layout->addWidget(flightNumberLabel, row++, 1);
        
        // Hãng bay
        layout->addWidget(new QLabel("Hãng bay", this), row, 0);
        auto *airlineLabel = new QLabel(QString::fromStdString(flight->getAirline()));
        layout->addWidget(airlineLabel, row++, 1);
        
        // Lộ trình
        layout->addWidget(new QLabel("Lộ trình", this), row, 0);
        if (route) {
            auto *routeLabel = new QLabel(
                QString("%1 → %2")
                    .arg(QString::fromStdString(route->getDepartureAirport()))
                    .arg(QString::fromStdString(route->getArrivalAirport()))
            );
            layout->addWidget(routeLabel, row++, 1);
        } else {
            layout->addWidget(new QLabel("Không xác định", this), row++, 1);
        }
        
        // Thời gian khởi hành
        layout->addWidget(new QLabel("Thời gian khởi hành", this), row, 0);
        auto *departureLabel = new QLabel(
            QString("%1 %2")
                .arg(QString::fromStdString(flight->getDepartureDate()))
                .arg(QString::fromStdString(flight->getDepartureTime()))
        );
        layout->addWidget(departureLabel, row++, 1);
        
        // Thời gian hạ cánh (dự kiến)
        layout->addWidget(new QLabel("Thời gian hạ cánh (dự kiến)", this), row, 0);
        auto *arrivalLabel = new QLabel(
            QString("%1 %2")
                .arg(QString::fromStdString(flight->getArrivalDate()))
                .arg(QString::fromStdString(flight->getArrivalTime()))
        );
        layout->addWidget(arrivalLabel, row++, 1);
        
    } else {
        layout->addWidget(new QLabel("Không tìm thấy thông tin chuyến bay", this), 0, 0, 1, 2);
    }
    
    return group;
}

QGroupBox* BookingDetailsDialog::createPassengerInfoGroup()
{
    auto *group = new QGroupBox("Thông tin hành khách", this);
    group->setStyleSheet("QGroupBox { font-weight: bold; color: #123B7A; }");
    
    auto *layout = new QGridLayout(group);
    layout->setVerticalSpacing(8);
    layout->setHorizontalSpacing(20);
    
    int row = 0;
    
    // Lấy thông tin hành khách từ PassengerManager
    Passenger* passenger = passengerManager_->findPassengerById(booking_->getPassengerId());
    
    if (passenger) {
        // CCCD hành khách
        layout->addWidget(new QLabel("CCCD hành khách", this), row, 0);
        auto *passengerIdLabel = new QLabel(QString::fromStdString(passenger->getId()));
        passengerIdLabel->setStyleSheet("font-weight: 600;");
        layout->addWidget(passengerIdLabel, row++, 1);
        
        // Họ và tên
        layout->addWidget(new QLabel("Họ và tên", this), row, 0);
        auto *nameLabel = new QLabel(QString::fromStdString(passenger->getFullName()));
        layout->addWidget(nameLabel, row++, 1);
        
        // Ngày sinh
        layout->addWidget(new QLabel("Ngày sinh", this), row, 0);
        auto *dobLabel = new QLabel(QString::fromStdString(passenger->getDateOfBirth()));
        layout->addWidget(dobLabel, row++, 1);
        
        // Số điện thoại
        layout->addWidget(new QLabel("Số điện thoại", this), row, 0);
        auto *phoneLabel = new QLabel(QString::fromStdString(passenger->getPhoneNumber()));
        layout->addWidget(phoneLabel, row++, 1);
    } else {
        // Fallback: chỉ hiển thị ID nếu không tìm thấy thông tin đầy đủ
        layout->addWidget(new QLabel("CCCD hành khách", this), row, 0);
        auto *passengerIdLabel = new QLabel(QString::fromStdString(booking_->getPassengerId()));
        passengerIdLabel->setStyleSheet("font-weight: 600;");
        layout->addWidget(passengerIdLabel, row++, 1);
        
        layout->addWidget(new QLabel("Không tìm thấy thông tin chi tiết hành khách", this), row, 0, 1, 2);
    }
    
    return group;
}

QFrame* BookingDetailsDialog::createSeparator()
{
    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("color: #E0E0E0;");
    separator->setMaximumHeight(1);
    return separator;
}

//================= gemini ==================//
// #include "BookingDetailsDialog.h"

// #include "entities/Booking.h"
// #include "core/FlightManager.h"
// #include "core/AccountManager.h"
// #include "core/PassengerManager.h" 
// #include "entities/Flight.h"
// #include "entities/Route.h"
// #include "entities/Passenger.h" 

// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QGridLayout>
// #include <QLabel>
// #include <QPushButton>
// #include <QGroupBox>

// // Lưu ý: Cần thêm PassengerManager.h vào BookingDetailsDialog.h
// BookingDetailsDialog::BookingDetailsDialog(Booking* booking,
//                                          FlightManager* flightManager,
//                                          AccountManager* accountManager,
//                                          PassengerManager* passengerManager, // Đã thêm
//                                          QWidget *parent)
//     : QDialog(parent),
//       booking_(booking),
//       flightManager_(flightManager),
//       accountManager_(accountManager),
//       passengerManager_(passengerManager) // Khởi tạo
// {
//     Q_ASSERT(booking_ != nullptr);
//     Q_ASSERT(flightManager_ != nullptr);
//     Q_ASSERT(passengerManager_ != nullptr); 
    
//     setWindowTitle("Chi tiết Đặt chỗ");
//     setFixedSize(450, 600); 
    
//     setupUi();
// }

// void BookingDetailsDialog::setupUi()
// {
//     auto *mainLayout = new QVBoxLayout(this);
//     mainLayout->setSpacing(16);
//     mainLayout->setContentsMargins(20, 20, 20, 20);

//     // Tiêu đề
//     auto *titleLabel = new QLabel("Thông tin chi tiết Đặt chỗ", this);
//     titleLabel->setStyleSheet("font-size: 18px; font-weight: 700; color: #123B7A;");
//     titleLabel->setAlignment(Qt::AlignCenter);
//     mainLayout->addWidget(titleLabel);
    
//     // --- Thông tin Booking ---
//     auto *bookingGroup = new QGroupBox("Thông tin Đặt chỗ", this);
//     bookingGroup->setStyleSheet("QGroupBox::title { font-weight: bold; }"); 
//     auto *bookingLayout = new QGridLayout(bookingGroup);
//     bookingLayout->setSpacing(12);
//     bookingLayout->setContentsMargins(15, 20, 15, 15); 

//     int row = 0;

//     // Hàng 1: Mã đặt chỗ | Ngày đặt 
//     bookingLayout->addWidget(new QLabel("Mã Đặt chỗ:"), row, 0);
//     bookingIdLabel_ = new QLabel(QString::fromStdString(booking_->getBookingId()));
//     bookingIdLabel_->setStyleSheet("font-weight: 600;");
//     bookingLayout->addWidget(bookingIdLabel_, row, 1);

//     bookingLayout->addWidget(new QLabel("Ngày đặt:"), row, 2);
//     bookingDateLabel_ = new QLabel(QString::fromStdString(booking_->getBookingDate()));
//     bookingLayout->addWidget(bookingDateLabel_, row++, 3);
    
//     // Hàng 2: Hạng vé | Giá vé
//     bookingLayout->addWidget(new QLabel("Hạng vé:"), row, 0);
//     QString classStr = (booking_->getClass() == BookingClass::Economy)
//                         ? "Hạng phổ thông" : "Thương gia";
//     classLabel_ = new QLabel(classStr);
//     bookingLayout->addWidget(classLabel_, row, 1);

//     bookingLayout->addWidget(new QLabel("Giá vé:"), row, 2);
//     QString fareStr = QString("%L1 VND").arg(booking_->getBaseFare());
//     fareLabel_ = new QLabel(fareStr);
//     fareLabel_->setStyleSheet("font-weight: 600; color: #2E7D32;");
//     bookingLayout->addWidget(fareLabel_, row++, 3);
    
//     // Hàng 3: Trạng thái
//     bookingLayout->addWidget(new QLabel("Trạng thái:"), row, 0);
//     QString statusStr = (booking_->getStatus() == BookingStatus::Issued)
//                          ? "Đang giữ chỗ" :
//                        (booking_->getStatus() == BookingStatus::Cancelled) ? "Đã hủy" : "Đã đổi";
//     statusLabel_ = new QLabel(statusStr);
//     QString statusColor = (booking_->getStatus() == BookingStatus::Issued)
//                             ? "#2E7D32" : "#C62828";
//     statusLabel_->setStyleSheet(QString("font-weight: 600; color: %1;").arg(statusColor));
//     bookingLayout->addWidget(statusLabel_, row++, 1, 1, 3);
    
//     bookingLayout->setColumnStretch(1, 1);
//     bookingLayout->setColumnStretch(3, 1);
//     bookingLayout->setColumnMinimumWidth(2, 80);

//     mainLayout->addWidget(bookingGroup);
    
//     // --- Thông tin Chuyến bay ---
//     auto *flightGroup = new QGroupBox("Thông tin Chuyến bay", this);
//     flightGroup->setStyleSheet("QGroupBox::title { font-weight: bold; }");
//     auto *flightLayout = new QVBoxLayout(flightGroup);
//     flightLayout->setContentsMargins(15, 20, 15, 15);
//     flightLayout->setSpacing(8);

//     Flight* flight = flightManager_->findFlightById(booking_->getFlightId());
//     if (flight) {
//         Route* route = flightManager_->findRouteById(flight->getRouteId());
        
//         QString flightInfo;
        
//         flightInfo += QString("<p style='margin-bottom: 2px;'>Mã chuyến: <b>%1</b></p>")
//             .arg(QString::fromStdString(flight->getFlightNumber()));
        
//         if (route) {
//             flightInfo += QString("<p style='margin-bottom: 2px;'>Hãng bay: %1</p>")
//                 .arg(QString::fromStdString(flight->getAirline()));
//             flightInfo += QString("<p style='margin-bottom: 2px;'>Lộ trình: <b>%1 → %2</b></p>")
//                 .arg(QString::fromStdString(route->getDepartureAirport()))
//                 .arg(QString::fromStdString(route->getArrivalAirport()));
//         }

//         flightInfo += QString("<p style='margin-bottom: 2px;'>Khởi hành: <b>%1 %2</b></p>")
//             .arg(QString::fromStdString(flight->getDepartureDate()))
//             .arg(QString::fromStdString(flight->getDepartureTime()));
        
//         flightInfo += QString("<p style='margin-bottom: 0px;'>Đến: <b>%1 %2</b></p>")
//             .arg(QString::fromStdString(flight->getArrivalDate()))
//             .arg(QString::fromStdString(flight->getArrivalTime()));
        
//         flightInfoLabel_ = new QLabel(flightInfo);
//         flightInfoLabel_->setTextFormat(Qt::RichText);
//     } else {
//         flightInfoLabel_ = new QLabel("Không tìm thấy thông tin chuyến bay");
//     }
    
//     flightLayout->addWidget(flightInfoLabel_);
//     mainLayout->addWidget(flightGroup);
    
//     // --- Thông tin Hành khách ---
//     auto *passengerGroup = new QGroupBox("Thông tin Hành khách", this);
//     passengerGroup->setStyleSheet("QGroupBox::title { font-weight: bold; }");
//     auto *passengerLayout = new QVBoxLayout(passengerGroup);
//     passengerLayout->setContentsMargins(15, 20, 15, 15);
//     passengerLayout->setSpacing(8);
    
//     Passenger* passenger = passengerManager_->findPassengerById(booking_->getPassengerId());
    
//     QString passengerInfo;
//     if (passenger) {
//         passengerInfo += QString("<p style='margin-bottom: 2px;'>CCCD Hành khách: <b>%1</b></p>")
//             .arg(QString::fromStdString(passenger->getId()));
//         passengerInfo += QString("<p style='margin-bottom: 2px;'>Họ và tên: %1</p>")
//             .arg(QString::fromStdString(passenger->getFullName()));
//         passengerInfo += QString("<p style='margin-bottom: 0px;'>Ngày sinh: %1</p>")
//             .arg(QString::fromStdString(passenger->getDateOfBirth()));
//         passengerInfo += QString("<p style='margin-bottom: 0px;'>SĐT: %1</p>")
//             .arg(QString::fromStdString(passenger->getPhoneNumber()));
//     } else {
//         passengerInfo += QString("<p>ID Hành khách: <b>%1</b></p>")
//             .arg(QString::fromStdString(booking_->getPassengerId()));
//     }
    
//     passengerInfoLabel_ = new QLabel(passengerInfo);
//     passengerInfoLabel_->setTextFormat(Qt::RichText);
//     passengerLayout->addWidget(passengerInfoLabel_);
//     mainLayout->addWidget(passengerGroup);
    
//     mainLayout->addStretch();
    
//     // --- Nút đóng ---
//     auto *buttonLayout = new QHBoxLayout();
//     buttonLayout->addStretch();
    
//     auto *closeButton = new QPushButton("Đóng", this);
//     closeButton->setStyleSheet(
//         "QPushButton { background:#5886C0; color:white; border:none; "
//         "border-radius:6px; padding:10px 30px; font-weight:600; font-size: 14px; }"
//         "QPushButton:hover { background:#466a9a; }"
//     );
//     closeButton->setMinimumWidth(120);
//     connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
//     buttonLayout->addWidget(closeButton);
//     buttonLayout->addStretch();
    
//     mainLayout->addLayout(buttonLayout);
// }