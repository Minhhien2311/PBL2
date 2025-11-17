#include "FlightDialog.h"
#include "AirportComboBox.h"
#include "core/FlightManager.h"
#include "core/AirportManager.h"
#include "entities/Flight.h"
#include "entities/Route.h"
#include "utils/Helpers.h"
#include <string>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QSpinBox>
#include <QMessageBox>
#include <QDate>
#include <QTime>
#include <QGroupBox>
#include <QScrollArea>

// Constructor cho THÊM MỚI
FlightDialog::FlightDialog(FlightManager* flightManager, 
                          AirportManager* airportManager,
                          QWidget* parent)
    : QDialog(parent),
      flightManager_(flightManager),
      airportManager_(airportManager),
      isEditMode_(false)
{
    setupUi(false);
    loadExistingRoutes(true);
}

// FlightDialog.cpp - Constructor EDIT mode (dòng 33-97)
FlightDialog::FlightDialog(FlightManager* flightManager,
                          AirportManager* airportManager,
                          const QString& flightId,
                          const QString& flightNumber,
                          const QString& airline,
                          const QString& fromIATA,
                          const QString& toIATA,
                          const QString& depDate,
                          const QString& depTime,
                          const QString& arrDate,
                          const QString& arrTime,
                          int totalCapacity,
                          int fareEconomy,
                          int fareBusiness,
                          QWidget* parent)
    : QDialog(parent),
      flightManager_(flightManager),
      airportManager_(airportManager),
      isEditMode_(true),
      currentFlightId_(flightId)
{
    setupUi(true);
    
    // ✅ SỬA: Load routes KHÔNG có placeholder
    loadExistingRoutes(false);  // ← THAY ĐỔI: false = không thêm "-- Chọn tuyến bay --"
    
    // ✅ SỬA: Tìm và chọn route bằng routeId thay vì text
    QString currentRouteId = fromIATA + "-" + toIATA;  // ← VD: "HAN-SGN"
    
    for (int i = 0; i < flightRouteCombo_->count(); ++i) {
        QString itemRouteId = flightRouteCombo_->itemData(i).toString();  // ← So sánh bằng DATA, không phải TEXT
        if (itemRouteId == currentRouteId) {
            flightRouteCombo_->setCurrentIndex(i);
            break;
        }
    }
    
    // ✅ KIỂM TRA: Nếu vẫn không tìm thấy, log warning
    if (flightRouteCombo_->currentIndex() == -1 || 
        flightRouteCombo_->currentData().toString().isEmpty()) {
        qWarning() << "Warning: Could not find route" << currentRouteId 
                   << "in combo box. Available routes:";
        for (int i = 0; i < flightRouteCombo_->count(); ++i) {
            qWarning() << "  -" << flightRouteCombo_->itemData(i).toString() 
                      << ":" << flightRouteCombo_->itemText(i);
        }
    }
    
    // Trigger event để cập nhật fromCombo_ và toCombo_
    onFlightRouteChanged();  // ← Cập nhật điểm đi/điểm đến
    
    // Set hãng hàng không
    airlineCombo_->setCurrentText(airline);

    // Set số hiệu (có thể sửa)
    flightNumberEdit_->setText(flightNumber);
    
    // Set lịch trình
    departureDateEdit_->setDate(QDate::fromString(depDate, "dd/MM/yyyy"));
    departureTimeEdit_->setTime(QTime::fromString(depTime, "HH:mm"));
    arrivalDateEdit_->setDate(QDate::fromString(arrDate, "dd/MM/yyyy"));
    arrivalTimeEdit_->setTime(QTime::fromString(arrTime, "HH:mm"));
    
    // Set sức chứa và giá
    totalCapacitySpin_->setValue(totalCapacity);
    fareEconomySpin_->setValue(fareEconomy);
    fareBusinessSpin_->setValue(fareBusiness);
}
void FlightDialog::setupUi(bool isEditMode)
{
    setWindowTitle(isEditMode ? "Sửa chuyến bay" : "Thêm chuyến bay mới");
    setMinimumWidth(650);
    setMinimumHeight(600);
    
    setStyleSheet(
        "QDialog { background: #F2F6FD; }"
        "QLabel { color: #123B7A; font-weight: 600; }"
        "QLineEdit, QComboBox, QDateEdit, QTimeEdit, QSpinBox { "
        "  background: white; "
        "  border: 1px solid #608bc1; "
        "  border-radius: 4px; "
        "  height: 32px; "
        "  padding-left: 8px; "
        "}"
        "QLineEdit:read-only { background: #f0f0f0; color: #666; }"
        "QComboBox:disabled { background: #f0f0f0; color: #666; }"
        "QGroupBox { "
        "  border: 1px solid #c2cfe2; "
        "  border-radius: 6px; "
        "  margin-top: 10px; "
        "  font-weight: 600; "
        "  color: #123B7A; "
        "}"
        "QGroupBox::title { "
        "  subcontrol-origin: margin; "
        "  subcontrol-position: top left; "
        "  padding: 0 5px; "
        "  background: #F2F6FD; "
        "}"
        "QPushButton { "
        "  background: #5886C0; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 6px; "
        "  height: 40px; "
        "  padding: 0 24px; "
        "  font-weight: 600; "
        "}"
        "QPushButton:hover { background: #466a9a; }"
        "QPushButton#cancelBtn { "
        "  background: #999; "
        "  color: white; "
        "}"
        "QPushButton#cancelBtn:hover { background: #777; }"
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical { "
        "  border: none; "
        "  background: #E8EEF7; "
        "  width: 10px; "
        "  margin: 0; "
        "}"
        "QScrollBar::handle:vertical { "
        "  background: #5886C0; "
        "  border-radius: 5px; "
        "  min-height: 20px; "
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
        "  height: 0px; "
        "}"
    );

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Tiêu đề (cố định)
    QWidget* headerWidget = new QWidget();
    headerWidget->setStyleSheet("background: #F2F6FD;");
    auto* headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(24, 24, 24, 12);
    
    QLabel* titleLabel = new QLabel(isEditMode ? "Chỉnh sửa thông tin chuyến bay" : "Nhập thông tin chuyến bay mới");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #123B7A;");
    headerLayout->addWidget(titleLabel);
    
    mainLayout->addWidget(headerWidget);

    // Scroll Area cho nội dung
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget* scrollContent = new QWidget();
    auto* contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(24, 12, 24, 24);

    // ========== NHÓM 1: THÔNG TIN TUYẾN BAY ==========
    QGroupBox* routeGroup = new QGroupBox("📍 Thông tin tuyến bay");
    auto* routeLayout = new QFormLayout(routeGroup);
    routeLayout->setSpacing(12);

    // Tạo các widget trước
    flightRouteCombo_ = new QComboBox();
    fromCombo_ = new AirportComboBox(airportManager_);
    toCombo_ = new AirportComboBox(airportManager_);

    airlineCombo_ = new QComboBox();
    std::vector<std::string> airlines = Helpers::loadAirlinesFromFile("C:/PBL2/data/airlines.txt");
    for (const std::string& airline : airlines) {
        airlineCombo_->addItem(QString::fromStdString(airline));
    }
    if (!isEditMode) {
        airlineCombo_->insertItem(0, "-- Chọn hãng hàng không --", "");
        airlineCombo_->setCurrentIndex(0);  // ← Set về placeholder
    }
    
    flightNumberEdit_ = new QLineEdit();
    flightNumberEdit_->setPlaceholderText("VD: VN123");

    if (!isEditMode) {
        // MODE THÊM MỚI
        flightRouteCombo_->addItem("-- Chọn tuyến bay --", "");
        
        // Thứ tự: 1. Chọn tuyến
        routeLayout->addRow("Chọn tuyến:", flightRouteCombo_);
        
        // 2. Điểm đi (disabled)
        fromCombo_->setEnabled(false);
        routeLayout->addRow("Điểm đi:", fromCombo_);
        
        // 3. Điểm đến (disabled)
        toCombo_->setEnabled(false);
        routeLayout->addRow("Điểm đến:", toCombo_);

        // 4. Hãng hàng không (có thể sửa)
        airlineCombo_->setEnabled(true);
        routeLayout->addRow("Hãng hàng không:", airlineCombo_);
        
        // 5. Số hiệu chuyến bay
        routeLayout->addRow("Số hiệu chuyến bay:", flightNumberEdit_);
        
        connect(flightRouteCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &FlightDialog::onFlightRouteChanged);
        
    } else {
        // MODE CHỈNH SỬA - TẤT CẢ ĐỀU SỬA ĐƯỢC
        
        // 1. Chọn tuyến bay (CÓ THỂ SỬA)
        flightRouteCombo_->removeItem(0);   // Xoá mục "-- Chọn tuyến bay --"
        routeLayout->addRow("Chọn tuyến:", flightRouteCombo_);
        
        // 2. Điểm đi (chỉ hiển thị, tự động cập nhật theo tuyến)
        fromCombo_->setEnabled(false);
        routeLayout->addRow("Điểm đi:", fromCombo_);
        
        // 3. Điểm đến (chỉ hiển thị, tự động cập nhật theo tuyến)
        toCombo_->setEnabled(false);
        routeLayout->addRow("Điểm đến:", toCombo_);
        
        // 4. Hãng hàng không (CÓ THỂ SỬA)
        routeLayout->addRow("Hãng hàng không:", airlineCombo_);
        
        // 5. Số hiệu (CÓ THỂ SỬA)
        routeLayout->addRow("Số hiệu chuyến bay:", flightNumberEdit_);
        
        // Kết nối sự kiện thay đổi tuyến bay
        connect(flightRouteCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &FlightDialog::onFlightRouteChanged);
    }

    contentLayout->addWidget(routeGroup);

    // ========== NHÓM 2: LỊCH TRÌNH BAY (CÓ THỂ SỬA) ==========
    QGroupBox* scheduleGroup = new QGroupBox("🕒 Lịch trình bay");
    auto* scheduleLayout = new QFormLayout(scheduleGroup);
    scheduleLayout->setSpacing(12);

    departureDateEdit_ = new QDateEdit();
    departureDateEdit_->setCalendarPopup(true);
    departureDateEdit_->setDisplayFormat("dd/MM/yyyy");
    departureDateEdit_->setDate(QDate::currentDate().addDays(1));
    departureDateEdit_->setMinimumDate(QDate::currentDate());
    scheduleLayout->addRow("Ngày khởi hành:", departureDateEdit_);

    // Giờ khởi hành - NHẬP TAY
    departureTimeEdit_ = new QTimeEdit();
    departureTimeEdit_->setDisplayFormat("HH:mm");
    departureTimeEdit_->setTime(QTime(6, 0));
    departureTimeEdit_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    departureTimeEdit_->setAlignment(Qt::AlignLeft);
    scheduleLayout->addRow("Giờ khởi hành:", departureTimeEdit_);

    arrivalDateEdit_ = new QDateEdit();
    arrivalDateEdit_->setCalendarPopup(true);
    arrivalDateEdit_->setDisplayFormat("dd/MM/yyyy");
    arrivalDateEdit_->setDate(QDate::currentDate().addDays(1));
    arrivalDateEdit_->setMinimumDate(QDate::currentDate());
    scheduleLayout->addRow("Ngày hạ cánh:", arrivalDateEdit_);

    // Giờ hạ cánh - NHẬP TAY
    arrivalTimeEdit_ = new QTimeEdit();
    arrivalTimeEdit_->setDisplayFormat("HH:mm");
    arrivalTimeEdit_->setTime(QTime(8, 0));
    arrivalTimeEdit_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    arrivalTimeEdit_->setAlignment(Qt::AlignLeft);
    scheduleLayout->addRow("Giờ hạ cánh:", arrivalTimeEdit_);

    contentLayout->addWidget(scheduleGroup);

    // ========== NHÓM 3: THÔNG TIN GIÁ VÀ SỨC CHỨA (CÓ THỂ SỬA) ==========
    QGroupBox* priceGroup = new QGroupBox("💺 Sức chứa & Giá vé");
    auto* priceLayout = new QFormLayout(priceGroup);
    priceLayout->setSpacing(12);

    totalCapacitySpin_ = new QSpinBox();
    totalCapacitySpin_->setRange(50, 500);
    totalCapacitySpin_->setValue(180);
    totalCapacitySpin_->setSuffix(" ghế");
    totalCapacitySpin_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    priceLayout->addRow("Tổng số ghế:", totalCapacitySpin_);

    fareEconomySpin_ = new QSpinBox();
    fareEconomySpin_->setRange(500000, 10000000);
    fareEconomySpin_->setValue(1500000);
    fareEconomySpin_->setSingleStep(100000);
    fareEconomySpin_->setSuffix(" VNĐ");
    fareEconomySpin_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    priceLayout->addRow("Giá vé Phổ thông:", fareEconomySpin_);

    fareBusinessSpin_ = new QSpinBox();
    fareBusinessSpin_->setRange(1000000, 20000000);
    fareBusinessSpin_->setValue(3000000);
    fareBusinessSpin_->setSingleStep(100000);
    fareBusinessSpin_->setSuffix(" VNĐ");
    fareBusinessSpin_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    priceLayout->addRow("Giá vé Thương gia:", fareBusinessSpin_);

    contentLayout->addWidget(priceGroup);

    // Thông báo (nếu edit)
    if (isEditMode) {
        QLabel* noteLabel = new QLabel(
            QString("⚠️ <b>Lưu ý:</b> Đang sửa chuyến bay <b>%1</b>").arg(currentFlightId_)
        );
        noteLabel->setStyleSheet("color: #d97706; font-size: 12px;");
        noteLabel->setWordWrap(true);
        contentLayout->addWidget(noteLabel);
    }

    contentLayout->addStretch();
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    // Nút bấm (cố định ở dưới)
    QWidget* footerWidget = new QWidget();
    footerWidget->setStyleSheet("background: #F2F6FD; border-top: 1px solid #c2cfe2;");
    auto* footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(24, 12, 24, 12);
    footerLayout->setSpacing(12);

    saveButton_ = new QPushButton(isEditMode ? "💾 Lưu thay đổi" : "➕ Thêm chuyến");
    saveButton_->setStyleSheet(
        "QPushButton { "
        "  background: #5886C0; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 6px; "
        "  height: 40px; "
        "  padding: 0 24px; "
        "  font-weight: 600; "
        "}"
        "QPushButton:hover { background: #466a9a; }"
    );
    
    cancelButton_ = new QPushButton("Hủy");
    cancelButton_->setObjectName("cancelBtn");
    cancelButton_->setStyleSheet(
        "QPushButton { "
        "  background: #999; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 6px; "
        "  height: 40px; "
        "  padding: 0 24px; "
        "  font-weight: 600; "
        "}"
        "QPushButton:hover { background: #777; }"
    );

    footerLayout->addStretch();
    footerLayout->addWidget(saveButton_);      // Nút Lưu/Thêm BÊN TRÁI
    footerLayout->addWidget(cancelButton_);    // Nút Hủy BÊN PHẢI

    mainLayout->addWidget(footerWidget);

    connect(saveButton_, &QPushButton::clicked, this, &FlightDialog::onAccept);
    connect(cancelButton_, &QPushButton::clicked, this, &QDialog::reject);
}

// DÒNG 388 - loadExistingFlights() - NÊN ĐỔI THÀNH loadExistingRoutes()
void FlightDialog::loadExistingRoutes(bool includeEmptyOption) {
    flightRouteCombo_->clear();
    if (includeEmptyOption) {
        flightRouteCombo_->addItem("-- Chọn tuyến bay --", "");
    }

    const std::vector<Route*>& routes = flightManager_->getAllRoutes();
    
    for (int i = 0; i < routes.size(); ++i) {
        Route* route = routes[i];
        if (route) {
            QString displayText = QString("%1 → %2")
                .arg(QString::fromStdString(airportManager_->getDisplayName(route->getDepartureAirport())))
                .arg(QString::fromStdString(airportManager_->getDisplayName(route->getArrivalAirport())));
            
            flightRouteCombo_->addItem(displayText, QString::fromStdString(route->getRouteId()));
        }
    }
}

void FlightDialog::onFlightRouteChanged()
{
    QString selectedFlightId = flightRouteCombo_->currentData().toString();
    
    // Reset các trường khi chọn "-- Chọn tuyến bay --"
    if (selectedFlightId.isEmpty()) {
        if (!isEditMode_) {
            flightNumberEdit_->clear();
        }
        airlineCombo_->setCurrentText("--");
        fromCombo_->setCurrentIndex(0);
        toCombo_->setCurrentIndex(0);
        return;
    }
    
    Route* route = flightManager_->findRouteById(selectedFlightId.toStdString());
    if (!route) return;
    
    // Tự động cập nhật thông tin tuyến
    if (!isEditMode_) {
        flightNumberEdit_->clear(); // Ở mode thêm, xóa số hiệu để user tự nhập
    }
    // Ở mode edit, GIỮ NGUYÊN số hiệu đã nhập
    
    fromCombo_->setSelectedIATA(route->getDepartureAirport());
    toCombo_->setSelectedIATA(route->getArrivalAirport());
}

void FlightDialog::onAccept()
{
    // Validate
    if (flightRouteCombo_->currentIndex() == 0 || flightRouteCombo_->currentData().toString().isEmpty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng chọn tuyến bay.");
        return;
    }

    if (flightNumberEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng nhập số hiệu chuyến bay.");
        return;
    }

    QDate depDate = departureDateEdit_->date();
    QTime depTime = departureTimeEdit_->time();
    QDate arrDate = arrivalDateEdit_->date();
    QTime arrTime = arrivalTimeEdit_->time();

    QDateTime depDateTime(depDate, depTime);
    QDateTime arrDateTime(arrDate, arrTime);
    
    if (arrDateTime <= depDateTime) {
        QMessageBox::warning(this, "Dữ liệu không hợp lệ", 
            "Giờ hạ cánh phải sau giờ khởi hành.");
        return;
    }

    if (fareBusinessSpin_->value() <= fareEconomySpin_->value()) {
        QMessageBox::warning(this, "Dữ liệu không hợp lệ", 
            "Giá vé Thương gia phải cao hơn giá vé Phổ thông.");
        return;
    }

    accept();
}

// Getters
QString FlightDialog::getFlightId() const {
    return flightRouteCombo_->currentData().toString();
}

QString FlightDialog::getFlightNumber() const {
    return flightNumberEdit_->text().trimmed();
}

QString FlightDialog::getAirline() const {
    return airlineCombo_->currentText();
}

QString FlightDialog::getFromIATA() const {
    return QString::fromStdString(fromCombo_->getSelectedIATA());
}

QString FlightDialog::getToIATA() const {
    return QString::fromStdString(toCombo_->getSelectedIATA());
}

QString FlightDialog::getDepartureDate() const {
    return departureDateEdit_->date().toString("dd/MM/yyyy");
}

QString FlightDialog::getDepartureTime() const {
    return departureTimeEdit_->time().toString("HH:mm");
}

QString FlightDialog::getArrivalDate() const {
    return arrivalDateEdit_->date().toString("dd/MM/yyyy");
}

QString FlightDialog::getArrivalTime() const {
    return arrivalTimeEdit_->time().toString("HH:mm");
}

int FlightDialog::getTotalCapacity() const {
    return totalCapacitySpin_->value();
}

int FlightDialog::getFareEconomy() const {
    return fareEconomySpin_->value();
}

int FlightDialog::getFareBusiness() const {
    return fareBusinessSpin_->value();
}

QString FlightDialog::getRouteId() const {
    return flightRouteCombo_->currentData().toString();
}