#include "BookingDialog.h"

#include "core/AccountManager.h"
#include "core/AirportManager.h"
#include "core/BookingManager.h"
#include "core/FlightManager.h"
#include "core/PassengerManager.h"
#include "core/SeatManager.h"
#include "entities/Account.h"
#include "entities/Flight.h"
#include "entities/Seat.h"
#include "utils/DateTime.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QDateTime>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <iostream>

namespace {
    const QString SEAT_BASE_STYLE = 
        "QPushButton { border: none; border-radius: 3px; }"; 

    const QString BUSINESS_AVAILABLE_STYLE = SEAT_BASE_STYLE + 
        "QPushButton { background: #FFBD2E; } QPushButton:hover { background: #FFC700; border: 2px solid #FF6B35; }";
    
    const QString ECONOMY_AVAILABLE_STYLE = SEAT_BASE_STYLE + 
        "QPushButton { background: #27C93F; } QPushButton:hover { background: #3ad94a; border: 2px solid #FF6B35; }";
    
    const QString BOOKED_STYLE = SEAT_BASE_STYLE + 
        "QPushButton { background: #D3D3D3; }";
    
    const QString LOCKED_STYLE = SEAT_BASE_STYLE + 
        "QPushButton { background: #FF5F57; }";
    
    const QString SELECTED_STYLE_SUFFIX = " border: 2px solid #FF6B35;";
}

BookingDialog::BookingDialog(Flight* flight, 
                             FlightManager* flightManager,
                             BookingManager* bookingManager,
                             AccountManager* accountManager,
                             PassengerManager* passengerManager,
                             AirportManager* airportManager,
                             QWidget *parent)
    : QDialog(parent),
      flight_(flight),
      flightManager_(flightManager),
      bookingManager_(bookingManager),
      accountManager_(accountManager),
      passengerManager_(passengerManager),
      airportManager_(airportManager),
      seatMapContainer_(nullptr),
      seatMapLayout_(nullptr),
      selectedSeatId_("")
{
    Q_ASSERT(flight_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    
    setWindowTitle("Đặt vé máy bay");
    setFixedSize(700, 630);
    setStyleSheet("QDialog { background-color: #FFFFFF; }");
    
    setupUi();
    updateStepIndicator();
}

void BookingDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 0, 50, 20);
    mainLayout->setSpacing(0);
    
    setupProgressHeader();
    
    stackedWidget_ = new QStackedWidget(this);
    stackedWidget_->setStyleSheet("QStackedWidget { background-color: #FFFFFF; border: none; }");
    
    setupPage1_FlightInfo();
    setupPage2_PassengerInfo();
    setupPage3_SeatSelection();
    stackedWidget_->setCurrentIndex(0);
    
    mainLayout->addWidget(stackedWidget_);
    
    auto *footerWidget = new QWidget(this);
    footerWidget->setStyleSheet("background-color: #FFFFFF; border: none;");
    auto *footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(0, 0, 0, 0);

    cancelBtn_ = new QPushButton("Hủy đặt vé", this);
    cancelBtn_->setStyleSheet(
        "QPushButton { "
        "  background: #b5c7d7; "
        "  color: white; "
        "  border: none; " 
        "  border-radius: 4px; "
        "  padding: 8px 15px; "
        "  font-size: 14px; "
        "  font-weight: 600; "
        "} "
        "QPushButton:hover { background: #8c9aa6ff; }"
    );

    backBtn_ = new QPushButton("Trang trước", this);
    backBtn_->setStyleSheet(
        "QPushButton { "
        "  background: white; "
        "  color: #608bc1; "
        "  border: 1px solid #608bc1; "
        "  border-radius: 4px; "
        "  padding: 7px 15px; "
        "  font-size: 14px; "
        "  font-weight: 600; "
        "} "
        "QPushButton:hover { background: #c2d1e3ff; }"
    );
    backBtn_->setVisible(false);

    nextBtn_ = new QPushButton("Trang sau", this);
    nextBtn_->setStyleSheet(
        "QPushButton { "
        "  background: #608bc1; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 8px 15px; "
        "  font-size: 14px; "
        "  font-weight: 600; "
        "} "
        "QPushButton:hover { background: #1565C0; }"
    );

    confirmBtn_ = new QPushButton("Xác nhận", this);
    confirmBtn_->setStyleSheet(
        "QPushButton { "
        "  background: #608bc1; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 8px 16px; "
        "  font-size: 14px; "
        "  font-weight: 600; "
        "} "
        "QPushButton:hover { background: #1565C0; }"
    );
    confirmBtn_->setVisible(false);

    footerLayout->addWidget(cancelBtn_);
    footerLayout->addStretch();
    footerLayout->addWidget(backBtn_);
    footerLayout->addWidget(nextBtn_);
    footerLayout->addWidget(confirmBtn_);

    connect(nextBtn_, &QPushButton::clicked, this, &BookingDialog::onNextClicked);
    connect(backBtn_, &QPushButton::clicked, this, &BookingDialog::onBackClicked);
    connect(confirmBtn_, &QPushButton::clicked, this, &BookingDialog::onConfirmClicked);
    connect(cancelBtn_, &QPushButton::clicked, this, &BookingDialog::reject);

    mainLayout->addWidget(footerWidget);
}

