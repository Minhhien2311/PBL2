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
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QScreen>
#include <QGuiApplication>
#include <QButtonGroup>
#include <QDebug>
#include <QDateTime>
#include <QFrame>
#include <QScrollBar>

const QString ChangeBookingDialog::SEAT_BASE_STYLE =
    "QPushButton { border: none; border-radius: 3px; }";

const QString ChangeBookingDialog::BUSINESS_AVAILABLE_STYLE =
    ChangeBookingDialog::SEAT_BASE_STYLE +
    "QPushButton { background: #FFD700; } QPushButton:hover { background: #FFC700; border: 2px solid #FF6B35; }";

const QString ChangeBookingDialog::ECONOMY_AVAILABLE_STYLE =
    ChangeBookingDialog::SEAT_BASE_STYLE +
    "QPushButton { background: #90EE90; } QPushButton:hover { background: #7FD87F; border: 2px solid #FF6B35; }";

const QString ChangeBookingDialog::BOOKED_STYLE =
    ChangeBookingDialog::SEAT_BASE_STYLE +
    "QPushButton { background: #D3D3D3; }";

const QString ChangeBookingDialog::LOCKED_STYLE =
    ChangeBookingDialog::SEAT_BASE_STYLE +
    "QPushButton { background: #FF6B6B; }";

const QString ChangeBookingDialog::SELECTED_STYLE_SUFFIX = " border: 2px solid #FF6B35;";

namespace {
    QString dotFormat(int v) {
        QString s = QString::number(v);
        int pos = s.length() - 3;
        while (pos > 0) {
            s.insert(pos, '.');
            pos -= 3;
        }
        return s;
    }
}

