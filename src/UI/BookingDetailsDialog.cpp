#include "BookingDetailsDialog.h"
#include "entities/Booking.h"
#include "core/FlightManager.h"
#include "core/AccountManager.h"
#include "core/PassengerManager.h"
#include "entities/Flight.h"
#include "entities/Route.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

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
    setMinimumWidth(500);  // Giảm từ 600 xuống 500
    setMaximumWidth(650);  // Giảm từ 800 xuống 650
    
    // Style tổng thể với nền trắng đơn giản
    setStyleSheet(
        "QDialog { background: white; }"
        "QLabel { border: none; }"
    );
    
    setupUi();
}

void BookingDetailsDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);  // Giảm spacing
    mainLayout->setContentsMargins(25, 20, 25, 20);  // Giảm margins
    
    // Tiêu đề chính
    auto *titleLabel = new QLabel("Chi tiết thông tin đặt chỗ", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #123B7A;"
        "   font-weight: bold;"
        "   font-size: 20px;"  // Giảm từ 24px xuống 20px
        "   background: transparent;"
        "   border: none;"
        "   margin-bottom: 5px;"
        "}"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // === THÔNG TIN ĐẶT CHỖ ===
    auto *bookingFrame = createInfoFrame("Thông tin đặt chỗ", createBookingInfoLayout());
    mainLayout->addWidget(bookingFrame);

    // === THÔNG TIN CHUYẾN BAY ===
    auto *flightFrame = createInfoFrame("Thông tin chuyến bay", createFlightInfoLayout());
    mainLayout->addWidget(flightFrame);

    // === THÔNG TIN HÀNH KHÁCH ===
    auto *passengerFrame = createInfoFrame("Thông tin hành khách", createPassengerInfoLayout());
    mainLayout->addWidget(passengerFrame);

    // Nút đóng
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    auto *closeButton = new QPushButton("Đóng", this);
    closeButton->setStyleSheet(
        "QPushButton {"
        "   background: #4472C4;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   height: 36px;"
        "   padding: 0 35px;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "   background: #365a9e;"
        "}"
    );
    closeButton->setCursor(Qt::PointingHandCursor);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
}

QFrame* BookingDetailsDialog::createInfoFrame(const QString& title, QLayout* contentLayout)
{
    auto *frame = new QFrame(this);
    frame->setStyleSheet(
        "QFrame {"
        "   background: #F8FAFF;"
        "   border: 2px solid #94B8E8;"  // Viền xanh rõ hơn
        "   border-radius: 8px;"
        "}"
    );
    
    auto *frameLayout = new QVBoxLayout(frame);
    frameLayout->setContentsMargins(15, 15, 15, 15);
    frameLayout->setSpacing(8);
    
    // Tiêu đề phần
    auto *titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #1E4B8C;"
        "   font-weight: bold;"
        "   font-size: 16px;"  // Giảm từ 18px xuống 16px
        "   background: transparent;"
        "   border: none;"
        "   margin-bottom: 5px;"
        "}"
    );
    frameLayout->addWidget(titleLabel);
    
    // Đường phân cách nhỏ dưới tiêu đề
    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("background-color: #94B8E8; color: #94B8E8;");
    separator->setMaximumHeight(1);
    frameLayout->addWidget(separator);
    
    // Nội dung
    frameLayout->addLayout(contentLayout);
    
    return frame;
}

QGridLayout* BookingDetailsDialog::createBookingInfoLayout()
{
    auto *layout = new QGridLayout();
    layout->setVerticalSpacing(8);   // Giảm spacing
    layout->setHorizontalSpacing(20); // Giảm spacing
    
    int row = 0;
    
    // Mã đặt chỗ
    addInfoRow(layout, row++, "Mã đặt chỗ", 
               QString::fromStdString(booking_->getBookingId()), true, "#1E4B8C");
    
    // Thời gian đặt
    addInfoRow(layout, row++, "Thời gian đặt", 
               QString::fromStdString(booking_->getBookingDate()), false, "#2A5A8A");
    
    // Hạng vé
    QString classStr = (booking_->getClass() == BookingClass::Economy) 
                       ? "Hạng phổ thông" : "Thương gia";
    addInfoRow(layout, row++, "Hạng vé", classStr, false, "#2A5A8A");
    
    // Giá vé
    QString fareStr = formatVietnamCurrency(booking_->getBaseFare());
    addInfoRow(layout, row++, "Giá vé", fareStr, false, "#2A5A8A");
    
    // Trạng thái
    QString statusStr = (booking_->getStatus() == BookingStatus::Issued) 
                        ? "Đang giữ chỗ" : 
                      (booking_->getStatus() == BookingStatus::Cancelled) ? "Đã hủy" : "Đã đổi";
    QString statusColor = (booking_->getStatus() == BookingStatus::Issued) 
                          ? "#2E7D32" : "#C62828";
    addInfoRow(layout, row++, "Trạng thái", statusStr, true, statusColor);
    
    return layout;
}