// ========== PROGRESS HEADER ==========
void BookingDialog::setupProgressHeader() {
    QWidget* header = new QWidget(this);
    header->setFixedHeight(60);
    header->setStyleSheet("background: white; border: none;");
    
    QHBoxLayout* layout = new QHBoxLayout(header);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 20, 0, 10);

    auto createLabel = [](const QString& text) {
        QLabel* lbl = new QLabel(text);
        lbl->setFixedSize(20, 20);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet(
            "border-radius: 10px; "
            "font-weight: bold; "
            "font-size: 14px; "
            "background-color: white; "
            "color: white;"
        );
        return lbl;
    };

    auto createLine = []() {
        QWidget* line = new QWidget;
        line->setFixedSize(120, 3);
        line->setStyleSheet("background-color: #FFFFFF;");
        return line;
    };

    step1Label_ = createLabel("1");
    line1_ = createLine();
    step2Label_ = createLabel("2");
    line2_ = createLine();
    step3Label_ = createLabel("3");

    layout->addWidget(step1Label_);
    layout->addWidget(line1_);
    layout->addWidget(step2Label_);
    layout->addWidget(line2_);
    layout->addWidget(step3Label_);

    this->layout()->addWidget(header);
}

void BookingDialog::updateStepIndicator() {
    int current = stackedWidget_->currentIndex();
    
    QString activeStyle = 
        "background-color: #74cc00; "
        "color: white; "
        "border: none; "
        "border-radius: 10px; "
        "font-weight: bold; "
        "font-size: 12.5px;";
    
    QString inactiveStyle = 
        "background-color: #cbdceb; "
        "color: white; "
        "border: none; "
        "border-radius: 10px; "
        "font-weight: bold; "
        "font-size: 12.5px;";
    
    QString lineActive = "background-color: #74cc00;";
    QString lineInactive = "background-color: #cbdceb;";

    step1Label_->setStyleSheet(inactiveStyle);
    step2Label_->setStyleSheet(inactiveStyle);
    step3Label_->setStyleSheet(inactiveStyle);
    line1_->setStyleSheet(lineInactive);
    line2_->setStyleSheet(lineInactive);

    if (current >= 0) step1Label_->setStyleSheet(activeStyle);
    if (current >= 1) {
        line1_->setStyleSheet(lineActive);
        step2Label_->setStyleSheet(activeStyle);
    }
    if (current >= 2) {
        line2_->setStyleSheet(lineActive);
        step3Label_->setStyleSheet(activeStyle);
    }

    backBtn_->setVisible(current >= 1);
    nextBtn_->setVisible(current < 2);
    confirmBtn_->setVisible(current == 2);
}

