#include "ChangeBookingDialog.h"
#include "entities/Booking.h"
#include "entities/Flight.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/SeatManager.h"
#include "entities/Seat.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QMessageBox>
#include <QScrollArea>
#include <QButtonGroup>
#include <QFrame>
#include <QStackedWidget>
#include <QDebug>

// Style base cho nút ghế ngồi (Lấy từ BookingDialog.cpp)
const QString ChangeBookingDialog::SEAT_BASE_STYLE = 
    "QPushButton { border: none; border-radius: 3px; }"; 

// Style cho các trạng thái ghế ngồi (Lấy từ BookingDialog.cpp)
const QString ChangeBookingDialog::BUSINESS_AVAILABLE_STYLE = ChangeBookingDialog::SEAT_BASE_STYLE + 
    "QPushButton { background: #FFBD2E; } QPushButton:hover { background: #FFC700; border: 2px solid #FF6B35; }";

const QString ChangeBookingDialog::ECONOMY_AVAILABLE_STYLE = ChangeBookingDialog::SEAT_BASE_STYLE + 
    "QPushButton { background: #27C93F; } QPushButton:hover { background: #3ad94a; border: 2px solid #FF6B35; }";

const QString ChangeBookingDialog::BOOKED_STYLE = ChangeBookingDialog::SEAT_BASE_STYLE + 
    "QPushButton { background: #D3D3D3; }";

const QString ChangeBookingDialog::LOCKED_STYLE = ChangeBookingDialog::SEAT_BASE_STYLE + 
    "QPushButton { background: #FF5F57; }";

const QString ChangeBookingDialog::SELECTED_STYLE_SUFFIX = " border: 2px solid #FF6B35;";


// Helper function để format tiền tệ Việt Nam
namespace {
QString dotFormat(int v) {
    QString s = QString::number(v);
    QString result;
    int count = 0;
    
    for (int i = s.length() - 1; i >= 0; --i) {
        if (count == 3) {
            result.prepend('.');
            count = 0;
        }
        result.prepend(s[i]);
        count++;
    }
    
    return result;
}
}

ChangeBookingDialog::ChangeBookingDialog(
        Booking* currentBooking,
        BookingManager* bookingManager,
        FlightManager* flightManager,
        QWidget *parent)
    : QDialog(parent),
      currentBooking_(currentBooking),
      bookingManager_(bookingManager),
      flightManager_(flightManager),
      currentFlight_(nullptr),
      selectedNewFlight_(nullptr),
      currentStep_(1),
      step1Label_(nullptr),
      line1_(nullptr),
      step2Label_(nullptr)
{
    setWindowTitle("Đổi đặt chỗ");
    // Kích thước cố định theo BookingDialog
    setFixedSize(700, 600); 
    setStyleSheet("QDialog { background-color: #FFFFFF; }");

    currentFlight_ = flightManager_->findFlightById(currentBooking_->getFlightId());

    setupUi();
    setupConnections();
    showStep(1);
}

// === PHẦN HEADER DÙNG CHO CHANGE BOOKING (2 BƯỚC) ===
void ChangeBookingDialog::setupProgressHeader() {
    QWidget* header = new QWidget(this);
    header->setFixedHeight(60);
    header->setStyleSheet("background: white; border: none;");
    
    QHBoxLayout* layout = new QHBoxLayout(header);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 20, 0, 10);

    // Dùng style label của BookingDialog
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

    // Dùng style line của BookingDialog
    auto createLine = []() {
        QWidget* line = new QWidget;
        line->setFixedSize(120, 3);
        line->setStyleSheet("background-color: #FFFFFF;");
        return line;
    };

    step1Label_ = createLabel("1");
    line1_ = createLine();
    step2Label_ = createLabel("2");

    layout->addWidget(step1Label_);
    layout->addWidget(line1_);
    layout->addWidget(step2Label_);

    this->layout()->addWidget(header);
}