ChangeBookingDialog::ChangeBookingDialog(Booking* currentBooking,
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
      selectedSeatId_("")
{
    Q_ASSERT(currentBooking_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);

    setWindowTitle("Đổi đặt chỗ");
    // responsive sizing similar BookingDialog
    setMinimumSize(700, 560);
    QRect sg = QGuiApplication::primaryScreen()->availableGeometry();
    setMaximumHeight(static_cast<int>(sg.height() * 0.92));
    setupUi();
    setupConnections();

    // load current flight for reference
    currentFlight_ = flightManager_->findFlightById(currentBooking_->getFlightId());

    showStep(1);
}

void ChangeBookingDialog::setupUi()
{
    // overall layout
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 8, 40, 12);
    mainLayout->setSpacing(10);

    // progress header (2 steps) - centered like BookingDialog
    QWidget* header = new QWidget(this);
    header->setFixedHeight(64);
    header->setStyleSheet("background: transparent;");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setAlignment(Qt::AlignCenter);
    headerLayout->setContentsMargins(0, 12, 0, 8);
    headerLayout->setSpacing(6);

    auto createStepLabel = [](const QString& txt, bool active)->QLabel* {
        QLabel* lbl = new QLabel(txt);
        lbl->setFixedSize(28, 28);
        lbl->setAlignment(Qt::AlignCenter);
        if (active) {
            lbl->setStyleSheet("background-color: #74cc00; color: white; border-radius: 14px; font-weight: bold;");
        } else {
            lbl->setStyleSheet("background-color: #cbdceb; color: white; border-radius: 14px; font-weight: bold;");
        }
        return lbl;
    };

    QLabel* st1 = createStepLabel("1", true);
    QLabel* line = new QLabel;
    line->setFixedSize(120, 4);
    line->setStyleSheet("background-color: #cbdceb; border-radius:2px;");
    QLabel* st2 = createStepLabel("2", false);

    headerLayout->addWidget(st1);
    headerLayout->addWidget(line);
    headerLayout->addWidget(st2);
    mainLayout->addWidget(header);

    // Stacked widget style: we will simply hide/show two containers
    QWidget* body = new QWidget(this);
    QVBoxLayout* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(0,0,0,0);
    bodyLayout->setSpacing(12);

    // ====== STEP 1: nhập ID chuyến ======
    step1Container_ = new QWidget(body);
    step1Container_->setStyleSheet("background:white; border:1px solid #BFCFE0; border-radius:8px;");
    QVBoxLayout* s1 = new QVBoxLayout(step1Container_);
    s1->setContentsMargins(16,16,16,16);
    s1->setSpacing(12);

    QLabel* t1 = new QLabel("Bước 1: Nhập ID chuyến bay thay thế");
    t1->setStyleSheet("font-size:16px; font-weight:700; color:#1565C0;");
    s1->addWidget(t1);

    QHBoxLayout* idRow = new QHBoxLayout();
    QLabel* idLabel = new QLabel("ID chuyến:");
    idLabel->setMinimumWidth(90);
    flightIdEdit_ = new QLineEdit();
    flightIdEdit_->setPlaceholderText("VD: FL12345");
    flightIdEdit_->setMinimumWidth(320);
    searchBtn_ = new QPushButton("Tìm");
    searchBtn_->setStyleSheet("QPushButton { background:#608bc1; color:white; padding:8px 14px; border-radius:6px; font-weight:600; }"
                              "QPushButton:hover { background:#1565C0; }");

    idRow->addWidget(idLabel);
    idRow->addWidget(flightIdEdit_);
    idRow->addWidget(searchBtn_);
    idRow->addStretch();
    s1->addLayout(idRow);

    // result table (single row)
    resultTable_ = new QTableView(step1Container_);
    resultTable_->setMinimumHeight(160);
    resultTable_->verticalHeader()->setVisible(false);
    resultModel_ = new QStandardItemModel(0,5,this);
    resultModel_->setHorizontalHeaderLabels({"STT","Mã chuyến","Hãng","Số hiệu","Ngày/giờ"});
    resultTable_->setModel(resultModel_);
    resultTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    resultTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    s1->addWidget(resultTable_);

    QHBoxLayout* s1BtnRow = new QHBoxLayout();
    s1BtnRow->addStretch();
    QPushButton* cancelBtn = new QPushButton("Hủy", step1Container_);
    cancelBtn->setStyleSheet("QPushButton { background: #b5c7d7; color:white; padding:8px 12px; border-radius:6px; font-weight:600; }"
                             "QPushButton:hover { background:#8c9aa6; }");
    step1NextBtn_ = new QPushButton("Trang sau", step1Container_);
    step1NextBtn_->setStyleSheet("QPushButton { background:#608bc1; color:white; padding:8px 14px; border-radius:6px; font-weight:600; }"
                                "QPushButton:hover { background:#1565C0; }");
    step1NextBtn_->setEnabled(false);
    s1BtnRow->addWidget(cancelBtn);
    s1BtnRow->addSpacing(8);
    s1BtnRow->addWidget(step1NextBtn_);
    s1->addLayout(s1BtnRow);

    bodyLayout->addWidget(step1Container_);

    // ====== STEP 2: Chọn hạng + seat map (theo BookingDialog) ======
    step2Container_ = new QWidget(body);
    step2Container_->setStyleSheet("background:white; border:1px solid #BFCFE0; border-radius:8px;");
    QVBoxLayout* s2 = new QVBoxLayout(step2Container_);
    s2->setContentsMargins(16,16,16,16);
    s2->setSpacing(10);

    QLabel* t2 = new QLabel("Bước 2: Chọn hạng và ghế");
    t2->setStyleSheet("font-size:16px; font-weight:700; color:#1565C0;");
    s2->addWidget(t2);

    // flight info
    flightInfoText_ = new QTextEdit(step2Container_);
    flightInfoText_->setReadOnly(true);
    flightInfoText_->setMinimumHeight(80);
    flightInfoText_->setStyleSheet("background:#FAFBFF; border:1px solid #e6f0fb; border-radius:6px; padding:8px; font-size:13px;");
    s2->addWidget(flightInfoText_);

    // class selector (reuse BookingDialog style)
    QFrame* classFrame = new QFrame(step2Container_);
    classFrame->setStyleSheet("#classFrame { background:white; border:1px solid #608bc1; border-radius:8px; }");
    QHBoxLayout* classLayout = new QHBoxLayout(classFrame);
    classLayout->setContentsMargins(12,10,12,10);
    classLayout->setSpacing(12);
    QLabel* lblClass = new QLabel("Chọn hạng vé:");
    lblClass->setStyleSheet("font-weight:bold; color:#1565C0;");
    economyRadio_ = new QRadioButton("Hạng phổ thông");
    businessRadio_ = new QRadioButton("Hạng thương gia");
    economyRadio_->setChecked(true);
    QButtonGroup* grp = new QButtonGroup(this);
    grp->addButton(economyRadio_);
    grp->addButton(businessRadio_);
    fareLabel_ = new QLabel();
    fareLabel_->setStyleSheet("font-size:15px; font-weight:bold; color:#74cc00;");
    classLayout->addWidget(lblClass);
    classLayout->addWidget(economyRadio_);
    classLayout->addWidget(businessRadio_);
    classLayout->addStretch();
    classLayout->addWidget(fareLabel_);
    s2->addWidget(classFrame);

    // seat map wrapper: frame + scroll area (copy BookingDialog layout)
    QFrame* seatMapFrame = new QFrame(step2Container_);
    seatMapFrame->setObjectName("SeatMapFrame");
    seatMapFrame->setStyleSheet("#SeatMapFrame { background:white; border:1px solid #608bc1; border-radius:8px; }");
    QVBoxLayout* frameLayout = new QVBoxLayout(seatMapFrame);
    frameLayout->setContentsMargins(10,10,10,10);

    QScrollArea* scrollArea = new QScrollArea(seatMapFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet("background: transparent; border: none;");
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    seatMapContainer_ = new QWidget;
    seatMapLayout_ = new QGridLayout(seatMapContainer_);
    seatMapLayout_->setSpacing(6);
    seatMapLayout_->setAlignment(Qt::AlignCenter);
    seatMapLayout_->setContentsMargins(6,6,6,6);
    scrollArea->setWidget(seatMapContainer_);
    frameLayout->addWidget(scrollArea);
    s2->addWidget(seatMapFrame, 1);

    // legend & selected display
    legendLabel_ = new QLabel("Chú thích:  Hạng phổ thông (xanh)  Hạng thương gia (vàng)  Không thể chọn (đỏ)  Đã đặt (xám)");
    legendLabel_->setStyleSheet("color:#666; font-size:12px;");
    s2->addWidget(legendLabel_);

    selectedSeatDisplayLabel_ = new QLabel("Hiện chưa chọn ghế");
    selectedSeatDisplayLabel_->setAlignment(Qt::AlignCenter);
    selectedSeatDisplayLabel_->setStyleSheet("font-weight:700; color:#1565C0;");
    s2->addWidget(selectedSeatDisplayLabel_);

    priceDiffLabel_ = new QLabel("");
    priceDiffLabel_->setAlignment(Qt::AlignCenter);
    priceDiffLabel_->setStyleSheet("font-weight:700;");
    s2->addWidget(priceDiffLabel_);

    // buttons: back + confirm
    QHBoxLayout* s2Btns = new QHBoxLayout();
    s2Btns->addStretch();
    backBtn_ = new QPushButton("Trang trước", step2Container_);
    backBtn_->setStyleSheet("QPushButton { background: white; color: #608bc1; border:1px solid #608bc1; padding:8px 12px; border-radius:6px; font-weight:600; }"
                            "QPushButton:hover { background:#eaf3ff; }");
    confirmBtn_ = new QPushButton("Xác nhận đổi vé", step2Container_);
    confirmBtn_->setStyleSheet("QPushButton { background:#608bc1; color:white; padding:8px 14px; border-radius:6px; font-weight:600; }"
                               "QPushButton:hover { background:#1565C0; }");
    confirmBtn_->setEnabled(false);
    s2Btns->addWidget(backBtn_);
    s2Btns->addSpacing(8);
    s2Btns->addWidget(confirmBtn_);
    s2Btns->addStretch();
    s2->addLayout(s2Btns);

    bodyLayout->addWidget(step2Container_);
    step2Container_->hide();

    mainLayout->addWidget(body);

    // connect cancel
    connect(cancelBtn, &QPushButton::clicked, this, &ChangeBookingDialog::reject);
}

void ChangeBookingDialog::setupConnections()
{
    connect(searchBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSearchByIdClicked);
    connect(step1NextBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSelectFoundFlight);
    connect(resultTable_, &QTableView::doubleClicked, this, &ChangeBookingDialog::onSelectFoundFlight);
    connect(backBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onBackToStep1);
    connect(economyRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
    connect(businessRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
    connect(confirmBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onConfirmClicked);

    connect(resultTable_->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection&, const QItemSelection&){
                step1NextBtn_->setEnabled(!resultTable_->selectionModel()->selectedRows().isEmpty());
            });
}

QString ChangeBookingDialog::formatWithDot(int v) const { return dotFormat(v); }

// ================= Step1 =================
void ChangeBookingDialog::onSearchByIdClicked()
{
    resultModel_->removeRows(0, resultModel_->rowCount());
    QString id = flightIdEdit_->text().trimmed();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập ID chuyến bay.");
        return;
    }

    Flight* f = flightManager_->findFlightById(id.toStdString());
    if (!f) {
        QMessageBox::information(this, "Không tìm thấy", "Không tìm thấy chuyến có ID này.");
        return;
    }

    QList<QStandardItem*> row;
    row << new QStandardItem("1")
        << new QStandardItem(QString::fromStdString(f->getFlightId()))
        << new QStandardItem(QString::fromStdString(f->getAirline()))
        << new QStandardItem(QString::fromStdString(f->getFlightNumber()))
        << new QStandardItem(QString::fromStdString(f->getDepartureDate()) + " " + QString::fromStdString(f->getDepartureTime()));
    for (auto &it : row) it->setTextAlignment(Qt::AlignCenter);
    resultModel_->appendRow(row);
    resultTable_->selectRow(0);
}

// select & go step2
void ChangeBookingDialog::onSelectFoundFlight()
{
    auto sel = resultTable_->selectionModel()->selectedRows();
    if (sel.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn chuyến để tiếp tục.");
        return;
    }
    QString id = resultModel_->item(sel.first().row(), 1)->text();
    selectedNewFlight_ = flightManager_->findFlightById(id.toStdString());
    if (!selectedNewFlight_) {
        QMessageBox::warning(this, "Lỗi", "Không thể tải chuyến đã chọn.");
        return;
    }

    // fill flight info
    flightInfoText_->setText(QString("Mã chuyến: %1\nSố hiệu: %2\nNgày: %3 %4\nHãng: %5")
                             .arg(QString::fromStdString(selectedNewFlight_->getFlightId()))
                             .arg(QString::fromStdString(selectedNewFlight_->getFlightNumber()))
                             .arg(QString::fromStdString(selectedNewFlight_->getDepartureDate()))
                             .arg(QString::fromStdString(selectedNewFlight_->getDepartureTime()))
                             .arg(QString::fromStdString(selectedNewFlight_->getAirline()))
                             );

    economyRadio_->setChecked(true);
    renderSeatMap();
    updatePriceDifference();
    showStep(2);
}

void ChangeBookingDialog::showStep(int step)
{
    currentStep_ = step;
    step1Container_->setVisible(step == 1);
    step2Container_->setVisible(step == 2);
}

// back
void ChangeBookingDialog::onBackToStep1()
{
    selectedSeatId_.clear();
    confirmBtn_->setEnabled(false);
    showStep(1);
}

// ================= Step2: seat map (copy BookingDialog logic) =================
void ChangeBookingDialog::onClassChanged()
{
    renderSeatMap();
    updatePriceDifference();
    selectedSeatId_.clear();
    selectedSeatDisplayLabel_->setText("Hiện chưa chọn ghế");
    confirmBtn_->setEnabled(false);
}

void ChangeBookingDialog::renderSeatMap()
{
    // clear layout
    QLayoutItem* item;
    while ((item = seatMapLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    selectedSeatId_.clear();
    confirmBtn_->setEnabled(false);

    if (!selectedNewFlight_) return;

    SeatManager* sm = flightManager_->getSeatManager();
    if (!sm) return;

    if (!sm->loadSeatMapFor(selectedNewFlight_)) {
        QLabel* err = new QLabel("Không thể tải sơ đồ ghế");
        err->setStyleSheet("color:red;");
        seatMapLayout_->addWidget(err, 0, 0);
        return;
    }

    auto* seatMap = sm->getActiveSeatMap();
    if (!seatMap) return;

    int cols = sm->getSeatColumns();
    BookingClass currentClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;

    // Add column headers A B C...
    for (int col = 0; col < cols; ++col) {
        QLabel* header = new QLabel(QString(QChar('A' + col)));
        header->setAlignment(Qt::AlignCenter);
        header->setFixedSize(26, 18);
        header->setStyleSheet("font-weight:bold; color:#333;");
        seatMapLayout_->addWidget(header, 0, col + 1);
    }

    int currentRow = -1;
    int layoutRow = 1;

    for (Seat* seat : *seatMap) {
        int r, c;
        std::tie(r, c) = seat->getCoordinates();

        // row label
        if (r != currentRow) {
            currentRow = r;
            QLabel* rLbl = new QLabel(QString::number(r + 1));
            rLbl->setFixedSize(22, 22);
            rLbl->setAlignment(Qt::AlignCenter);
            rLbl->setStyleSheet("font-weight:bold; color:#333;");
            seatMapLayout_->addWidget(rLbl, layoutRow, 0);
        }

        QPushButton* btn = new QPushButton;
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
                QString sid = btn->property("seatId").toString();
                // reset styles
                for (int i = 0; i < seatMapLayout_->count(); ++i) {
                    QWidget* w = seatMapLayout_->itemAt(i)->widget();
                    QPushButton* b = qobject_cast<QPushButton*>(w);
                    if (b && b->property("baseStyle").isValid()) {
                        b->setStyleSheet(b->property("baseStyle").toString());
                    }
                }
                // highlight selected
                QString selStyle = btn->property("baseStyle").toString() + ChangeBookingDialog::SELECTED_STYLE_SUFFIX;
                btn->setStyleSheet(selStyle);
                selectedSeatId_ = sid;
                selectedSeatDisplayLabel_->setText(QString("Bạn đã chọn ghế: <b style='color:#1565C0;'>%1</b>").arg(sid));
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
    bool wantE = economyRadio_->isChecked();
    int newFare = wantE ? selectedNewFlight_->getFareEconomy() : selectedNewFlight_->getFareBusiness();
    int oldFare = currentBooking_->getBaseFare();
    int diff = newFare - oldFare;

    fareLabel_->setText(QString("%1 VNĐ").arg(dotFormat(newFare)));

    if (diff > 0) {
        priceDiffLabel_->setText(QString("Phí phát sinh: %1 VNĐ").arg(dotFormat(diff)));
        priceDiffLabel_->setStyleSheet("color:#0051ff; font-weight:700;");
    } else if (diff < 0) {
        priceDiffLabel_->setText(QString("Hoàn trả: %1 VNĐ").arg(dotFormat(-diff)));
        priceDiffLabel_->setStyleSheet("color:#ff2f2f; font-weight:700;");
    } else {
        priceDiffLabel_->setText("Không đổi giá");
        priceDiffLabel_->setStyleSheet("color:#008a00; font-weight:700;");
    }
}

// confirm change
void ChangeBookingDialog::onConfirmClicked()
{
    if (!selectedNewFlight_) {
        QMessageBox::warning(this,"Lỗi","Chưa chọn chuyến.");
        return;
    }
    if (selectedSeatId_.isEmpty()) {
        QMessageBox::warning(this,"Lỗi","Chưa chọn ghế.");
        return;
    }

    BookingClass bc = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;

    // confirm prompt
    if (QMessageBox::question(this, "Xác nhận đổi vé",
                              QString("Xác nhận đổi vé sang chuyến %1, ghế %2 ?")
                              .arg(QString::fromStdString(selectedNewFlight_->getFlightNumber()))
                              .arg(selectedSeatId_),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    SeatManager* sm = flightManager_->getSeatManager();
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
        QMessageBox::critical(this, "Thất bại", "Đổi vé thất bại. Vui lòng thử lại.");
    }
}