// ========== PAGE 1: FLIGHT INFO ==========
void BookingDialog::setupPage1_FlightInfo() {
    QWidget* page = new QWidget;
    page->setStyleSheet("background-color: #FFFFFF; border: none;");
    
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 20, 0, 20);
    layout->setSpacing(25);

    QLabel* title = new QLabel("Xác nhận thông tin chuyến bay");
    title->setStyleSheet(
        "font-size: 20px; "
        "font-weight: bold; "
        "color: #1565C0; "
        "background: transparent; "
        "border: none;"
    );
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QFrame* infoFrame = new QFrame;
    infoFrame->setObjectName("InfoFrame");

    infoFrame->setStyleSheet(
        "#InfoFrame { "
        "  background: white; "
        "  border: 1px solid #BFCFE0; "
        "  border-radius: 8px; "
        "}"
    );
    infoFrame->setMinimumHeight(280);
    
    QGridLayout* gridLayout = new QGridLayout(infoFrame);
    gridLayout->setVerticalSpacing(15);
    gridLayout->setHorizontalSpacing(30);
    gridLayout->setContentsMargins(20, 20, 20, 20);

    gridLayout->setColumnStretch(1, 1);

    auto addRow = [&](int row, QString label, QString value) {
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet(
            "font-weight: bold; "
            "color: #1565C0; "
            "font-size: 13px; "
            "background: none; "
            "border: none;"
        );
        
        QLabel* val = new QLabel(value);
        val->setStyleSheet(
            "color: #333; "
            "font-size: 13px; "
            "background: none; "
            "border: none;"
        );
        
        gridLayout->addWidget(lbl, row, 0, Qt::AlignLeft);
        gridLayout->addWidget(val, row, 1, Qt::AlignRight);
    };

    Route* route = flightManager_->findRouteById(flight_->getRouteId());

    addRow(0, "Lộ trình bay", QString::fromStdString(airportManager_->getDisplayName(route->getDepartureAirport()) + " → " + airportManager_->getDisplayName(route->getArrivalAirport())));
    addRow(1, "Hãng hàng không", QString::fromStdString(flight_->getAirline()));
    addRow(2, "Số hiệu chuyến bay", QString::fromStdString(flight_->getFlightNumber()));
    addRow(3, "Ngày khởi hành", QString::fromStdString(flight_->getDepartureDate()));
    addRow(4, "Giờ khởi hành", QString::fromStdString(flight_->getDepartureTime()));
    addRow(5, "Ngày hạ cánh", QString::fromStdString(flight_->getArrivalDate()));
    addRow(6, "Giờ hạ cánh", QString::fromStdString(flight_->getArrivalTime()));

    layout->addWidget(infoFrame);
    layout->addStretch();
    
    stackedWidget_->addWidget(page);
}

