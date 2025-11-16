#include "BookingDetailsDialog.h"

#include "entities/Booking.h"
#include "core/FlightManager.h"
#include "core/AccountManager.h"
#include "entities/Flight.h"
#include "entities/Route.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

BookingDetailsDialog::BookingDetailsDialog(Booking* booking, 
                                           FlightManager* flightManager,
                                           AccountManager* accountManager,
                                           QWidget *parent)
    : QDialog(parent),
      booking_(booking),
      flightManager_(flightManager),
      accountManager_(accountManager)
{
    Q_ASSERT(booking_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    
    setWindowTitle("Chi tiết Đặt chỗ");
    setMinimumWidth(500);
    
    setupUi();
}

void BookingDetailsDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Tiêu đề
    auto *titleLabel = new QLabel("Thông tin chi tiết Đặt chỗ", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: 700; color: #123B7A;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Thông tin Booking
    auto *bookingGroup = new QGroupBox("Thông tin Đặt chỗ", this);
    auto *bookingLayout = new QGridLayout(bookingGroup);
    bookingLayout->setSpacing(12);
    
    int row = 0;
    
    // Mã đặt chỗ
    bookingLayout->addWidget(new QLabel("Mã Đặt chỗ:"), row, 0);
    bookingIdLabel_ = new QLabel(QString::fromStdString(booking_->getBookingId()));
    bookingIdLabel_->setStyleSheet("font-weight: 600;");
    bookingLayout->addWidget(bookingIdLabel_, row++, 1);
    
    // Ngày đặt
    bookingLayout->addWidget(new QLabel("Ngày đặt:"), row, 0);
    bookingDateLabel_ = new QLabel(QString::fromStdString(booking_->getBookingDate()));
    bookingLayout->addWidget(bookingDateLabel_, row++, 1);
    
    // Hạng vé
    bookingLayout->addWidget(new QLabel("Hạng vé:"), row, 0);
    QString classStr = (booking_->getClass() == BookingClass::Economy) 
                       ? "Hạng phổ thông" : "Thương gia";
    classLabel_ = new QLabel(classStr);
    bookingLayout->addWidget(classLabel_, row++, 1);
    
    // Giá vé
    bookingLayout->addWidget(new QLabel("Giá vé:"), row, 0);
    QString fareStr = QString::number(booking_->getBaseFare()) + " VND";
    fareLabel_ = new QLabel(fareStr);
    fareLabel_->setStyleSheet("font-weight: 600; color: #2E7D32;");
    bookingLayout->addWidget(fareLabel_, row++, 1);
    
    // Trạng thái
    bookingLayout->addWidget(new QLabel("Trạng thái:"), row, 0);
    QString statusStr = (booking_->getStatus() == BookingStatus::Issued) 
                        ? "Đang giữ chỗ" : 
                      (booking_->getStatus() == BookingStatus::Cancelled) ? "Đã hủy" : "Đã đổi";
    statusLabel_ = new QLabel(statusStr);
    QString statusColor = (booking_->getStatus() == BookingStatus::Issued) 
                          ? "#2E7D32" : "#C62828";
    statusLabel_->setStyleSheet(QString("font-weight: 600; color: %1;").arg(statusColor));
    bookingLayout->addWidget(statusLabel_, row++, 1);
    
    mainLayout->addWidget(bookingGroup);
    
    // Thông tin Chuyến bay
    auto *flightGroup = new QGroupBox("Thông tin Chuyến bay", this);
    auto *flightLayout = new QVBoxLayout(flightGroup);
    
    // Lấy thông tin chuyến bay
    Flight* flight = flightManager_->findFlightById(booking_->getFlightId());
    if (flight) {
        Route* route = flightManager_->findRouteById(flight->getRouteId()); // ✅ ĐÚNG
        
        QString flightInfo = QString("Mã chuyến: %1\n")
            .arg(QString::fromStdString(flight->getFlightNumber()));
        
        if (route) {
            flightInfo += QString("Hãng bay: %1\n").arg(QString::fromStdString(flight->getAirline()));
            flightInfo += QString("Lộ trình: %1 → %2\n")
                .arg(QString::fromStdString(route->getDepartureAirport()))
                .arg(QString::fromStdString(route->getArrivalAirport()));
        }
        
        flightInfo += QString("Khởi hành: %1 %2\n")
            .arg(QString::fromStdString(flight->getDepartureDate()))
            .arg(QString::fromStdString(flight->getDepartureTime()));
        
        flightInfo += QString("Đến: %1 %2")
            .arg(QString::fromStdString(flight->getArrivalDate()))
            .arg(QString::fromStdString(flight->getArrivalTime()));
        
        flightInfoLabel_ = new QLabel(flightInfo);
    } else {
        flightInfoLabel_ = new QLabel("Không tìm thấy thông tin chuyến bay");
    }
    
    flightLayout->addWidget(flightInfoLabel_);
    mainLayout->addWidget(flightGroup);
    
    // Thông tin Hành khách
    auto *passengerGroup = new QGroupBox("Thông tin Hành khách", this);
    auto *passengerLayout = new QVBoxLayout(passengerGroup);
    
    QString passengerInfo = QString("ID Hành khách: %1")
        .arg(QString::fromStdString(booking_->getPassengerId()));
    
    passengerInfoLabel_ = new QLabel(passengerInfo);
    passengerLayout->addWidget(passengerInfoLabel_);
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