QGridLayout* BookingDetailsDialog::createFlightInfoLayout()
{
    auto *layout = new QGridLayout();
    layout->setVerticalSpacing(8);   // Giảm spacing
    layout->setHorizontalSpacing(20); // Giảm spacing
    
    int row = 0;
    
    // Lấy thông tin chuyến bay
    Flight* flight = flightManager_->findFlightById(booking_->getFlightId());
    if (flight) {
        Route* route = flightManager_->findRouteById(flight->getRouteId());
        
        // Số hiệu chuyến bay
        addInfoRow(layout, row++, "Số hiệu chuyến bay", 
                   QString::fromStdString(flight->getFlightNumber()), true, "#1E4B8C");
        
        // Hãng bay
        addInfoRow(layout, row++, "Hãng bay", 
                   QString::fromStdString(flight->getAirline()), false, "#2A5A8A");
        
        // Lộ trình
        if (route) {
            QString routeStr = QString("%1 → %2")
                .arg(QString::fromStdString(route->getDepartureAirport()))
                .arg(QString::fromStdString(route->getArrivalAirport()));
            addInfoRow(layout, row++, "Lộ trình", routeStr, false, "#2A5A8A");
        } else {
            addInfoRow(layout, row++, "Lộ trình", "Không xác định", false, "#C62828");
        }
        
        // Thời gian khởi hành
        QString departureStr = QString("%1 %2")
            .arg(QString::fromStdString(flight->getDepartureDate()))
            .arg(QString::fromStdString(flight->getDepartureTime()));
        addInfoRow(layout, row++, "Thời gian khởi hành", departureStr, false, "#2A5A8A");
        
        // Thời gian hạ cánh (dự kiến)
        QString arrivalStr = QString("%1 %2")
            .arg(QString::fromStdString(flight->getArrivalDate()))
            .arg(QString::fromStdString(flight->getArrivalTime()));
        addInfoRow(layout, row++, "Thời gian hạ cánh (dự kiến)", arrivalStr, false, "#2A5A8A");
        
    } else {
        addInfoRow(layout, row++, "Lỗi", "Không tìm thấy thông tin chuyến bay", false, "#C62828");
    }
    
    return layout;
}

QGridLayout* BookingDetailsDialog::createPassengerInfoLayout()
{
    auto *layout = new QGridLayout();
    layout->setVerticalSpacing(8);   // Giảm spacing
    layout->setHorizontalSpacing(20); // Giảm spacing
    
    int row = 0;
    
    // Lấy thông tin hành khách từ PassengerManager
    Passenger* passenger = passengerManager_->findPassengerById(booking_->getPassengerId());
    
    if (passenger) {
        // CCCD hành khách
        addInfoRow(layout, row++, "CCCD hành khách", 
                   QString::fromStdString(passenger->getId()), true, "#1E4B8C");
        
        // Họ và tên
        addInfoRow(layout, row++, "Họ và tên", 
                   QString::fromStdString(passenger->getFullName()), false, "#2A5A8A");
        
        // Ngày sinh
        addInfoRow(layout, row++, "Ngày sinh", 
                   QString::fromStdString(passenger->getDateOfBirth()), false, "#2A5A8A");
        
        // Số điện thoại
        addInfoRow(layout, row++, "Số điện thoại", 
                   QString::fromStdString(passenger->getPhoneNumber()), false, "#2A5A8A");
    } else {
        // Fallback: chỉ hiển thị ID nếu không tìm thấy thông tin đầy đủ
        addInfoRow(layout, row++, "CCCD hành khách", 
                   QString::fromStdString(booking_->getPassengerId()), true, "#1E4B8C");
        
        auto *errorLabel = new QLabel("Không tìm thấy thông tin chi tiết hành khách", this);
        errorLabel->setStyleSheet("color: #6688AA; font-style: italic; font-size: 11px;");
        layout->addWidget(errorLabel, row, 0, 1, 2);
    }
    
    return layout;
}

void BookingDetailsDialog::addInfoRow(QGridLayout* layout, int row, 
                                     const QString& labelText, 
                                     const QString& valueText, 
                                     bool valueBold,
                                     const QString& valueColor)
{
    // Label (nhãn) - Màu xanh đậm và in đậm
    auto *label = new QLabel(labelText, this);
    label->setStyleSheet(
        "QLabel {"
        "   color: #1E4B8C;"
        "   font-weight: bold;"
        "   font-size: 13px;"  // Giảm từ 14px xuống 13px
        "   background: transparent;"
        "   border: none;"
        "}"
    );
    layout->addWidget(label, row, 0);
    
    // Value (giá trị) - Màu xanh với các cấp độ khác nhau
    auto *value = new QLabel(valueText, this);
    
    QString style = "QLabel {"
                   "   color: " + valueColor + ";"
                   "   font-weight: " + (valueBold ? "bold" : "normal") + ";"
                   "   font-size: 13px;"  // Giảm từ 14px xuống 13px
                   "   background: transparent;"
                   "   border: none;"
                   "}";
    value->setStyleSheet(style);
    
    layout->addWidget(value, row, 1);
}