// ========== PAGE 2: PASSENGER INFO ==========
void BookingDialog::setupPage2_PassengerInfo() {
    QWidget* page = new QWidget;
    page->setStyleSheet("background-color: #FFFFFF; border: none;");
    
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 20, 0, 20);
    layout->setSpacing(25);

    QLabel* title = new QLabel("Nhập thông tin hành khách");
    title->setStyleSheet(
        "font-size: 20px; "
        "font-weight: bold; "
        "color: #1565C0; "
        "background: transparent; "
        "border: none;"
    );
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QWidget* formContainer = new QWidget;
    formContainer->setStyleSheet("background: transparent; border: none;");
    QFormLayout* form = new QFormLayout(formContainer);
    form->setSpacing(20);
    form->setLabelAlignment(Qt::AlignLeft);
    form->setContentsMargins(0, 10, 0, 10);

    QString inputStyle = 
        "QLineEdit { "
        "  border: 1px solid #BFCFE0; "
        "  border-radius: 4px; "
        "  padding: 10px; "
        "  font-size: 13px; "
        "  background: white; "
        "} "
        "QLineEdit:focus { "
        "  border: 2px solid #1976D2; "
        "  background: white; "
        "}";

    passengerIdEdit_ = new QLineEdit;
    passengerIdEdit_->setPlaceholderText("Nhập 9 hoặc 12 chữ số");
    passengerIdEdit_->setStyleSheet(inputStyle);
    passengerIdEdit_->setMinimumHeight(40);
    
    connect(passengerIdEdit_, &QLineEdit::editingFinished, this, [this, inputStyle]() {
        QString pid = passengerIdEdit_->text().trimmed();
        if (pid.length() != 9 && pid.length() != 12) return;
        
        Passenger* p = passengerManager_->findPassengerById(pid.toStdString());
        if (p) {
            passengerNameEdit_->setText(QString::fromStdString(p->getFullName()));
            dateOfBirthEdit_->setText(QString::fromStdString(p->getDateOfBirth()));
            passengerPhoneEdit_->setText(QString::fromStdString(p->getPhoneNumber()));
            
            passengerNameEdit_->setStyleSheet(
                "QLineEdit { background: #E8F5E9; border: 1px solid #BFCFE0; border-radius: 4px; padding: 10px; }"
            );
            
            QTimer::singleShot(1500, this, [this, inputStyle](){
                passengerNameEdit_->setStyleSheet(inputStyle);
            });
            std::cout << "[INFO] Auto-filled passenger: " << pid.toStdString() << std::endl;
        }
    });

    passengerNameEdit_ = new QLineEdit;
    passengerNameEdit_->setPlaceholderText("Nhập họ và tên");
    passengerNameEdit_->setStyleSheet(inputStyle);
    passengerNameEdit_->setMinimumHeight(40);

    dateOfBirthEdit_ = new QLineEdit;
    dateOfBirthEdit_->setPlaceholderText("DD/MM/YYYY");
    dateOfBirthEdit_->setStyleSheet(inputStyle);
    dateOfBirthEdit_->setMinimumHeight(40);
    
    passengerPhoneEdit_ = new QLineEdit;
    passengerPhoneEdit_->setPlaceholderText("Nhập số điện thoại");
    passengerPhoneEdit_->setStyleSheet(inputStyle);
    passengerPhoneEdit_->setMinimumHeight(40);

    QLabel* lblId = new QLabel("Căn cước công dân <span style='color:red'>*</span>");
    lblId->setStyleSheet("font-weight: bold; font-size: 13px; color: #1565C0; background: transparent; border: none;");
    form->addRow(lblId, passengerIdEdit_);

    QLabel* lblName = new QLabel("Họ và tên <span style='color:red'>*</span>");
    lblName->setStyleSheet("font-weight: bold; font-size: 13px; color: #1565C0; background: transparent; border: none;");
    form->addRow(lblName, passengerNameEdit_);

    QLabel* lblDob = new QLabel("Ngày sinh");
    lblDob->setStyleSheet("font-weight: bold; font-size: 13px; color: #1565C0; background: transparent; border: none;");
    form->addRow(lblDob, dateOfBirthEdit_);

    QLabel* lblPhone = new QLabel("Số điện thoại");
    lblPhone->setStyleSheet("font-weight: bold; font-size: 13px; color: #1565C0; background: transparent; border: none;");
    form->addRow(lblPhone, passengerPhoneEdit_);

    layout->addWidget(formContainer);
    layout->addStretch();
    
    stackedWidget_->addWidget(page);
}