// Hàm này được gọi bởi showStep()
void ChangeBookingDialog::updateStepIndicator() {
    int current = currentStep_; // Sử dụng biến currentStep_
    
    // Style sheets giống BookingDialog
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
    line1_->setStyleSheet(lineInactive);

    if (current >= 1) step1Label_->setStyleSheet(activeStyle);
    if (current >= 2) {
        line1_->setStyleSheet(lineActive);
        step2Label_->setStyleSheet(activeStyle);
    }
}

void ChangeBookingDialog::setupUi()
{
    // Cài đặt nền Dialog để khớp với BookingDialog
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#FFFFFF"));
    setAutoFillBackground(true);
    setPalette(pal);

    auto* mainLayout = new QVBoxLayout(this);
    // Margins cố định theo BookingDialog
    mainLayout->setContentsMargins(50, 0, 50, 20); 
    mainLayout->setSpacing(0);
    
    setupProgressHeader(); // Header 2 bước

    // QStackedWidget cho 2 bước
    stackedWidget_ = new QStackedWidget(this);
    stackedWidget_->setStyleSheet("QStackedWidget { background-color: #FFFFFF; border: none; }");
    
    // Tạo 2 trang: Chọn chuyến bay và Chọn ghế
    setupPage1_FlightSearch();
    setupPage2_SeatSelection();
    
    mainLayout->addWidget(stackedWidget_);

    // --- FOOTER (Tái tạo layout và style của BookingDialog) ---
    auto *footerWidget = new QWidget(this);
    footerWidget->setStyleSheet("background-color: #FFFFFF; border: none;");
    auto *footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(0, 0, 0, 0); 

    // Styles nút bấm (Lấy từ BookingDialog.cpp)
    QString cancelBtnStyle = 
        "QPushButton { background: #b5c7d7; color: white; border: none; border-radius: 4px; padding: 8px 15px; font-size: 14px; font-weight: 600; } QPushButton:hover { background: #8c9aa6ff; }";
    QString backBtnStyle = 
        "QPushButton { background: white; color: #608bc1; border: 1px solid #608bc1; border-radius: 4px; padding: 7px 15px; font-size: 14px; font-weight: 600; } QPushButton:hover { background: #c2d1e3ff; }";
    QString mainBtnStyle = 
        "QPushButton { background: #608bc1; color: white; border: none; border-radius: 4px; padding: 8px 16px; font-size: 14px; font-weight: 600; } QPushButton:hover { background: #1565C0; } QPushButton:disabled { background: #cbdceb; }";


    cancelBtn_ = new QPushButton("Hủy đổi vé", this); // Đổi tên thành Hủy đổi vé
    cancelBtn_->setStyleSheet(cancelBtnStyle);
    connect(cancelBtn_, &QPushButton::clicked, this, &QDialog::reject);

    backBtn_ = new QPushButton("Trang trước", this);
    backBtn_->setStyleSheet(backBtnStyle);
    backBtn_->setVisible(false); // Sẽ được điều chỉnh trong showStep

    step1NextBtn_ = new QPushButton("Trang sau", this);
    step1NextBtn_->setStyleSheet(mainBtnStyle);
    step1NextBtn_->setEnabled(false); // Khác BookingDialog: Mặc định tắt cho đến khi tìm thấy chuyến bay
    
    confirmBtn_ = new QPushButton("Xác nhận đổi vé", this); // Đổi tên thành Xác nhận đổi vé
    confirmBtn_->setStyleSheet(mainBtnStyle);
    confirmBtn_->setEnabled(false); // Khác BookingDialog: Mặc định tắt cho đến khi chọn ghế
    confirmBtn_->setVisible(false); // Sẽ được điều chỉnh trong showStep

    // --- SẮP XẾP LAYOUT ---
    footerLayout->addWidget(cancelBtn_);
    footerLayout->addStretch();
    footerLayout->addWidget(backBtn_);
    footerLayout->addWidget(step1NextBtn_);
    footerLayout->addWidget(confirmBtn_);

    mainLayout->addWidget(footerWidget);
}

