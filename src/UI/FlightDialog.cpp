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
    setWindowTitle("Thêm chuyến bay");
    setFixedWidth(700);
    setMinimumHeight(630);
    
    setStyleSheet(
        "QDialog { background: white; }"
        "QLabel { border: none; }"
    );

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
                          int availableSeats,
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
    setWindowTitle("Sửa chuyến bay");
    setFixedWidth(700);
    setMinimumHeight(630);

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
    // --- THIẾT LẬP CHUNG (Giữ nguyên) ---
    setWindowTitle(isEditMode ? "Sửa chuyến bay" : "Thêm chuyến bay mới");
    
    setStyleSheet(
        "QDialog { background: white; }"
        "QLabel { color: #133e87; font-weight: 600; font-size: 12px; border: none; }"
        "QLineEdit, QComboBox, QDateEdit, QTimeEdit, QSpinBox { "
        "   background: white; border: 1px solid #608bc1; border-radius: 4px; height: 20px; "
        "   padding-left: 8px; font-size: 12px; color: #1E4B8C; }"
        "QLineEdit:focus, QComboBox:focus, QSpinBox:focus { border: 2px solid #1E4B8C; }"
        "QLineEdit:read-only, QComboBox:disabled { background: #f5f7fa; color: #666; border: 1px solid #cbd5e0; }"
        "QGroupBox { background: white; border: 1px solid #608bc1; border-radius: 6px; margin-top: 12px; font-size: 13px; font-weight: bold; }"
        "QGroupBox::title { font-weight: 650; subcontrol-origin: margin; subcontrol-position: top left; left: 15px; padding: 0 8px; color: #1E4B8C; background-color: white; }"
    );

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(40, 20, 40, 20);

    // Header
    auto *titleLabel = new QLabel(isEditMode ? "Chỉnh sửa thông tin chuyến bay" : "Nhập thông tin chuyến bay mới", this);
    titleLabel->setStyleSheet("color: #123B7A; font-weight: bold; font-size: 20px; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Scroll Area
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: white; } QWidget { background: white; }");
    
    QWidget* scrollContent = new QWidget();
    auto* contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setSpacing(15);
    contentLayout->setContentsMargins(5, 5, 5, 5);

    // ========== NHÓM 1: THÔNG TIN TUYẾN BAY (Đã bỏ from/to combo) ==========
    QGroupBox* routeGroup = new QGroupBox("Thông tin tuyến bay");
    auto* routeLayout = new QFormLayout(routeGroup);
    routeLayout->setContentsMargins(30, 20, 30, 15);
    routeLayout->setSpacing(10);
    routeLayout->setLabelAlignment(Qt::AlignLeft);

    flightRouteCombo_ = new QComboBox();
    airlineCombo_ = new QComboBox();
    flightNumberEdit_ = new QLineEdit();
    flightNumberEdit_->setPlaceholderText("VD: VN123");

    // Load Airline
    std::vector<std::string> airlines = Helpers::loadAirlinesFromFile("C:/PBL2/data/airlines.txt");
    for (const std::string& airline : airlines) {
        airlineCombo_->addItem(QString::fromStdString(airline));
    }

    routeLayout->addRow("Chọn tuyến:", flightRouteCombo_);
    routeLayout->addRow("Hãng hàng không:", airlineCombo_);
    routeLayout->addRow("Số hiệu chuyến bay:", flightNumberEdit_);

    if (!isEditMode) {
        airlineCombo_->insertItem(0, "-- Chọn hãng hàng không --", "");
        airlineCombo_->setCurrentIndex(0);
        flightRouteCombo_->addItem("-- Chọn tuyến bay --", "");
    } else {
        flightRouteCombo_->removeItem(0);
    }

    connect(flightRouteCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FlightDialog::onFlightRouteChanged);
    
    contentLayout->addWidget(routeGroup);

    // ========== NHÓM 2: LỊCH TRÌNH BAY (Giữ nguyên) ==========
    QGroupBox* scheduleGroup = new QGroupBox("Lịch trình bay");
    auto* scheduleLayout = new QFormLayout(scheduleGroup);
    scheduleLayout->setContentsMargins(30, 20, 30, 15);
    scheduleLayout->setSpacing(10);
    
    departureDateEdit_ = new QDateEdit();
    departureDateEdit_->setCalendarPopup(true);
    departureDateEdit_->setDisplayFormat("dd/MM/yyyy");
    departureDateEdit_->setDate(QDate::currentDate().addDays(1));
    
    departureTimeEdit_ = new QTimeEdit();
    departureTimeEdit_->setDisplayFormat("HH:mm");
    departureTimeEdit_->setTime(QTime(6, 0));
    departureTimeEdit_->setButtonSymbols(QAbstractSpinBox::NoButtons);

    arrivalDateEdit_ = new QDateEdit();
    arrivalDateEdit_->setCalendarPopup(true);
    arrivalDateEdit_->setDisplayFormat("dd/MM/yyyy");
    arrivalDateEdit_->setDate(QDate::currentDate().addDays(1));

    arrivalTimeEdit_ = new QTimeEdit();
    arrivalTimeEdit_->setDisplayFormat("HH:mm");
    arrivalTimeEdit_->setTime(QTime(8, 0));
    arrivalTimeEdit_->setButtonSymbols(QAbstractSpinBox::NoButtons);

    QHBoxLayout* depLayout = new QHBoxLayout();
    depLayout->addWidget(departureDateEdit_, 6);
    depLayout->addWidget(departureTimeEdit_, 4);
    scheduleLayout->addRow("Khởi hành:", depLayout);

    QHBoxLayout* arrLayout = new QHBoxLayout();
    arrLayout->addWidget(arrivalDateEdit_, 6);
    arrLayout->addWidget(arrivalTimeEdit_, 4);
    scheduleLayout->addRow("Hạ cánh:", arrLayout);

    contentLayout->addWidget(scheduleGroup);

    // ========== NHÓM 3: SỨC CHỨA & GIÁ VÉ (Giữ nguyên) ==========
    QGroupBox* priceGroup = new QGroupBox("Sức chứa & Giá vé");
    auto* priceLayout = new QFormLayout(priceGroup);
    priceLayout->setContentsMargins(30, 20, 30, 15);
    priceLayout->setSpacing(10);

    totalCapacitySpin_ = new QSpinBox();
    totalCapacitySpin_->setRange(50, 500);
    totalCapacitySpin_->setValue(180);
    totalCapacitySpin_->setSuffix(" ghế");
    
    fareEconomySpin_ = new QSpinBox();
    fareEconomySpin_->setRange(500000, 10000000);
    fareEconomySpin_->setValue(1500000);
    fareEconomySpin_->setSingleStep(100000);
    fareEconomySpin_->setSuffix(" VNĐ");

    fareBusinessSpin_ = new QSpinBox();
    fareBusinessSpin_->setRange(1000000, 20000000);
    fareBusinessSpin_->setValue(3000000);
    fareBusinessSpin_->setSingleStep(100000);
    fareBusinessSpin_->setSuffix(" VNĐ");

    priceLayout->addRow("Tổng số ghế:", totalCapacitySpin_);
    priceLayout->addRow("Giá vé Phổ thông:", fareEconomySpin_);
    priceLayout->addRow("Giá vé Thương gia:", fareBusinessSpin_);
    contentLayout->addWidget(priceGroup);

    if (isEditMode) {
        QLabel* noteLabel = new QLabel(QString("⚠️ Đang sửa chuyến bay: %1").arg(currentFlightId_));
        noteLabel->setStyleSheet("color: #FF5F57; font-style: italic; margin-left: 10px;");
        noteLabel->setAlignment(Qt::AlignCenter);
        contentLayout->addWidget(noteLabel);
    }

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    // Footer Buttons
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    saveButton_ = new QPushButton(isEditMode ? "Lưu thay đổi" : "Thêm chuyến bay");
    saveButton_->setCursor(Qt::PointingHandCursor);
    saveButton_->setStyleSheet("QPushButton { background: #608bc1; color: white; border: none; border-radius: 6px; height: 35px; font-weight: bold; font-size: 13px; } QPushButton:hover { background: #365a9e; }");

    cancelButton_ = new QPushButton("Hủy bỏ");
    cancelButton_->setCursor(Qt::PointingHandCursor);
    cancelButton_->setStyleSheet("QPushButton { background: #E0E0E0; color: #333; border: none; border-radius: 6px; height: 35px; font-weight: bold; font-size: 13px; } QPushButton:hover { background: #BDBDBD; }");
    
    buttonLayout->addWidget(cancelButton_, 1);
    buttonLayout->addWidget(saveButton_, 2);
    mainLayout->addLayout(buttonLayout);

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
    
    // Nếu user chọn "-- Chọn tuyến bay --" hoặc rỗng
    if (selectedFlightId.isEmpty()) {
        if (!isEditMode_) {
            flightNumberEdit_->clear();
        }
        airlineCombo_->setCurrentText("--"); // Reset hãng nếu cần
        return;
    }
    
    // Nếu ở mode thêm mới, có thể muốn clear số hiệu để nhập cái mới
    if (!isEditMode_) {
        flightNumberEdit_->clear();
    }
    
    // Không cần set fromCombo/toCombo nữa vì đã xóa khỏi UI
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

// Lấy điểm ĐI trực tiếp từ Route ID đang chọn
QString FlightDialog::getFromIATA() const {
    QString routeId = flightRouteCombo_->currentData().toString();
    if (routeId.isEmpty()) return "";

    Route* route = flightManager_->findRouteById(routeId.toStdString());
    if (route) {
        return QString::fromStdString(route->getDepartureAirport());
    }
    return "";
}

// Lấy điểm ĐẾN trực tiếp từ Route ID đang chọn
QString FlightDialog::getToIATA() const {
    QString routeId = flightRouteCombo_->currentData().toString();
    if (routeId.isEmpty()) return "";

    Route* route = flightManager_->findRouteById(routeId.toStdString());
    if (route) {
        return QString::fromStdString(route->getArrivalAirport());
    }
    return "";
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

int FlightDialog::getAvailableSeats() const {
    // Giả sử available seats = total capacity lúc tạo chuyến bay mới
    return totalCapacitySpin_->value();
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