// ========== PAGE 3: SEAT SELECTION ==========
void BookingDialog::setupPage3_SeatSelection() {
    QWidget* page = new QWidget;
    page->setStyleSheet("background-color: #FFFFFF; border: none;");
    
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 20, 0, 20);
    layout->setSpacing(15);

    QLabel* title = new QLabel("Chọn ghế ngồi");
    title->setStyleSheet(
        "font-size: 20px; "
        "font-weight: bold; "
        "color: #1565C0; "
        "background: transparent; "
        "border: none;"
    );
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QFrame* classFrame = new QFrame;
    classFrame->setObjectName("ClassFrame");
    classFrame->setStyleSheet(
        "#ClassFrame { "
        "  background: white; "
        "  border: 1px solid #608bc1; "
        "  border-radius: 8px; "
        "}"
    );
    
    QHBoxLayout* classLayout = new QHBoxLayout(classFrame);
    classLayout->setSpacing(20);
    classLayout->setContentsMargins(20, 12, 20, 12);
    
    QLabel* lblClass = new QLabel("Chọn hạng vé");
    lblClass->setStyleSheet("font-weight: bold; font-size: 14px; color: #1565C0; background: transparent; border: none;");
    
    economyRadio_ = new QRadioButton("Hạng phổ thông");
    businessRadio_ = new QRadioButton("Hạng thương gia");
    economyRadio_->setChecked(true);
    
    QString radioStyle = 
        "QRadioButton { font-size: 13px; color: #608bc1; background: transparent; spacing: 5px; } "
        "QRadioButton::indicator { width: 12px; height: 12px; } "
        "QRadioButton::indicator:unchecked { border: 2px solid #608bc1; border-radius: 6px; background: white; } "
        "QRadioButton::indicator:checked { border: 2px solid #608bc1; border-radius: 6px; background: #74cc00; }";
    
    economyRadio_->setStyleSheet(radioStyle);
    businessRadio_->setStyleSheet(radioStyle);

    QButtonGroup* grp = new QButtonGroup(page);
    grp->addButton(economyRadio_);
    grp->addButton(businessRadio_);

    fareLabel_ = new QLabel();
    fareLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #74cc00; background: transparent; border: none;");
    
    classLayout->addWidget(lblClass);
    classLayout->addWidget(economyRadio_);
    classLayout->addWidget(businessRadio_);
    classLayout->addStretch();
    classLayout->addWidget(fareLabel_);
    
    layout->addWidget(classFrame);

    QFrame* promoFrame = new QFrame;
    promoFrame->setStyleSheet("background: white; border: 1px solid #608bc1; border-radius: 8px;");

    QHBoxLayout* promoLayout = new QHBoxLayout(promoFrame);
    promoLayout->setContentsMargins(20, 12, 20, 12);

    QLabel* lblPromo = new QLabel("Mã khuyến mãi");
    lblPromo->setStyleSheet("font-weight: bold; font-size: 14px; color: #1565C0; border: none; background: transparent;");

    promoCodeEdit_ = new QLineEdit;
    promoCodeEdit_->setPlaceholderText("Nhập mã (VD: CHAOHE25)");
    promoCodeEdit_->setFixedWidth(220);

    applyPromoBtn_ = new QPushButton("Áp dụng");
    applyPromoBtn_->setStyleSheet(
        "QPushButton { background: #74cc00; color: white; border: none; border-radius: 4px; padding: 8px 12px; font-weight: 600; }"
        "QPushButton:hover { background: #5ba300; }"
    );

    discountLabel_ = new QLabel("");
    discountLabel_->setStyleSheet("font-size: 13px; font-weight: bold; color: #27C93F; background: transparent; border: none;");

    promoLayout->addWidget(lblPromo);
    promoLayout->addWidget(promoCodeEdit_);
    promoLayout->addWidget(applyPromoBtn_);
    promoLayout->addWidget(discountLabel_);

    layout->addWidget(promoFrame);
    connect(applyPromoBtn_, &QPushButton::clicked, this, &BookingDialog::onApplyPromoClicked);

    QFrame* seatMapFrame = new QFrame;
    seatMapFrame->setObjectName("SeatMapFrame");
    seatMapFrame->setStyleSheet(
        "#SeatMapFrame { "
        "  background: white; "
        "  border: 1px solid #608bc1; "
        "  border-radius: 8px; "
        "}"
    );

    QVBoxLayout* frameLayout = new QVBoxLayout(seatMapFrame);
    frameLayout->setContentsMargins(10, 10, 10, 10);
    frameLayout->setSpacing(0);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet("background: transparent; border: none;");
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    seatMapContainer_ = new QWidget;
    seatMapContainer_->setStyleSheet("background: transparent; border: none;");
    
    seatMapLayout_ = new QGridLayout(seatMapContainer_);
    seatMapLayout_->setSpacing(4);
    seatMapLayout_->setAlignment(Qt::AlignCenter);
    seatMapLayout_->setContentsMargins(5, 5, 5, 5);

    scrollArea->setWidget(seatMapContainer_);
    frameLayout->addWidget(scrollArea);
    
    layout->addWidget(seatMapFrame, 1);
    QHBoxLayout* legendLayout = new QHBoxLayout;
    legendLayout->setAlignment(Qt::AlignCenter);
    legendLayout->setSpacing(15);
    
    auto addLegend = [&](QString color, QString text) {
        QHBoxLayout* itemLayout = new QHBoxLayout;
        itemLayout->setSpacing(5);
        QLabel* box = new QLabel;
        box->setFixedSize(14, 14);
        box->setStyleSheet("background: " + color + "; border: none; border-radius: 2px;");
        QLabel* textLbl = new QLabel(text);
        textLbl->setStyleSheet("font-size: 11px; color: #555; background: transparent; border: none;");
        itemLayout->addWidget(box);
        itemLayout->addWidget(textLbl);
        legendLayout->addLayout(itemLayout);
    };
    
    addLegend("#27C93F", "Hạng phổ thông");
    addLegend("#FFBD2E", "Hạng thương gia");
    addLegend("#FF5F57", "Không thể chọn");
    addLegend("#D3D3D3", "Đã đặt");

    layout->addLayout(legendLayout);

    selectedSeatDisplayLabel_ = new QLabel("Hiện tại chưa chọn ghế!");
    selectedSeatDisplayLabel_->setAlignment(Qt::AlignCenter);
    selectedSeatDisplayLabel_->setStyleSheet("font-size: 13px; color: #1565C0; font-weight: bold; background: transparent; border: none; margin: 5px 0;");
    layout->addWidget(selectedSeatDisplayLabel_);

    connect(economyRadio_, &QRadioButton::toggled, this, &BookingDialog::onClassChanged);
    connect(businessRadio_, &QRadioButton::toggled, this, &BookingDialog::onClassChanged);

    stackedWidget_->addWidget(page);
}