// === PAGE 1: CHỌN CHUYẾN BAY MỚI (Kết hợp Search và Info) ===
void ChangeBookingDialog::setupPage1_FlightSearch()
{
    step1Container_ = new QWidget;
    step1Container_->setStyleSheet("background-color: #FFFFFF; border: none;");
    
    QVBoxLayout* s1 = new QVBoxLayout(step1Container_);
    s1->setContentsMargins(0, 20, 0, 20);
    s1->setSpacing(25);

    QLabel* title1 = new QLabel("Chọn chuyến bay mới");
    title1->setStyleSheet(
        "font-size: 20px; "
        "font-weight: bold; "
        "color: #1565C0; "
        "background: transparent; "
        "border: none;"
    );
    title1->setAlignment(Qt::AlignCenter);
    s1->addWidget(title1);

    // --- KHU VỰC TÌM KIẾM ---
    QHBoxLayout* inputRowWrapper = new QHBoxLayout();

    QLabel* lblId = new QLabel("Nhập mã chuyến bay:");
    lblId->setStyleSheet("font-weight: bold; font-size: 13px; color: #1565C0; background: transparent; border: none;");
    
    // Style Input theo BookingDialog
    QString inputStyle = 
        "QLineEdit { border: 1px solid #BFCFE0; border-radius: 4px; padding: 10px; font-size: 13px; background: white; } "
        "QLineEdit:focus { border: 2px solid #1976D2; background: white; }";
        
    flightIdEdit_ = new QLineEdit();
    flightIdEdit_->setPlaceholderText("FI-XXXXX");
    flightIdEdit_->setStyleSheet(inputStyle);
    flightIdEdit_->setMinimumHeight(40);
    
    searchBtn_ = new QPushButton("Tìm kiếm");
    searchBtn_->setMinimumHeight(40);
    searchBtn_->setMinimumWidth(110);
    // Style nút theo mainBtnStyle của BookingDialog (màu xanh đậm)
    searchBtn_->setStyleSheet(
        "QPushButton { background: #608bc1; color: white; border: none; border-radius: 4px; padding: 8px 16px; font-size: 14px; font-weight: 600; } "
        "QPushButton:hover { background: #1565C0; }"
    );

    inputRowWrapper->addWidget(lblId);
    inputRowWrapper->addWidget(flightIdEdit_);
    inputRowWrapper->addWidget(searchBtn_);
    s1->addLayout(inputRowWrapper);
    s1->addSpacing(15);
    
    // --- KHUNG HIỂN THỊ THÔNG TIN (Giống Page 1 BookingDialog) ---
    QFrame* infoFrame = new QFrame();
    infoFrame->setObjectName("InfoFrame");
    infoFrame->setStyleSheet(
        "#InfoFrame { "
        "  background: white; "
        "  border: 1px solid #BFCFE0; " // Viền nhạt
        "  border-radius: 8px; "
        "}"
    );
    infoFrame->setMinimumHeight(280);
    
    QGridLayout* infoGrid = new QGridLayout(infoFrame);
    infoGrid->setVerticalSpacing(15);
    infoGrid->setHorizontalSpacing(30);
    infoGrid->setContentsMargins(20, 20, 20, 20);
    infoGrid->setColumnStretch(1, 1); 

    auto addRow = [&](int row, QString label, QString value) {
        QLabel* lbl = new QLabel(label);
        // Style label theo BookingDialog
        lbl->setStyleSheet("font-weight: bold; color: #1565C0; font-size: 13px; background: none; border: none;");
        
        QLabel* val = new QLabel(value);
        val->setObjectName(QString("finfo_%1").arg(row));
        // Style value theo BookingDialog
        val->setStyleSheet("color: #333; font-size: 13px; background: none; border: none;");
        
        infoGrid->addWidget(lbl, row, 0, Qt::AlignLeft);
        infoGrid->addWidget(val, row, 1, Qt::AlignRight); 
    };

    addRow(0, "Lộ trình bay", "-");
    addRow(1, "Hãng hàng không", "-");
    addRow(2, "Số hiệu chuyến bay", "-");
    addRow(3, "Ngày khởi hành", "-");
    addRow(4, "Giờ khởi hành", "-");
    addRow(5, "Ngày hạ cánh", "-");
    addRow(6, "Giờ hạ cánh", "-");

    s1->addWidget(infoFrame);
    s1->addStretch();
    
    stackedWidget_->addWidget(step1Container_);
}


