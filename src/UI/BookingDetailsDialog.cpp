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
#include <QGroupBox> 

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
    Q_ASSERT(accountManager_ != nullptr);

    setWindowTitle("Chi tiết đặt chỗ");
    setFixedWidth(700);
    
    setStyleSheet(
        "QDialog { background: white; }"
        "QLabel { border: none; }"
    );
    
    setupUi();

    // Tự động điều chỉnh kích thước dựa trên nội dung
    adjustSize();
}

void BookingDetailsDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20); 
    mainLayout->setContentsMargins(50, 20, 50, 20); 
    
    // Tiêu đề chính
    auto *titleLabel = new QLabel("Chi tiết thông tin đặt chỗ", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #123B7A;"
        "   font-weight: bold;"
        "   font-size: 20px;" 
        "   background: transparent;"
        "   border: none;"
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

    // Nút Đóng (Full Width)
    mainLayout->addSpacing(10);

    auto *closeButton = new QPushButton("Đóng", this);
    closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    closeButton->setStyleSheet(
        "QPushButton {"
        "   background: #608bc1;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   height: 20px;" 
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: #365a9e;"
        "}"
    );
    closeButton->setCursor(Qt::PointingHandCursor);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    mainLayout->addWidget(closeButton);
}

QGroupBox* BookingDetailsDialog::createInfoFrame(const QString& title, QLayout* contentLayout)
{
    auto *groupBox = new QGroupBox(title, this);
    
    // Thiết lập kiểu dáng cho QGroupBox
    groupBox->setStyleSheet(
        "QGroupBox {"
        "   background: white;"
        "   border: 1px solid #608bc1;"
        "   border-radius: 6px;"
        "   margin-top: 10px;" 
        "   font-size: 13px;"
        "   font-weight: bold;" // <-- Dòng này sẽ làm tiêu đề in đậm chắc chắn
        "}"
        "QGroupBox::title {"
        "   font-weight: 650;"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   left: 15px;" 
        "   padding: 0 8px;"
        "   color: #1E4B8C;" 
        "   background-color: white;" 
        "}"
    );
    
    auto *frameLayout = new QVBoxLayout(groupBox);
    frameLayout->setContentsMargins(30, 15, 30, 12); 
    frameLayout->setSpacing(8);
    
    frameLayout->addLayout(contentLayout);
    
    return groupBox;
}

QGridLayout* BookingDetailsDialog::createBookingInfoLayout()
{
    auto *layout = new QGridLayout();
    layout->setVerticalSpacing(8);  
    layout->setHorizontalSpacing(20); 
    
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
                          ? "#2E7D32" : (booking_->getStatus() == BookingStatus::Cancelled) ? "#C62828" : "#F57C00";
    addInfoRow(layout, row++, "Trạng thái", statusStr, true, statusColor);
    
    return layout;
}

QGridLayout* BookingDetailsDialog::createFlightInfoLayout()
{
    auto *layout = new QGridLayout();
    layout->setVerticalSpacing(8);  
    layout->setHorizontalSpacing(20); 
    
    int row = 0;
    
    Flight* flight = flightManager_->findFlightById(booking_->getFlightId());
    if (flight) {
        Route* route = flightManager_->findRouteById(flight->getRouteId());
        
        addInfoRow(layout, row++, "Số hiệu chuyến bay", 
                   QString::fromStdString(flight->getFlightNumber()), true, "#1E4B8C");
        
        addInfoRow(layout, row++, "Hãng bay", 
                   QString::fromStdString(flight->getAirline()), false, "#2A5A8A");
        
        if (route) {
            QString routeStr = QString("%1 → %2")
                .arg(QString::fromStdString(route->getDepartureAirport()))
                .arg(QString::fromStdString(route->getArrivalAirport()));
            addInfoRow(layout, row++, "Lộ trình", routeStr, false, "#2A5A8A");
        } else {
            addInfoRow(layout, row++, "Lộ trình", "Không xác định", false, "#C62828");
        }
        
        QString departureStr = QString("%1 %2")
            .arg(QString::fromStdString(flight->getDepartureDate()))
            .arg(QString::fromStdString(flight->getDepartureTime()));
        addInfoRow(layout, row++, "Thời gian khởi hành", departureStr, false, "#2A5A8A");
        
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
    layout->setVerticalSpacing(8); 
    layout->setHorizontalSpacing(20); 
    
    int row = 0;
    
    Passenger* passenger = passengerManager_->findPassengerById(booking_->getPassengerId());
    
    if (passenger) {
        addInfoRow(layout, row++, "CCCD hành khách", 
                   QString::fromStdString(passenger->getId()), true, "#1E4B8C");
        
        addInfoRow(layout, row++, "Họ và tên", 
                   QString::fromStdString(passenger->getFullName()), false, "#2A5A8A");
        
        addInfoRow(layout, row++, "Ngày sinh", 
                   QString::fromStdString(passenger->getDateOfBirth()), false, "#2A5A8A");
        
        addInfoRow(layout, row++, "Số điện thoại", 
                   QString::fromStdString(passenger->getPhoneNumber()), false, "#2A5A8A");
    } else {
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
    // Label (nhãn)
    auto *label = new QLabel(labelText, this);
    label->setStyleSheet(
        "QLabel {"
        "   color: #133e87;"
        "   font-weight: 600;" // Label bên trái giữ nguyên độ đậm vừa phải
        "   font-size: 12px;" 
        "   background: transparent;"
        "   border: none;"
        "}"
    );
    layout->addWidget(label, row, 0);
    
    // Value (giá trị)
    auto *value = new QLabel(valueText, this);
    
    // [SỬA Ở ĐÂY] Đổi "500" thành "bold" để in đậm rõ ràng
    QString fontWeight = valueBold ? "bold" : "normal";

    QString style = "QLabel {"
                   "   color: " + valueColor + ";"
                   "   font-weight: " + fontWeight + ";" 
                   "   font-size: 12px;" 
                   "   background: transparent;"
                   "   border: none;"
                   "}";
    value->setStyleSheet(style);
    
    layout->addWidget(value, row, 1);
}