// ========== EVENT HANDLERS ==========
void BookingDialog::onClassChanged() {
    updateFareDisplay();
    renderSeatMap();
    selectedSeatId_.clear();
    selectedSeatDisplayLabel_->setText("Hiện tại chưa chọn ghế!");
    
    discountLabel_->setText("");
    promoCodeEdit_->clear();
}

void BookingDialog::updateFareDisplay() {
    if (!flight_) return;
    
    int fare = economyRadio_->isChecked() ? 
        flight_->getFareEconomy() : flight_->getFareBusiness();
    
    fareLabel_->setText(QString("%L1 VNĐ").arg(fare));
    
    QString color = economyRadio_->isChecked() ? "#00bf63" : "#ff751f";
    fareLabel_->setStyleSheet(
        QString("font-size: 16px; font-weight: bold; color: %1; background: transparent; border: none;").arg(color)
    );
}

void BookingDialog::renderSeatMap()
{
    QLayoutItem* item;
    while ((item = seatMapLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    SeatManager* sm = flightManager_->getSeatManager();
    if (!sm->loadSeatMapFor(flight_)) {
        QLabel* errorLabel = new QLabel("Không thể tải sơ đồ ghế");
        errorLabel->setStyleSheet("color: red; font-size: 14px; background: transparent; border: none;");
        errorLabel->setAlignment(Qt::AlignCenter);
        seatMapLayout_->addWidget(errorLabel, 0, 0);
        return;
    }
    
    auto* seatMap = sm->getActiveSeatMap();
    int cols = sm->getSeatColumns();

    BookingClass currentClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;

    for (int col = 0; col < cols; ++col) {
        QLabel* header = new QLabel(QString(QChar('A' + col)));
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet(
            "font-size: 11px; "
            "font-weight: bold; "
            "color: #333; "
            "background: transparent; "
            "border: none;"
        );
        header->setFixedSize(26, 18);
        seatMapLayout_->addWidget(header, 0, col + 1);
    }

    int currentRow = -1;
    int layoutRow = 1;

    for (Seat* seat : *seatMap) {
        int r, c;
        std::tie(r, c) = seat->getCoordinates();

        if (r != currentRow) {
            currentRow = r;
            QLabel* rLbl = new QLabel(QString::number(r + 1).rightJustified(2, '0'));
            rLbl->setFixedSize(18, 26);
            rLbl->setAlignment(Qt::AlignCenter);
            rLbl->setStyleSheet(
                "font-size: 10px; "
                "font-weight: bold; "
                "color: #333; "
                "background: transparent; "
                "border: none;"
            );
            seatMapLayout_->addWidget(rLbl, layoutRow, 0);
        }

        QPushButton* btn = new QPushButton();
        btn->setFixedSize(26, 26);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFlat(true);

        QString style;
        bool enable = false;
        bool matchClass = (currentClass == BookingClass::Economy && seat->getType() == SeatType::Economy) ||
                          (currentClass == BookingClass::Business && seat->getType() == SeatType::Business);

        if (seat->getStatus() == SeatStatus::Booked) {
            style = BOOKED_STYLE;
            btn->setToolTip(QString("Ghế %1 (Đã đặt)").arg(QString::fromStdString(seat->getId())));
        } else if (!matchClass) {
            style = LOCKED_STYLE;
            btn->setToolTip("Khác hạng vé");
        } else {
            enable = true;
            style = (seat->getType() == SeatType::Business) ? BUSINESS_AVAILABLE_STYLE : ECONOMY_AVAILABLE_STYLE;
            btn->setToolTip(QString("Ghế %1 (Trống)").arg(QString::fromStdString(seat->getId())));
        }

        btn->setStyleSheet(style);
        btn->setEnabled(enable);
        btn->setProperty("seatId", QString::fromStdString(seat->getId()));
        btn->setProperty("baseStyle", style);

        if (enable) {
            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                QString sId = btn->property("seatId").toString();

                for (int i = 0; i < seatMapLayout_->count(); ++i) {
                    QPushButton* b = qobject_cast<QPushButton*>(seatMapLayout_->itemAt(i)->widget());
                    if (b && b->property("seatId").isValid()) {
                        b->setStyleSheet(b->property("baseStyle").toString());
                    }
                }

                QString selectedStyle = btn->property("baseStyle").toString() + SELECTED_STYLE_SUFFIX;
                btn->setStyleSheet(selectedStyle);

                selectedSeatId_ = sId;
                selectedSeatDisplayLabel_->setText(
                    QString("Bạn đã chọn ghế: <b style='color:#1565C0; font-size:14px;'>%1</b>").arg(selectedSeatId_)
                );
            });
        }

        seatMapLayout_->addWidget(btn, layoutRow, c + 1);

        if (c == cols - 1) layoutRow++;
    }
}