// === PAGE 2: CHỌN GHẾ NGỒI (Giống Page 3 BookingDialog) ===
void ChangeBookingDialog::setupPage2_SeatSelection()
{
    step2Container_ = new QWidget;
    step2Container_->setStyleSheet("background-color: #FFFFFF; border: none;");
    
    QVBoxLayout* s2 = new QVBoxLayout(step2Container_);
    s2->setContentsMargins(0, 20, 0, 20);
    s2->setSpacing(15);

    // --- 1. TIÊU ĐỀ ---
    QLabel* title2 = new QLabel("Chọn ghế ngồi");
    title2->setStyleSheet(
        "font-size: 20px; "
        "font-weight: bold; "
        "color: #1565C0; "
        "background: transparent; "
        "border: none;"
    );
    title2->setAlignment(Qt::AlignCenter);
    s2->addWidget(title2);

    // --- 2. KHUNG CHỌN HẠNG VÉ (Giống BookingDialog) ---
    QFrame* classFrame = new QFrame;
    classFrame->setObjectName("ClassFrame");
    classFrame->setStyleSheet(
        "#ClassFrame { "
        "  background: white; "
        "  border: 1px solid #608bc1; " // Viền xanh
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
    
    // Style Radio Button (Lấy từ BookingDialog)
    QString radioStyle = 
        "QRadioButton { font-size: 13px; color: #608bc1; background: transparent; spacing: 5px; } "
        "QRadioButton::indicator { width: 12px; height: 12px; } "
        "QRadioButton::indicator:unchecked { border: 2px solid #608bc1; border-radius: 6px; background: white; } "
        "QRadioButton::indicator:checked { border: 2px solid #608bc1; border-radius: 6px; background: #74cc00; }";
    
    economyRadio_->setStyleSheet(radioStyle);
    businessRadio_->setStyleSheet(radioStyle);

    QButtonGroup* grp = new QButtonGroup(step2Container_);
    grp->addButton(economyRadio_);
    grp->addButton(businessRadio_);

    fareLabel_ = new QLabel();
    fareLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #74cc00; background: transparent; border: none;");
    
    classLayout->addWidget(lblClass);
    classLayout->addWidget(economyRadio_);
    classLayout->addWidget(businessRadio_);
    classLayout->addStretch();
    classLayout->addWidget(fareLabel_);
    s2->addWidget(classFrame);

    // --- 3. KHUNG SƠ ĐỒ GHẾ (Tái tạo cấu trúc BookingDialog) ---
    QFrame* seatMapFrame = new QFrame;
    seatMapFrame->setObjectName("SeatMapFrame");
    seatMapFrame->setStyleSheet(
        "#SeatMapFrame { "
        "  background: white; "
        "  border: 1px solid #608bc1; " // Viền xanh
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
    
    s2->addWidget(seatMapFrame, 1);

    // --- 4. CHÚ THÍCH (LEGEND - Lấy màu và font từ BookingDialog) ---
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
    addLegend("#FF5F57", "Không thể chọn"); // Locked
    addLegend("#D3D3D3", "Đã đặt");

    s2->addLayout(legendLayout);

    // --- 5. THÔNG BÁO GHẾ ĐANG CHỌN & CHÊNH LỆCH GIÁ ---
    selectedSeatDisplayLabel_ = new QLabel("Hiện tại chưa chọn ghế!");
    selectedSeatDisplayLabel_->setAlignment(Qt::AlignCenter);
    selectedSeatDisplayLabel_->setStyleSheet("font-size: 13px; color: #1565C0; font-weight: bold; background: transparent; border: none; margin: 5px 0;");
    s2->addWidget(selectedSeatDisplayLabel_);

    priceDiffLabel_ = new QLabel("");
    priceDiffLabel_->setAlignment(Qt::AlignCenter);
    priceDiffLabel_->setStyleSheet("font-size: 14px; font-weight: 600;");
    s2->addWidget(priceDiffLabel_);
    s2->addStretch();
    
    stackedWidget_->addWidget(step2Container_);
}

void ChangeBookingDialog::setupConnections()
{
    // Giữ nguyên logic connections
    connect(searchBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSearchByIdClicked);
    connect(step1NextBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSelectFoundFlight);

    connect(backBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onBackToStep1);
    connect(economyRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
    connect(businessRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
    connect(confirmBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onConfirmClicked);
}

// Hàm này được gọi khi bấm Trang sau (Bước 1 -> Bước 2)
void ChangeBookingDialog::onSelectFoundFlight()
{
    if (!selectedNewFlight_) return;

    // Reset radio button và cập nhật UI
    economyRadio_->setChecked(true);
    updateFareDisplay(); // Cập nhật giá vé
    renderSeatMap();

    showStep(2);
}

// Thay đổi logic của showStep để quản lý stackedWidget và nút bấm
void ChangeBookingDialog::showStep(int step)
{
    currentStep_ = step;
    
    // Cập nhật Step Indicator
    updateStepIndicator();
    
    // Cập nhật Stacked Widget
    stackedWidget_->setCurrentIndex(step - 1);

    // Cập nhật nút bấm
    backBtn_->setVisible(step > 1);
    step1NextBtn_->setVisible(step < 2); // Chỉ hiện ở Bước 1
    confirmBtn_->setVisible(step == 2); // Chỉ hiện ở Bước 2
    
    // Đảm bảo nút Xác nhận luôn tắt ở Bước 2 cho đến khi chọn ghế
    if (step == 2) {
        confirmBtn_->setEnabled(!selectedSeatId_.isEmpty());
    }
}

void ChangeBookingDialog::onBackToStep1()
{
    // Logic quay lại bước 1
    showStep(1);
}

// Hàm này được gọi khi radio button thay đổi
void ChangeBookingDialog::onClassChanged()
{
    selectedSeatId_.clear();
    selectedSeatDisplayLabel_->setText("Hiện tại chưa chọn ghế!");
    confirmBtn_->setEnabled(false); // Vô hiệu hóa nút Xác nhận
    
    updateFareDisplay(); // Cập nhật giá vé (mới)
    renderSeatMap();     // Tải lại sơ đồ ghế cho hạng vé mới
    updatePriceDifference(); // Cập nhật chênh lệch giá
}

// Cập nhật hiển thị giá vé mới
void ChangeBookingDialog::updateFareDisplay() {
    if (!selectedNewFlight_) return;
    
    int fare = economyRadio_->isChecked() ? 
        selectedNewFlight_->getFareEconomy() : selectedNewFlight_->getFareBusiness();
    
    fareLabel_->setText(QString("%L1 VNĐ").arg(fare));
    
    // Style color theo BookingDialog (Economy: #00bf63, Business: #ff751f)
    QString color = economyRadio_->isChecked() ? "#00bf63" : "#ff751f"; // Dùng màu #74cc00 cho Economy checked
    fareLabel_->setStyleSheet(
        QString("font-size: 16px; font-weight: bold; color: %1; background: transparent; border: none;").arg(color)
    );
}

void ChangeBookingDialog::renderSeatMap()
{
    // Giữ nguyên logic render seat map
    QLayoutItem* item;
    while ((item = seatMapLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (!selectedNewFlight_) return;

    SeatManager* sm = flightManager_->getSeatManager();
    if (!sm) return;

    if (!sm->loadSeatMapFor(selectedNewFlight_)) {
        QLabel* err = new QLabel("Không thể tải sơ đồ ghế");
        err->setStyleSheet("color: red; font-size: 14px; background: transparent; border: none;");
        err->setAlignment(Qt::AlignCenter);
        seatMapLayout_->addWidget(err, 0, 0);
        return;
    }

    auto* seatMap = sm->getActiveSeatMap();
    if (!seatMap) return;

    int cols = sm->getSeatColumns();
    BookingClass currentClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;

    // Add column headers (A, B, C, ...)
    for (int col = 0; col < cols; ++col) {
        QLabel* header = new QLabel(QString(QChar('A' + col)));
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet("font-weight:700; color:#333;");
        seatMapLayout_->addWidget(header, 0, col + 1);
    }

    int currentRow = -1;
    int layoutRow = 1;

    for (Seat* seat : *seatMap) {
        int r, c;
        std::tie(r, c) = seat->getCoordinates();

        // Add row number label (Giống BookingDialog)
        if (r != currentRow) {
            currentRow = r;
            QLabel* rLbl = new QLabel(QString::number(r + 1).rightJustified(2, '0'));
            rLbl->setFixedSize(18, 26);
            rLbl->setAlignment(Qt::AlignCenter);
            rLbl->setStyleSheet("font-size: 10px; font-weight: bold; color: #333; background: transparent; border: none;");
            seatMapLayout_->addWidget(rLbl, layoutRow, 0);
        }

        QPushButton* btn = new QPushButton();
        btn->setFixedSize(26, 26); // Kích thước nút nhỏ hơn 28,28 để khớp BookingDialog
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFlat(true);

        bool matchClass =
            (currentClass == BookingClass::Economy  && seat->getType() == SeatType::Economy) ||
            (currentClass == BookingClass::Business && seat->getType() == SeatType::Business);

        QString seatCode = QString::fromStdString(seat->getId());

        QString style;
        bool enable = false;

        if (seat->getStatus() == SeatStatus::Booked) {
            style = BOOKED_STYLE;
            btn->setToolTip(seatCode + " (Đã đặt)");
        }
        else if (!matchClass) {
            style = LOCKED_STYLE;
            btn->setToolTip(seatCode + " (Khác hạng)");
        }
        else {
            enable = true;
            style = (seat->getType() == SeatType::Business) ?
                        BUSINESS_AVAILABLE_STYLE :
                        ECONOMY_AVAILABLE_STYLE;

            btn->setToolTip(seatCode + " (Trống)");
        }

        btn->setEnabled(enable);
        btn->setStyleSheet(style);
        btn->setProperty("seatId", seatCode);
        btn->setProperty("baseStyle", style);

        if (enable) {
            connect(btn, &QPushButton::clicked, this, [this, btn, seatCode]() {
                // Reset all seats
                for (int i = 0; i < seatMapLayout_->count(); ++i) {
                    QWidget* w = seatMapLayout_->itemAt(i)->widget();
                    QPushButton* b = qobject_cast<QPushButton*>(w);
                    if (b && b->property("baseStyle").isValid()) {
                        b->setStyleSheet(b->property("baseStyle").toString());
                    }
                }

                // Highlight selected seat
                btn->setStyleSheet(btn->property("baseStyle").toString() + SELECTED_STYLE_SUFFIX);

                selectedSeatId_ = btn->property("seatId").toString();
                
                // Style thông báo ghế đang chọn (Giống BookingDialog)
                selectedSeatDisplayLabel_->setText(
                    QString("Bạn đã chọn ghế: <b style='color:#1565C0; font-size:14px;'>%1</b>").arg(seatCode)
                );
                confirmBtn_->setEnabled(true);
            });
        }

        seatMapLayout_->addWidget(btn, layoutRow, c + 1);

        if (c == cols - 1) layoutRow++;
    }
}

void ChangeBookingDialog::updatePriceDifference()
{
    if (!selectedNewFlight_) return;

    bool eco = economyRadio_->isChecked();
    int newFare = eco ? selectedNewFlight_->getFareEconomy() :
                        selectedNewFlight_->getFareBusiness();

    int oldFare = currentBooking_->getBaseFare();
    int diff = newFare - oldFare;

    // Tái tạo style chênh lệch giá theo logic cũ, nhưng dùng font/màu mới
    if (diff > 0) {
        priceDiffLabel_->setText("Phí phát sinh: " + dotFormat(diff) + " VNĐ");
        priceDiffLabel_->setStyleSheet("color:#D32F2F; font-weight:700; font-size: 14px;"); // Đỏ
    }
    else if (diff < 0) {
        priceDiffLabel_->setText("Hoàn lại: " + dotFormat(-diff) + " VNĐ");
        priceDiffLabel_->setStyleSheet("color:#43A047; font-weight:700; font-size: 14px;"); // Xanh lá
    }
    else {
        priceDiffLabel_->setText("Không thay đổi giá");
        priceDiffLabel_->setStyleSheet("color:#1565C0; font-weight:700; font-size: 14px;"); // Xanh navy
    }
}

// Logic giữ nguyên
void ChangeBookingDialog::onSearchByIdClicked()
{
    QString id = flightIdEdit_->text().trimmed();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập mã chuyến bay.");
        return;
    }

    Flight* f = flightManager_->findFlightById(id.toStdString());
    if (!f) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy chuyến bay.");
        QStringList emptyValues = {"-", "-", "-", "-", "-", "-", "-"};
        for (int i = 0; i < emptyValues.size(); ++i) {
            QLabel* v = this->findChild<QLabel*>(QString("finfo_%1").arg(i));
            if (v) v->setText(emptyValues[i]);
        }
        step1NextBtn_->setEnabled(false);
        return;
    }

    selectedNewFlight_ = f;

    QStringList values = {
        QString::fromStdString(f->getRouteId()),
        QString::fromStdString(f->getAirline()),
        QString::fromStdString(f->getFlightNumber()),
        QString::fromStdString(f->getDepartureDate()),
        QString::fromStdString(f->getDepartureTime()),
        QString::fromStdString(f->getArrivalDate()),
        QString::fromStdString(f->getArrivalTime())
    };

    for (int i = 0; i < values.size(); ++i) {
        QLabel* v = this->findChild<QLabel*>(QString("finfo_%1").arg(i));
        if (v) v->setText(values[i]);
    }

    step1NextBtn_->setEnabled(true);
}

// Logic giữ nguyên
void ChangeBookingDialog::onConfirmClicked()
{
    if (!selectedNewFlight_ || selectedSeatId_.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Bạn chưa chọn ghế.");
        return;
    }

    BookingClass bc = economyRadio_->isChecked() ?
        BookingClass::Economy : BookingClass::Business;

    SeatManager* sm = flightManager_->getSeatManager();

    confirmBtn_->setEnabled(false);

    bool ok = bookingManager_->changeBooking(
        *flightManager_,
        *sm,
        currentBooking_->getBookingId(),
        selectedNewFlight_->getFlightId(),
        selectedSeatId_.toStdString(),
        bc
    );

    if (ok) {
        QMessageBox::information(this, "Thành công", "Đổi vé thành công.");
        accept();
    } else {
        QMessageBox::critical(this, "Thất bại", "Đổi vé thất bại.");
        confirmBtn_->setEnabled(true);
    }
}