void BookingDialog::onNextClicked() {
    int current = stackedWidget_->currentIndex();
    
    if (current == 1) {
        if (passengerIdEdit_->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Thiếu thông tin", "Vui lòng nhập CCCD/CMND.");
            passengerIdEdit_->setFocus();
            return;
        }
        QRegularExpression idRegex(R"(^\d{9}$|^\d{12}$)");
        if (!idRegex.match(passengerIdEdit_->text().trimmed()).hasMatch()) {
            QMessageBox::warning(this, "Lỗi", "CCCD phải là 9 hoặc 12 số.");
            passengerIdEdit_->setFocus();
            return;
        }
    }

    if (current < 2) {
        stackedWidget_->setCurrentIndex(current + 1);
        updateStepIndicator();
        
        if (current + 1 == 2) {
            updateFareDisplay();
            renderSeatMap();
        }
    }
}

void BookingDialog::onBackClicked() {
    int current = stackedWidget_->currentIndex();
    
    if (current == 0) {
        reject();
    } else {
        stackedWidget_->setCurrentIndex(current - 1);
        updateStepIndicator();
    }
}

void BookingDialog::onCancelClicked() {
    reject();
}

void BookingDialog::onConfirmClicked() {
    if (selectedSeatId_.isEmpty()) {
        QMessageBox::warning(this, "Chưa chọn ghế", "Vui lòng chọn một ghế ngồi trên bản đồ.");
        return;
    }

    confirmBtn_->setEnabled(false);

    QString passengerId = passengerIdEdit_->text().trimmed();
    QString fullName = passengerNameEdit_->text().trimmed();
    QString dob = dateOfBirthEdit_->text().trimmed();
    QString phone = passengerPhoneEdit_->text().trimmed();

    if (fullName.isEmpty()) {
        auto reply = QMessageBox::question(this, "Xác nhận",
            "Bạn chưa nhập họ tên hành khách.\nTiếp tục dùng tên mặc định?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            passengerNameEdit_->setFocus();
            confirmBtn_->setEnabled(true);
            return;
        }
        fullName = "Khách hàng " + passengerId;
    }

    if (dob.isEmpty()) dob = "01/01/1990";
    
    Passenger* passenger = passengerManager_->createOrUpdatePassenger(
        passengerId.toStdString(),
        fullName.toStdString(),
        dob.toStdString(),
        phone.toStdString()
    );

    if (!passenger) {
        QMessageBox::critical(this, "Lỗi", "Không thể lưu thông tin hành khách.");
        confirmBtn_->setEnabled(true);
        return;
    }
    passengerManager_->saveAllData();

    SeatManager* seatManager = flightManager_->getSeatManager();
    if (!seatManager) {
        QMessageBox::critical(this, "Lỗi", "Lỗi hệ thống (SeatManager null).");
        confirmBtn_->setEnabled(true);
        return;
    }

    // Chọn ghế trước khi tạo booking (createNewBooking sẽ gọi confirmSelection)
    if (!seatManager->selectSeat(selectedSeatId_.toStdString())) {
        QMessageBox::critical(this, "Lỗi", "Không thể chọn ghế này. Vui lòng chọn ghế khác.");
        renderSeatMap();
        confirmBtn_->setEnabled(true);
        return;
    }

    Account* currentUser = accountManager_->getCurrentUser();
    std::string agentId = currentUser ? currentUser->getId() : "unknown";
    
    BookingClass bookingClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;
    int baseFare = (bookingClass == BookingClass::Economy) ? flight_->getFareEconomy() : flight_->getFareBusiness();
    int finalFare = bookingManager_->applyPromotion( promoCodeEdit_->text().trimmed().toStdString(), baseFare);

    // Sử dụng createNewBooking để tự động cập nhật availableSeats
    // Hàm này sẽ gọi confirmSelection() để book ghế
    Booking* newBooking = bookingManager_->createNewBooking(
        *flightManager_,
        flight_->getFlightId(),
        agentId,
        passengerId.toStdString(),
        bookingClass,
        finalFare,
        *seatManager
    );

    if (!newBooking) {
        QMessageBox::critical(this, "Lỗi", "Không thể tạo và lưu booking. Vui lòng thử lại.");
        seatManager->cancelSelection();
        confirmBtn_->setEnabled(true);
        return;
    }

    // createNewBooking đã tự động lưu booking vào file rồi
    // Bây giờ chỉ cần lưu thay đổi số ghế trống vào file flights.txt
    if (!flightManager_->saveAllData()) {
        std::cerr << "[WARN] Failed to save flight data changes." << std::endl;
    }

    QMessageBox::information(this, "Thành công", 
        QString("Đặt vé thành công!\n\n"
               "Mã đặt chỗ: %1\n"
               "Khách hàng: %2\n"
               "Ghế: %3\n"
               "Tổng tiền: %L4 VNĐ")
            .arg(QString::fromStdString(newBooking->getBookingId()))
            .arg(fullName)
            .arg(selectedSeatId_)
            .arg(baseFare));

    accept();
}

void BookingDialog::onApplyPromoClicked() {
    int baseFare = economyRadio_->isChecked() ? 
        flight_->getFareEconomy() : flight_->getFareBusiness();
    
    int finalFare = bookingManager_->applyPromotion(
        promoCodeEdit_->text().trimmed().toStdString(), 
        baseFare
    );
    
    if (finalFare < baseFare) {
        fareLabel_->setText(QString("%L1 VNĐ").arg(finalFare));
        discountLabel_->setText(QString("Giảm %L1 VNĐ").arg(baseFare - finalFare));
    } else {
        QMessageBox::warning(this, "Lỗi", "Mã không hợp lệ hoặc đã hết hạn!");
        discountLabel_->clear();
    }
}

// ========== GETTERS ==========
QString BookingDialog::getPassengerId() const { return passengerIdEdit_->text().trimmed(); }
QString BookingDialog::getPassengerName() const { return passengerNameEdit_->text().trimmed(); }
QString BookingDialog::getDateOfBirth() const { return dateOfBirthEdit_->text().trimmed(); }
QString BookingDialog::getPassengerPhone() const { return passengerPhoneEdit_->text().trimmed(); }
BookingClass BookingDialog::getSelectedClass() const { return economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business; }
QString BookingDialog::getSelectedSeatId() const { return selectedSeatId_; }