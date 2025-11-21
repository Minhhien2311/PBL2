// #include "ChangeBookingDialog.h"
// #include "entities/Booking.h"
// #include "entities/Flight.h"
// #include "core/FlightManager.h"
// #include "core/BookingManager.h"
// #include "core/SeatManager.h"
// #include "entities/Seat.h"

// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QGridLayout>
// #include <QLabel>
// #include <QLineEdit>
// #include <QTextEdit>
// #include <QPushButton>
// #include <QRadioButton>
// #include <QMessageBox>
// #include <QScrollArea>
// #include <QButtonGroup>
// #include <QFrame>
// #include <QDebug>

// // Ensure base style contains transparent border so hover border becomes visible
// const QString ChangeBookingDialog::SEAT_BASE_STYLE =
//     "QPushButton { border: 2px solid transparent; border-radius: 3px; }";

// // include base style into each seat style so hover border works correctly
// const QString ChangeBookingDialog::BUSINESS_AVAILABLE_STYLE =
//     ChangeBookingDialog::SEAT_BASE_STYLE +
//     "QPushButton { background: #FFD740; } "
//     "QPushButton:hover { background: #FFCA28; border: 2px solid #FF6B35; }";

// const QString ChangeBookingDialog::ECONOMY_AVAILABLE_STYLE =
//     ChangeBookingDialog::SEAT_BASE_STYLE +
//     "QPushButton { background: #A5D6A7; } "
//     "QPushButton:hover { background: #81C784; border: 2px solid #FF6B35; }";

// const QString ChangeBookingDialog::BOOKED_STYLE =
//     ChangeBookingDialog::SEAT_BASE_STYLE +
//     "QPushButton { background: #BDBDBD; }";

// const QString ChangeBookingDialog::LOCKED_STYLE =
//     ChangeBookingDialog::SEAT_BASE_STYLE +
//     "QPushButton { background: #EF5350; }";

// const QString ChangeBookingDialog::SELECTED_STYLE_SUFFIX =
//     " border: 2px solid #1E88E5;";

// namespace {
// QString dotFormat(int v) {
//     QString s = QString::number(v);
//     int pos = s.length() - 3;
//     while (pos > 0) {
//         s.insert(pos, '.');
//         pos -= 3;
//     }
//     return s;
// }
// }

// ChangeBookingDialog::ChangeBookingDialog(
//         Booking* currentBooking,
//         BookingManager* bookingManager,
//         FlightManager* flightManager,
//         QWidget *parent)
//     : QDialog(parent),
//       currentBooking_(currentBooking),
//       bookingManager_(bookingManager),
//       flightManager_(flightManager),
//       currentFlight_(nullptr),
//       selectedNewFlight_(nullptr),
//       currentStep_(1)
// {
//     setWindowTitle("Đổi đặt chỗ");
//     setMinimumSize(800, 650);

//     currentFlight_ = flightManager_->findFlightById(currentBooking_->getFlightId());

//     setupUi();
//     setupConnections();
//     showStep(1);
// }

// void ChangeBookingDialog::setupUi()
// {
//     auto* mainLayout = new QVBoxLayout(this);
//     mainLayout->setContentsMargins(24, 12, 24, 12);
//     mainLayout->setSpacing(6);

//     // ---------- header: step indicator ----------
//     QWidget* header = new QWidget(this);
//     header->setFixedHeight(72);
//     QHBoxLayout* headerLayout = new QHBoxLayout(header);
//     headerLayout->setAlignment(Qt::AlignCenter);
//     headerLayout->setContentsMargins(0, 8, 0, 8);
//     headerLayout->setSpacing(6);

//     QLabel* stepCircle1 = new QLabel("1", header);
//     stepCircle1->setObjectName("stepCircle1");
//     stepCircle1->setFixedSize(32, 32);
//     stepCircle1->setAlignment(Qt::AlignCenter);
//     stepCircle1->setStyleSheet("background:#7BC043; color:white; border-radius:16px; font-weight:700;");

//     QLabel* stepLine = new QLabel(header);
//     stepLine->setObjectName("stepLine");
//     stepLine->setFixedSize(160, 6);
//     stepLine->setStyleSheet("background:#E3F2FD; border-radius:3px;");

//     QLabel* stepCircle2 = new QLabel("2", header);
//     stepCircle2->setObjectName("stepCircle2");
//     stepCircle2->setFixedSize(32, 32);
//     stepCircle2->setAlignment(Qt::AlignCenter);
//     stepCircle2->setStyleSheet("background:#E3F2FD; color:white; border-radius:16px; font-weight:700;");

//     headerLayout->addWidget(stepCircle1);
//     headerLayout->addWidget(stepLine);
//     headerLayout->addWidget(stepCircle2);

//     mainLayout->addWidget(header);

//     // ---------- body ----------
//     // Step1 container
//     step1Container_ = new QWidget(this);
//     QVBoxLayout* s1 = new QVBoxLayout(step1Container_);
//     s1->setContentsMargins(12, 8, 12, 8);
//     s1->setSpacing(8);

//     QLabel* title1 = new QLabel("Chọn chuyến bay mới");
//     title1->setStyleSheet("font-size:18px; font-weight:700; color:#1565C0;");
//     title1->setAlignment(Qt::AlignCenter);
//     s1->addWidget(title1);

//     // input row
//     QHBoxLayout* inputRow = new QHBoxLayout();
//     inputRow->setSpacing(8);
//     QLabel* lblId = new QLabel("Nhập mã chuyến bay:");
//     flightIdEdit_ = new QLineEdit();
//     flightIdEdit_->setPlaceholderText("FI-XXXXX");
//     flightIdEdit_->setFixedWidth(300);
//     searchBtn_ = new QPushButton("Tìm kiếm");
//     searchBtn_->setStyleSheet(
//         "QPushButton { background:#1E88E5; color:white; padding:8px 14px; border-radius:6px; }"
//         "QPushButton:hover { background:#4e7eb5; }"
//         "QPushButton:pressed { background:#3c6aa3; }"
//     );

//     inputRow->addWidget(lblId);
//     inputRow->addWidget(flightIdEdit_);
//     inputRow->addWidget(searchBtn_);
//     inputRow->addStretch();
//     s1->addLayout(inputRow);

//     // info frame (grid) raised higher
//     QFrame* infoFrame = new QFrame();
//     infoFrame->setObjectName("infoFrame");
//     infoFrame->setStyleSheet("#infoFrame{ border:1px solid #BFD7ED; border-radius:8px; background:white; }");
//     infoFrame->setFixedHeight(140);

//     s1->addSpacing(4);
//     s1->addWidget(infoFrame, 0, Qt::AlignTop);

//     QGridLayout* infoGrid = new QGridLayout(infoFrame);
//     infoGrid->setContentsMargins(12, 8, 12, 8);
//     infoGrid->setHorizontalSpacing(18);
//     infoGrid->setVerticalSpacing(6);

//     QStringList leftLabels = {
//         "Lộ trình bay", "Hãng hàng không", "Số hiệu chuyến bay",
//         "Ngày khởi hành", "Giờ khởi hành", "Ngày hạ cánh", "Giờ hạ cánh"
//     };

//     for (int i = 0; i < leftLabels.size(); ++i) {
//         QLabel* l = new QLabel(leftLabels[i]);
//         l->setStyleSheet("font-weight:600; color:#1E3D58;");
//         QLabel* v = new QLabel("-");
//         v->setObjectName(QString("finfo_%1").arg(i));
//         v->setStyleSheet("color:#1E3D58;");
//         infoGrid->addWidget(l, i, 0, Qt::AlignLeft);
//         infoGrid->addWidget(v, i, 1, Qt::AlignLeft);
//     }

//     // next button
//     QHBoxLayout* step1Btns = new QHBoxLayout();
//     step1Btns->addStretch();
//     step1NextBtn_ = new QPushButton("Trang sau");
//     step1NextBtn_->setEnabled(false);
//     step1NextBtn_->setStyleSheet(
//         "QPushButton { background:#1E88E5; color:white; padding:8px 14px; border-radius:6px; }"
//         "QPushButton:hover { background:#4e7eb5; }"
//         "QPushButton:pressed { background:#3c6aa3; }"
//     );
//     step1Btns->addWidget(step1NextBtn_);
//     s1->addLayout(step1Btns);

//     // Step2 container -----------------------------------------
//     step2Container_ = new QWidget(this);
//     QVBoxLayout* s2 = new QVBoxLayout(step2Container_);
//     s2->setContentsMargins(12,8,12,8);
//     s2->setSpacing(8);

//     QLabel* title2 = new QLabel("Chọn ghế ngồi");
//     title2->setAlignment(Qt::AlignCenter);
//     title2->setStyleSheet("font-size:18px; font-weight:700; color:#1565C0;");
//     s2->addWidget(title2);

//     // class selector
//     QFrame* classFrame = new QFrame();
//     classFrame->setStyleSheet("border:1px solid #90CAF9; border-radius:8px; padding:8px; background:white;");
//     QHBoxLayout* classLayout = new QHBoxLayout(classFrame);
//     classLayout->setContentsMargins(12,6,12,6);
//     classLayout->setSpacing(18);

//     QLabel* classLabel = new QLabel("Chọn hạng vé:");
//     classLabel->setStyleSheet("font-weight:700; color:#1565C0;");

//     economyRadio_ = new QRadioButton("Hạng phổ thông");
//     economyRadio_->setObjectName("ecoRadio");
//     businessRadio_ = new QRadioButton("Hạng thương gia");
//     businessRadio_->setObjectName("busRadio");

//     QString radioStyle = R"(
//         QRadioButton::indicator {
//             width:18px; height:18px;
//             border-radius:4px;
//             border: 1px solid #90CAF9;
//             background:white;
//         }
//         QRadioButton#ecoRadio::indicator:checked {
//             background:#A5D6A7;
//             border:1px solid #7CBF73;
//         }
//         QRadioButton#busRadio::indicator:checked {
//             background:#FFD740;
//             border:1px solid #FFCA28;
//         }
//     )";
//     economyRadio_->setStyleSheet(radioStyle);
//     businessRadio_->setStyleSheet(radioStyle);

//     QButtonGroup* classGrp = new QButtonGroup(this);
//     classGrp->addButton(economyRadio_);
//     classGrp->addButton(businessRadio_);

//     fareLabel_ = new QLabel("0 VNĐ");
//     fareLabel_->setStyleSheet("font-weight:700; color:#43A047;"); // default eco

//     classLayout->addWidget(classLabel);
//     classLayout->addWidget(economyRadio_);
//     classLayout->addWidget(businessRadio_);
//     classLayout->addStretch();
//     classLayout->addWidget(fareLabel_);
//     s2->addWidget(classFrame);

//     // seat map scroll
//     seatMapContainer_ = new QWidget;
//     seatMapLayout_ = new QGridLayout(seatMapContainer_);
//     seatMapLayout_->setSpacing(6);
//     seatMapLayout_->setContentsMargins(10,10,10,10);
//     seatMapLayout_->setAlignment(Qt::AlignCenter);

//     QScrollArea* scroll = new QScrollArea;
//     scroll->setWidget(seatMapContainer_);
//     scroll->setWidgetResizable(true);
//     scroll->setStyleSheet("border:none;");
//     s2->addWidget(scroll, 1);

//     // legend
//     QHBoxLayout* legend = new QHBoxLayout();
//     legend->setSpacing(20);
//     legend->setAlignment(Qt::AlignCenter);

//     auto addLegend = [&](QString color, QString text){
//         QLabel* box = new QLabel;
//         box->setFixedSize(14,14);
//         box->setStyleSheet("background:" + color + "; border-radius:3px;");
//         QLabel* lab = new QLabel(text);
//         QHBoxLayout* h = new QHBoxLayout();
//         h->addWidget(box);
//         h->addWidget(lab);
//         QWidget* w = new QWidget;
//         w->setLayout(h);
//         legend->addWidget(w);
//     };

//     addLegend("#A5D6A7", "Hạng phổ thông");
//     addLegend("#FFD740", "Hạng thương gia");
//     addLegend("#EF5350", "Không thể chọn");
//     addLegend("#BDBDBD", "Đã đặt");
//     s2->addLayout(legend);

//     selectedSeatDisplayLabel_ = new QLabel("Hiện tại chưa chọn ghế");
//     selectedSeatDisplayLabel_->setAlignment(Qt::AlignCenter);
//     selectedSeatDisplayLabel_->setStyleSheet("font-weight:700; color:#1565C0;");
//     s2->addWidget(selectedSeatDisplayLabel_);

//     priceDiffLabel_ = new QLabel("");
//     priceDiffLabel_->setAlignment(Qt::AlignCenter);
//     s2->addWidget(priceDiffLabel_);

//     // bottom bar
//     QWidget* bottomBar = new QWidget(this);
//     QHBoxLayout* bottomLayout = new QHBoxLayout(bottomBar);
//     bottomLayout->setContentsMargins(0,8,0,0);

//     QPushButton* cancelBtn = new QPushButton("Hủy đặt vé");
//     cancelBtn->setStyleSheet(
//         "QPushButton{ background:#b5c7d7; color:white; padding:8px 12px; border-radius:6px; font-weight:600; }"
//         "QPushButton:hover{ background:#9eb2c4; }"
//         "QPushButton:pressed{ background:#8ba1b5; }"
//     );

//     connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

//     backBtn_ = new QPushButton("Trang trước");
//     backBtn_->setStyleSheet(
//         "QPushButton { background:white; color:#608bc1; border:1px solid #608bc1; padding:8px 12px; border-radius:6px; }"
//         "QPushButton:hover { background:#d7eaff; }"
//     );
//     backBtn_->hide();

//     confirmBtn_ = new QPushButton("Xác nhận đổi vé");
//     confirmBtn_->setStyleSheet(
//         "QPushButton { background:#608bc1; color:white; padding:8px 14px; border-radius:6px; font-weight:600; }"
//         "QPushButton:hover { background:#4e7eb5; }"
//     );
//     confirmBtn_->setEnabled(false);
//     confirmBtn_->hide();

//     step1NextBtn_->setParent(bottomBar);

//     bottomLayout->addWidget(cancelBtn);
//     bottomLayout->addStretch();
//     bottomLayout->addWidget(backBtn_);
//     bottomLayout->addWidget(step1NextBtn_);
//     bottomLayout->addWidget(confirmBtn_);

//     mainLayout->addWidget(step1Container_, 1);
//     mainLayout->addWidget(step2Container_, 1);
//     mainLayout->addWidget(bottomBar);

//     step2Container_->hide();
// }

// void ChangeBookingDialog::setupConnections()
// {
//     connect(searchBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSearchByIdClicked);
//     connect(step1NextBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSelectFoundFlight);

//     connect(backBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onBackToStep1);
//     connect(economyRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
//     connect(businessRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
//     connect(confirmBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onConfirmClicked);
// }

// QString ChangeBookingDialog::formatWithDot(int v) const { return dotFormat(v); }

// void ChangeBookingDialog::onSearchByIdClicked()
// {
//     QString id = flightIdEdit_->text().trimmed();
//     if (id.isEmpty()) {
//         QMessageBox::warning(this, "Lỗi", "Vui lòng nhập mã chuyến bay.");
//         return;
//     }

//     Flight* f = flightManager_->findFlightById(id.toStdString());
//     if (!f) {
//         QMessageBox::warning(this, "Lỗi", "Không tìm thấy chuyến bay.");
//         return;
//     }

//     selectedNewFlight_ = f;

//     QStringList values = {
//         QString::fromStdString(f->getRouteId()),
//         QString::fromStdString(f->getAirline()),
//         QString::fromStdString(f->getFlightNumber()),
//         QString::fromStdString(f->getDepartureDate()),
//         QString::fromStdString(f->getDepartureTime()),
//         QString::fromStdString(f->getArrivalDate()),
//         QString::fromStdString(f->getArrivalTime())
//     };

//     for (int i = 0; i < values.size(); ++i) {
//         QLabel* v = this->findChild<QLabel*>(QString("finfo_%1").arg(i));
//         if (v) v->setText(values[i]);
//     }

//     step1NextBtn_->setEnabled(true);
// }

// void ChangeBookingDialog::onSelectFoundFlight()
// {
//     if (!selectedNewFlight_) return;

//     economyRadio_->setChecked(true);

//     renderSeatMap();
//     updatePriceDifference();

//     showStep(2);
// }

// void ChangeBookingDialog::showStep(int step)
// {
//     currentStep_ = step;

//     QLabel* step1 = findChild<QLabel*>("stepCircle1");
//     QLabel* step2 = findChild<QLabel*>("stepCircle2");
//     QLabel* line  = findChild<QLabel*>("stepLine");

//     if (step == 1) {
//         step1Container_->show();
//         step2Container_->hide();

//         step1->setStyleSheet("background:#7BC043; color:white; border-radius:16px; font-weight:700;");
//         line->setStyleSheet("background:#E3F2FD; border-radius:3px;");
//         step2->setStyleSheet("background:#E3F2FD; color:white; border-radius:16px; font-weight:700;");

//         step1NextBtn_->show();
//         backBtn_->hide();
//         confirmBtn_->hide();
//     }
//     else {
//         step1Container_->hide();
//         step2Container_->show();

//         step1->setStyleSheet("background:#7BC043; color:white; border-radius:16px; font-weight:700;");
//         line->setStyleSheet("background:#7BC043; border-radius:3px;");
//         step2->setStyleSheet("background:#7BC043; color:white; border-radius:16px; font-weight:700;");

//         step1NextBtn_->hide();
//         backBtn_->show();
//         confirmBtn_->show();
//     }
// }

// void ChangeBookingDialog::onBackToStep1()
// {
//     showStep(1);
// }

// void ChangeBookingDialog::onClassChanged()
// {
//     selectedSeatId_.clear();
//     selectedSeatDisplayLabel_->setText("Hiện tại chưa chọn ghế");
//     confirmBtn_->setEnabled(false);

//     renderSeatMap();
//     updatePriceDifference();
// }

// void ChangeBookingDialog::renderSeatMap()
// {
//     while (seatMapLayout_->count()) {
//         QLayoutItem* it = seatMapLayout_->takeAt(0);
//         if (it->widget()) it->widget()->deleteLater();
//         delete it;
//     }

//     if (!selectedNewFlight_) return;

//     SeatManager* sm = flightManager_->getSeatManager();
//     if (!sm) return;

//     if (!sm->loadSeatMapFor(selectedNewFlight_)) {
//         QLabel* err = new QLabel("Không thể tải sơ đồ ghế");
//         seatMapLayout_->addWidget(err, 0, 0);
//         return;
//     }

//     auto* seatMap = sm->getActiveSeatMap();
//     if (!seatMap) return;

//     int cols = sm->getSeatColumns();
//     BookingClass currentClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;

//     // Add column headers
//     for (int col = 0; col < cols; ++col) {
//         QLabel* header = new QLabel(QString(QChar('A' + col)));
//         header->setAlignment(Qt::AlignCenter);
//         header->setStyleSheet("font-weight:700; color:#333;");
//         seatMapLayout_->addWidget(header, 0, col + 1);
//     }

//     int currentRow = -1;
//     int layoutRow = 1;

//     for (Seat* seat : *seatMap) {
//         int r, c;
//         std::tie(r, c) = seat->getCoordinates();

//         if (r != currentRow) {
//             currentRow = r;
//             QLabel* rLbl = new QLabel(QString::number(r + 1));
//             rLbl->setAlignment(Qt::AlignCenter);
//             rLbl->setStyleSheet("font-weight:700; color:#333;");
//             seatMapLayout_->addWidget(rLbl, layoutRow, 0);
//         }

//         QPushButton* btn = new QPushButton();
//         btn->setFixedSize(28, 28);
//         btn->setCursor(Qt::PointingHandCursor);

//         bool matchClass =
//             (currentClass == BookingClass::Economy  && seat->getType() == SeatType::Economy) ||
//             (currentClass == BookingClass::Business && seat->getType() == SeatType::Business);

//         QString seatCode = QString("%1%2").arg(QChar('A' + c)).arg(r + 1, 2, 10, QChar('0'));

//         QString style;
//         bool enable = false;

//         if (seat->getStatus() == SeatStatus::Booked) {
//             style = BOOKED_STYLE;
//             btn->setToolTip(seatCode + " (Đã đặt)");
//         }
//         else if (!matchClass) {
//             style = LOCKED_STYLE;
//             btn->setToolTip(seatCode + " (Khác hạng)");
//         }
//         else {
//             enable = true;
//             style = (seat->getType() == SeatType::Business) ?
//                         BUSINESS_AVAILABLE_STYLE :
//                         ECONOMY_AVAILABLE_STYLE;

//             btn->setToolTip(seatCode + " (Trống)");
//         }

//         btn->setEnabled(enable);
//         btn->setStyleSheet(style);
//         btn->setProperty("seatId", QString::fromStdString(seat->getId()));
//         btn->setProperty("baseStyle", style);

//         if (enable) {
//             connect(btn, &QPushButton::clicked, this, [this, btn, seatCode]() {
//                 for (int i = 0; i < seatMapLayout_->count(); ++i) {
//                     QWidget* w = seatMapLayout_->itemAt(i)->widget();
//                     QPushButton* b = qobject_cast<QPushButton*>(w);
//                     if (b && b->property("baseStyle").isValid()) {
//                         b->setStyleSheet(b->property("baseStyle").toString());
//                     }
//                 }

//                 btn->setStyleSheet(btn->property("baseStyle").toString() + SELECTED_STYLE_SUFFIX);

//                 selectedSeatId_ = btn->property("seatId").toString();
//                 selectedSeatDisplayLabel_->setText("Bạn đã chọn ghế: " + seatCode);
//                 confirmBtn_->setEnabled(true);
//             });
//         }

//         seatMapLayout_->addWidget(btn, layoutRow, c + 1);

//         if (c == cols - 1) layoutRow++;
//     }
// }

// void ChangeBookingDialog::updatePriceDifference()
// {
//     if (!selectedNewFlight_) return;

//     bool eco = economyRadio_->isChecked();
//     int newFare = eco ? selectedNewFlight_->getFareEconomy() :
//                         selectedNewFlight_->getFareBusiness();

//     int oldFare = currentBooking_->getBaseFare();
//     int diff = newFare - oldFare;

//     fareLabel_->setText(dotFormat(newFare) + " VNĐ");
//     fareLabel_->setStyleSheet(
//         eco
//         ? "font-weight:700; color:#43A047;"
//         : "font-weight:700; color:#FFB300;"
//     );

//     if (diff > 0) {
//         priceDiffLabel_->setText("Phí phát sinh: " + dotFormat(diff) + " VNĐ");
//         priceDiffLabel_->setStyleSheet("color:#1E88E5; font-weight:700;");
//     }
//     else if (diff < 0) {
//         priceDiffLabel_->setText("Hoàn lại: " + dotFormat(-diff) + " VNĐ");
//         priceDiffLabel_->setStyleSheet("color:#D32F2F; font-weight:700;");
//     }
//     else {
//         priceDiffLabel_->setText("Không thay đổi giá");
//         priceDiffLabel_->setStyleSheet("color:#43A047; font-weight:700;");
//     }
// }

// void ChangeBookingDialog::onConfirmClicked()
// {
//     if (!selectedNewFlight_ || selectedSeatId_.isEmpty()) {
//         QMessageBox::warning(this, "Lỗi", "Bạn chưa chọn ghế.");
//         return;
//     }

//     BookingClass bc = economyRadio_->isChecked() ?
//         BookingClass::Economy : BookingClass::Business;

//     SeatManager* sm = flightManager_->getSeatManager();

//     bool ok = bookingManager_->changeBooking(
//         *flightManager_,
//         *sm,
//         currentBooking_->getBookingId(),
//         selectedNewFlight_->getFlightId(),
//         selectedSeatId_.toStdString(),
//         bc
//     );

//     if (ok) {
//         QMessageBox::information(this, "Thành công", "Đổi vé thành công.");
//         accept();
//     } else {
//         QMessageBox::critical(this, "Thất bại", "Đổi vé thất bại.");
//     }
// }

//==== giong ben bookingdialog ================ 

// #include "ChangeBookingDialog.h"
// #include "entities/Booking.h"
// #include "entities/Flight.h"
// #include "core/FlightManager.h"
// #include "core/BookingManager.h"
// #include "core/SeatManager.h"
// #include "entities/Seat.h"

// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QGridLayout>
// #include <QLabel>
// #include <QLineEdit>
// #include <QTextEdit>
// #include <QPushButton>
// #include <QRadioButton>
// #include <QMessageBox>
// #include <QScrollArea>
// #include <QButtonGroup>
// #include <QFrame>
// #include <QDebug>

// // Ensure base style contains transparent border so hover border becomes visible
// const QString ChangeBookingDialog::SEAT_BASE_STYLE =
//     "QPushButton { border: 2px solid transparent; border-radius: 3px; }";

// // include base style into each seat style so hover border works correctly
// const QString ChangeBookingDialog::BUSINESS_AVAILABLE_STYLE =
//     ChangeBookingDialog::SEAT_BASE_STYLE +
//     "QPushButton { background: #FFD740; } "
//     "QPushButton:hover { background: #FFCA28; border: 2px solid #FF6B35; }";

// const QString ChangeBookingDialog::ECONOMY_AVAILABLE_STYLE =
//     ChangeBookingDialog::SEAT_BASE_STYLE +
//     "QPushButton { background: #A5D6A7; } "
//     "QPushButton:hover { background: #81C784; border: 2px solid #FF6B35; }";

// const QString ChangeBookingDialog::BOOKED_STYLE =
//     ChangeBookingDialog::SEAT_BASE_STYLE +
//     "QPushButton { background: #BDBDBD; }";

// const QString ChangeBookingDialog::LOCKED_STYLE =
//     ChangeBookingDialog::SEAT_BASE_STYLE +
//     "QPushButton { background: #EF5350; }";

// const QString ChangeBookingDialog::SELECTED_STYLE_SUFFIX =
//     " border: 2px solid #1E88E5;";

// namespace {
// QString dotFormat(int v) {
//     QString s = QString::number(v);
//     int pos = s.length() - 3;
//     while (pos > 0) {
//         s.insert(pos, '.');
//         pos -= 3;
//     }
//     return s;
// }
// }

// ChangeBookingDialog::ChangeBookingDialog(
//         Booking* currentBooking,
//         BookingManager* bookingManager,
//         FlightManager* flightManager,
//         QWidget *parent)
//     : QDialog(parent),
//       currentBooking_(currentBooking),
//       bookingManager_(bookingManager),
//       flightManager_(flightManager),
//       currentFlight_(nullptr),
//       selectedNewFlight_(nullptr),
//       currentStep_(1),
//       step1Label_(nullptr),
//       line1_(nullptr),
//       step2Label_(nullptr)
// {
//     setWindowTitle("Đổi đặt chỗ");
//     setMinimumSize(800, 650);

//     currentFlight_ = flightManager_->findFlightById(currentBooking_->getFlightId());

//     setupUi();
//     setupConnections();
//     showStep(1);
// }

// void ChangeBookingDialog::setupUi()
// {
//     auto* mainLayout = new QVBoxLayout(this);
//     mainLayout->setContentsMargins(50, 0, 50, 20); // ĐÃ SỬA: Giảm margin top từ 20 xuống 0
//     mainLayout->setSpacing(0);

//     // ---------- header: step indicator ----------
//     QWidget* header = new QWidget(this);
//     header->setFixedHeight(60);
//     header->setStyleSheet("background: white; border: none;");
    
//     QHBoxLayout* headerLayout = new QHBoxLayout(header);
//     headerLayout->setAlignment(Qt::AlignCenter);
//     headerLayout->setContentsMargins(0, 20, 0, 10);
//     headerLayout->setSpacing(0);

//     auto createLabel = [](const QString& text) {
//         QLabel* lbl = new QLabel(text);
//         lbl->setFixedSize(20, 20);
//         lbl->setAlignment(Qt::AlignCenter);
//         lbl->setStyleSheet(
//             "border-radius: 10px; "
//             "font-weight: bold; "
//             "font-size: 14px; "
//             "background-color: white; "
//             "color: white;"
//         );
//         return lbl;
//     };

//     auto createLine = []() {
//         QWidget* line = new QWidget;
//         line->setFixedSize(120, 3);
//         line->setStyleSheet("background-color: #FFFFFF;");
//         return line;
//     };

//     step1Label_ = createLabel("1");
//     line1_ = createLine();
//     step2Label_ = createLabel("2");

//     headerLayout->addWidget(step1Label_);
//     headerLayout->addWidget(line1_);
//     headerLayout->addWidget(step2Label_);

//     mainLayout->addWidget(header);

//     // ---------- body ----------
//     // Step1 container
//     step1Container_ = new QWidget(this);
//     QVBoxLayout* s1 = new QVBoxLayout(step1Container_);
//     s1->setContentsMargins(0, 10, 0, 10); // ĐÃ SỬA: Giảm margin top/bottom từ 20 xuống 10
//     s1->setSpacing(20); // ĐÃ SỬA: Giảm spacing từ 25 xuống 20

//     QLabel* title1 = new QLabel("Chọn chuyến bay mới");
//     title1->setStyleSheet("font-size: 20px; font-weight: bold; color: #1565C0; background: transparent; border: none;");
//     title1->setAlignment(Qt::AlignCenter);
//     s1->addWidget(title1);

//     // input row
//     QHBoxLayout* inputRow = new QHBoxLayout();
//     inputRow->setSpacing(8);
//     QLabel* lblId = new QLabel("Nhập mã chuyến bay:");
//     lblId->setStyleSheet("font-weight: bold; font-size: 13px; color: #1565C0; background: transparent; border: none;");
    
//     flightIdEdit_ = new QLineEdit();
//     flightIdEdit_->setPlaceholderText("FI-XXXXX");
//     flightIdEdit_->setFixedWidth(300);
//     flightIdEdit_->setStyleSheet(
//         "QLineEdit { "
//         "  border: 1px solid #BFCFE0; "
//         "  border-radius: 4px; "
//         "  padding: 10px; "
//         "  font-size: 13px; "
//         "  background: white; "
//         "} "
//         "QLineEdit:focus { "
//         "  border: 2px solid #1976D2; "
//         "  background: white; "
//         "}"
//     );
//     flightIdEdit_->setMinimumHeight(40);
    
//     searchBtn_ = new QPushButton("Tìm kiếm");
//     searchBtn_->setStyleSheet(
//         "QPushButton { background: #608bc1; color: white; border: none; border-radius: 4px; padding: 8px 15px; font-size: 14px; font-weight: 600; }"
//         "QPushButton:hover { background: #1565C0; }"
//     );

//     inputRow->addWidget(lblId);
//     inputRow->addWidget(flightIdEdit_);
//     inputRow->addWidget(searchBtn_);
//     inputRow->addStretch();
//     s1->addLayout(inputRow);

//     // info frame
//     QFrame* infoFrame = new QFrame();
//     infoFrame->setObjectName("InfoFrame");
//     infoFrame->setStyleSheet(
//         "#InfoFrame { "
//         "  background: white; "
//         "  border: 1px solid #BFD7ED; "
//         "  border-radius: 8px; "
//         "}"
//     );
//     infoFrame->setFixedHeight(280);

//     QGridLayout* infoGrid = new QGridLayout(infoFrame);
//     infoGrid->setVerticalSpacing(15);
//     infoGrid->setHorizontalSpacing(30);
//     infoGrid->setContentsMargins(20, 20, 20, 20);

//     infoGrid->setColumnStretch(1, 1);

//     auto addRow = [&](int row, QString label, QString value) {
//         QLabel* lbl = new QLabel(label);
//         lbl->setStyleSheet(
//             "font-weight: 600; "
//             "color: #1E3D58; "
//             "font-size: 13px; "
//             "background: none; "
//             "border: none;"
//         );
        
//         QLabel* val = new QLabel(value);
//         val->setObjectName(QString("finfo_%1").arg(row));
//         val->setStyleSheet(
//             "color: #1E3D58; "
//             "font-size: 13px; "
//             "background: none; "
//             "border: none;"
//         );
        
//         infoGrid->addWidget(lbl, row, 0, Qt::AlignLeft);
//         infoGrid->addWidget(val, row, 1, Qt::AlignLeft);
//     };

//     addRow(0, "Lộ trình bay", "-");
//     addRow(1, "Hãng hàng không", "-");
//     addRow(2, "Số hiệu chuyến bay", "-");
//     addRow(3, "Ngày khởi hành", "-");
//     addRow(4, "Giờ khởi hành", "-");
//     addRow(5, "Ngày hạ cánh", "-");
//     addRow(6, "Giờ hạ cánh", "-");

//     s1->addWidget(infoFrame, 0, Qt::AlignHCenter);
//     s1->addStretch();

//     // next button
//     step1NextBtn_ = new QPushButton("Trang sau");
//     step1NextBtn_->setEnabled(false);
//     step1NextBtn_->setStyleSheet(
//         "QPushButton { background: #608bc1; color: white; border: none; border-radius: 4px; padding: 8px 15px; font-size: 14px; font-weight: 600; }"
//         "QPushButton:hover { background: #1565C0; }"
//         "QPushButton:disabled { background: #cbdceb; }"
//     );
    
//     s1->addWidget(step1NextBtn_, 0, Qt::AlignRight);

//     // Step2 container
//     step2Container_ = new QWidget(this);
//     QVBoxLayout* s2 = new QVBoxLayout(step2Container_);
//     s2->setContentsMargins(0, 10, 0, 10); // ĐÃ SỬA: Giảm margin top/bottom từ 20 xuống 10
//     s2->setSpacing(15);

//     QLabel* title2 = new QLabel("Chọn ghế ngồi");
//     title2->setAlignment(Qt::AlignCenter);
//     title2->setStyleSheet("font-size: 20px; font-weight: bold; color: #1565C0; background: transparent; border: none;");
//     s2->addWidget(title2);

//     // class selector
//     QFrame* classFrame = new QFrame();
//     classFrame->setObjectName("ClassFrame");
//     classFrame->setStyleSheet(
//         "#ClassFrame { "
//         "  background: white; "
//         "  border: 1px solid #608bc1; "
//         "  border-radius: 8px; "
//         "}"
//     );
    
//     QHBoxLayout* classLayout = new QHBoxLayout(classFrame);
//     classLayout->setSpacing(20);
//     classLayout->setContentsMargins(20, 12, 20, 12);
    
//     QLabel* classLabel = new QLabel("Chọn hạng vé");
//     classLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #1565C0; background: transparent; border: none;");
    
//     economyRadio_ = new QRadioButton("Hạng phổ thông");
//     businessRadio_ = new QRadioButton("Hạng thương gia");
//     economyRadio_->setChecked(true);
    
//     QString radioStyle = 
//         "QRadioButton { font-size: 13px; color: #608bc1; background: transparent; spacing: 5px; } "
//         "QRadioButton::indicator { width: 12px; height: 12px; } "
//         "QRadioButton::indicator:unchecked { border: 2px solid #608bc1; border-radius: 6px; background: white; } "
//         "QRadioButton::indicator:checked { border: 2px solid #608bc1; border-radius: 6px; background: #74cc00; }";
    
//     economyRadio_->setStyleSheet(radioStyle);
//     businessRadio_->setStyleSheet(radioStyle);

//     QButtonGroup* classGrp = new QButtonGroup(this);
//     classGrp->addButton(economyRadio_);
//     classGrp->addButton(businessRadio_);

//     fareLabel_ = new QLabel();
//     fareLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #74cc00; background: transparent; border: none;");
    
//     classLayout->addWidget(classLabel);
//     classLayout->addWidget(economyRadio_);
//     classLayout->addWidget(businessRadio_);
//     classLayout->addStretch();
//     classLayout->addWidget(fareLabel_);
//     s2->addWidget(classFrame);

//     // seat map scroll
//     seatMapContainer_ = new QWidget;
//     seatMapLayout_ = new QGridLayout(seatMapContainer_);
//     seatMapLayout_->setSpacing(6);
//     seatMapLayout_->setContentsMargins(10, 10, 10, 10);
//     seatMapLayout_->setAlignment(Qt::AlignCenter);

//     QScrollArea* scroll = new QScrollArea;
//     scroll->setWidget(seatMapContainer_);
//     scroll->setWidgetResizable(true);
//     scroll->setStyleSheet("border:none;");
//     s2->addWidget(scroll, 1);

//     // legend
//     QHBoxLayout* legend = new QHBoxLayout();
//     legend->setSpacing(20);
//     legend->setAlignment(Qt::AlignCenter);

//     auto addLegend = [&](QString color, QString text){
//         QLabel* box = new QLabel;
//         box->setFixedSize(14, 14);
//         box->setStyleSheet("background:" + color + "; border-radius:3px;");
//         QLabel* lab = new QLabel(text);
//         lab->setStyleSheet("font-size: 11px; color: #555; background: transparent; border: none;");
//         QHBoxLayout* h = new QHBoxLayout();
//         h->addWidget(box);
//         h->addWidget(lab);
//         QWidget* w = new QWidget;
//         w->setLayout(h);
//         legend->addWidget(w);
//     };

//     addLegend("#A5D6A7", "Hạng phổ thông");
//     addLegend("#FFD740", "Hạng thương gia");
//     addLegend("#EF5350", "Không thể chọn");
//     addLegend("#BDBDBD", "Đã đặt");
//     s2->addLayout(legend);

//     selectedSeatDisplayLabel_ = new QLabel("Hiện tại chưa chọn ghế");
//     selectedSeatDisplayLabel_->setAlignment(Qt::AlignCenter);
//     selectedSeatDisplayLabel_->setStyleSheet("font-weight:700; color:#1565C0; font-size: 13px;");
//     s2->addWidget(selectedSeatDisplayLabel_);

//     priceDiffLabel_ = new QLabel("");
//     priceDiffLabel_->setAlignment(Qt::AlignCenter);
//     priceDiffLabel_->setStyleSheet("font-size: 13px;");
//     s2->addWidget(priceDiffLabel_);

//     // bottom bar
//     QWidget* bottomBar = new QWidget(this);
//     // Đổi thành background: transparent để nó ăn theo màu nền của cửa sổ
//     bottomBar->setStyleSheet("background: transparent; border: none;"); 
//     QHBoxLayout* bottomLayout = new QHBoxLayout(bottomBar);

//     bottomLayout->setContentsMargins(0, 10, 0, 0); // ĐÃ SỬA: Giảm margin top từ 20 xuống 10

//     QPushButton* cancelBtn = new QPushButton("Hủy đặt vé");
//     cancelBtn->setStyleSheet(
//         "QPushButton { "
//         "  background: #b5c7d7; "
//         "  color: white; "
//         "  border: none; " 
//         "  border-radius: 4px; "
//         "  padding: 8px 15px; "
//         "  font-size: 14px; "
//         "  font-weight: 600; "
//         "} "
//         "QPushButton:hover { background: #8c9aa6ff; }"
//     );

//     connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

//     backBtn_ = new QPushButton("Trang trước");
//     backBtn_->setStyleSheet(
//         "QPushButton { "
//         "  background: white; "
//         "  color: #608bc1; "
//         "  border: 1px solid #608bc1; "
//         "  border-radius: 4px; "
//         "  padding: 7px 15px; "
//         "  font-size: 14px; "
//         "  font-weight: 600; "
//         "} "
//         "QPushButton:hover { background: #c2d1e3ff; }"
//     );
//     backBtn_->hide();

//     confirmBtn_ = new QPushButton("Xác nhận đổi vé");
//     confirmBtn_->setStyleSheet(
//         "QPushButton { "
//         "  background: #608bc1; "
//         "  color: white; "
//         "  border: none; "
//         "  border-radius: 4px; "
//         "  padding: 8px 16px; "
//         "  font-size: 14px; "
//         "  font-weight: 600; "
//         "} "
//         "QPushButton:hover { background: #1565C0; }"
//         "QPushButton:disabled { background: #cbdceb; }"
//     );
//     confirmBtn_->setEnabled(false);
//     confirmBtn_->hide();

//     bottomLayout->addWidget(cancelBtn);
//     bottomLayout->addStretch();
//     bottomLayout->addWidget(backBtn_);
//     bottomLayout->addWidget(step1NextBtn_);
//     bottomLayout->addWidget(confirmBtn_);

//     mainLayout->addWidget(step1Container_, 1);
//     mainLayout->addWidget(step2Container_, 1);
//     mainLayout->addWidget(bottomBar);

//     step2Container_->hide();
// }

// void ChangeBookingDialog::setupConnections()
// {
//     connect(searchBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSearchByIdClicked);
//     connect(step1NextBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSelectFoundFlight);

//     connect(backBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onBackToStep1);
//     connect(economyRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
//     connect(businessRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
//     connect(confirmBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onConfirmClicked);
// }

// QString ChangeBookingDialog::formatWithDot(int v) const { return dotFormat(v); }

// void ChangeBookingDialog::onSearchByIdClicked()
// {
//     QString id = flightIdEdit_->text().trimmed();
//     if (id.isEmpty()) {
//         QMessageBox::warning(this, "Lỗi", "Vui lòng nhập mã chuyến bay.");
//         return;
//     }

//     Flight* f = flightManager_->findFlightById(id.toStdString());
//     if (!f) {
//         QMessageBox::warning(this, "Lỗi", "Không tìm thấy chuyến bay.");
//         return;
//     }

//     selectedNewFlight_ = f;

//     QStringList values = {
//         QString::fromStdString(f->getRouteId()),
//         QString::fromStdString(f->getAirline()),
//         QString::fromStdString(f->getFlightNumber()),
//         QString::fromStdString(f->getDepartureDate()),
//         QString::fromStdString(f->getDepartureTime()),
//         QString::fromStdString(f->getArrivalDate()),
//         QString::fromStdString(f->getArrivalTime())
//     };

//     for (int i = 0; i < values.size(); ++i) {
//         QLabel* v = this->findChild<QLabel*>(QString("finfo_%1").arg(i));
//         if (v) v->setText(values[i]);
//     }

//     step1NextBtn_->setEnabled(true);
// }

// void ChangeBookingDialog::onSelectFoundFlight()
// {
//     if (!selectedNewFlight_) return;

//     economyRadio_->setChecked(true);

//     renderSeatMap();
//     updatePriceDifference();

//     showStep(2);
// }

// void ChangeBookingDialog::showStep(int step)
// {
//     currentStep_ = step;

//     QString activeStyle = 
//         "background-color: #74cc00; "
//         "color: white; "
//         "border: none; "
//         "border-radius: 10px; "
//         "font-weight: bold; "
//         "font-size: 12.5px;";
    
//     QString inactiveStyle = 
//         "background-color: #cbdceb; "
//         "color: white; "
//         "border: none; "
//         "border-radius: 10px; "
//         "font-weight: bold; "
//         "font-size: 12.5px;";
    
//     QString lineActive = "background-color: #74cc00;";
//     QString lineInactive = "background-color: #cbdceb;";

//     step1Label_->setStyleSheet(inactiveStyle);
//     step2Label_->setStyleSheet(inactiveStyle);
//     line1_->setStyleSheet(lineInactive);

//     if (step >= 1) step1Label_->setStyleSheet(activeStyle);
//     if (step >= 2) {
//         line1_->setStyleSheet(lineActive);
//         step2Label_->setStyleSheet(activeStyle);
//     }

//     if (step == 1) {
//         step1Container_->show();
//         step2Container_->hide();

//         step1NextBtn_->show();
//         backBtn_->hide();
//         confirmBtn_->hide();
//     }
//     else {
//         step1Container_->hide();
//         step2Container_->show();

//         step1NextBtn_->hide();
//         backBtn_->show();
//         confirmBtn_->show();
//     }
// }

// void ChangeBookingDialog::onBackToStep1()
// {
//     showStep(1);
// }

// void ChangeBookingDialog::onClassChanged()
// {
//     selectedSeatId_.clear();
//     selectedSeatDisplayLabel_->setText("Hiện tại chưa chọn ghế");
//     confirmBtn_->setEnabled(false);

//     renderSeatMap();
//     updatePriceDifference();
// }

// void ChangeBookingDialog::renderSeatMap()
// {
//     while (seatMapLayout_->count()) {
//         QLayoutItem* it = seatMapLayout_->takeAt(0);
//         if (it->widget()) it->widget()->deleteLater();
//         delete it;
//     }

//     if (!selectedNewFlight_) return;

//     SeatManager* sm = flightManager_->getSeatManager();
//     if (!sm) return;

//     if (!sm->loadSeatMapFor(selectedNewFlight_)) {
//         QLabel* err = new QLabel("Không thể tải sơ đồ ghế");
//         seatMapLayout_->addWidget(err, 0, 0);
//         return;
//     }

//     auto* seatMap = sm->getActiveSeatMap();
//     if (!seatMap) return;

//     int cols = sm->getSeatColumns();
//     BookingClass currentClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;

//     // Add column headers
//     for (int col = 0; col < cols; ++col) {
//         QLabel* header = new QLabel(QString(QChar('A' + col)));
//         header->setAlignment(Qt::AlignCenter);
//         header->setStyleSheet("font-weight:700; color:#333;");
//         seatMapLayout_->addWidget(header, 0, col + 1);
//     }

//     int currentRow = -1;
//     int layoutRow = 1;

//     for (Seat* seat : *seatMap) {
//         int r, c;
//         std::tie(r, c) = seat->getCoordinates();

//         if (r != currentRow) {
//             currentRow = r;
//             QLabel* rLbl = new QLabel(QString::number(r + 1));
//             rLbl->setAlignment(Qt::AlignCenter);
//             rLbl->setStyleSheet("font-weight:700; color:#333;");
//             seatMapLayout_->addWidget(rLbl, layoutRow, 0);
//         }

//         QPushButton* btn = new QPushButton();
//         btn->setFixedSize(28, 28);
//         btn->setCursor(Qt::PointingHandCursor);

//         bool matchClass =
//             (currentClass == BookingClass::Economy  && seat->getType() == SeatType::Economy) ||
//             (currentClass == BookingClass::Business && seat->getType() == SeatType::Business);

//         QString seatCode = QString("%1%2").arg(QChar('A' + c)).arg(r + 1, 2, 10, QChar('0'));

//         QString style;
//         bool enable = false;

//         if (seat->getStatus() == SeatStatus::Booked) {
//             style = BOOKED_STYLE;
//             btn->setToolTip(seatCode + " (Đã đặt)");
//         }
//         else if (!matchClass) {
//             style = LOCKED_STYLE;
//             btn->setToolTip(seatCode + " (Khác hạng)");
//         }
//         else {
//             enable = true;
//             style = (seat->getType() == SeatType::Business) ?
//                         BUSINESS_AVAILABLE_STYLE :
//                         ECONOMY_AVAILABLE_STYLE;

//             btn->setToolTip(seatCode + " (Trống)");
//         }

//         btn->setEnabled(enable);
//         btn->setStyleSheet(style);
//         btn->setProperty("seatId", QString::fromStdString(seat->getId()));
//         btn->setProperty("baseStyle", style);

//         if (enable) {
//             connect(btn, &QPushButton::clicked, this, [this, btn, seatCode]() {
//                 for (int i = 0; i < seatMapLayout_->count(); ++i) {
//                     QWidget* w = seatMapLayout_->itemAt(i)->widget();
//                     QPushButton* b = qobject_cast<QPushButton*>(w);
//                     if (b && b->property("baseStyle").isValid()) {
//                         b->setStyleSheet(b->property("baseStyle").toString());
//                     }
//                 }

//                 btn->setStyleSheet(btn->property("baseStyle").toString() + SELECTED_STYLE_SUFFIX);

//                 selectedSeatId_ = btn->property("seatId").toString();
//                 selectedSeatDisplayLabel_->setText("Bạn đã chọn ghế: " + seatCode);
//                 confirmBtn_->setEnabled(true);
//             });
//         }

//         seatMapLayout_->addWidget(btn, layoutRow, c + 1);

//         if (c == cols - 1) layoutRow++;
//     }
// }

// void ChangeBookingDialog::updatePriceDifference()
// {
//     if (!selectedNewFlight_) return;

//     bool eco = economyRadio_->isChecked();
//     int newFare = eco ? selectedNewFlight_->getFareEconomy() :
//                         selectedNewFlight_->getFareBusiness();

//     int oldFare = currentBooking_->getBaseFare();
//     int diff = newFare - oldFare;

//     fareLabel_->setText(dotFormat(newFare) + " VNĐ");
//     fareLabel_->setStyleSheet(
//         eco
//         ? "font-weight:700; color:#43A047;"
//         : "font-weight:700; color:#FFB300;"
//     );

//     if (diff > 0) {
//         priceDiffLabel_->setText("Phí phát sinh: " + dotFormat(diff) + " VNĐ");
//         priceDiffLabel_->setStyleSheet("color:#1E88E5; font-weight:700;");
//     }
//     else if (diff < 0) {
//         priceDiffLabel_->setText("Hoàn lại: " + dotFormat(-diff) + " VNĐ");
//         priceDiffLabel_->setStyleSheet("color:#D32F2F; font-weight:700;");
//     }
//     else {
//         priceDiffLabel_->setText("Không thay đổi giá");
//         priceDiffLabel_->setStyleSheet("color:#43A047; font-weight:700;");
//     }
// }

// void ChangeBookingDialog::onConfirmClicked()
// {
//     if (!selectedNewFlight_ || selectedSeatId_.isEmpty()) {
//         QMessageBox::warning(this, "Lỗi", "Bạn chưa chọn ghế.");
//         return;
//     }

//     BookingClass bc = economyRadio_->isChecked() ?
//         BookingClass::Economy : BookingClass::Business;

//     SeatManager* sm = flightManager_->getSeatManager();

//     bool ok = bookingManager_->changeBooking(
//         *flightManager_,
//         *sm,
//         currentBooking_->getBookingId(),
//         selectedNewFlight_->getFlightId(),
//         selectedSeatId_.toStdString(),
//         bc
//     );

//     if (ok) {
//         QMessageBox::information(this, "Thành công", "Đổi vé thành công.");
//         accept();
//     } else {
//         QMessageBox::critical(this, "Thất bại", "Đổi vé thất bại.");
//     }
// }

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
#include <QDebug>

// Ensure base style contains transparent border so hover border becomes visible
const QString ChangeBookingDialog::SEAT_BASE_STYLE =
    "QPushButton { border: 2px solid transparent; border-radius: 3px; }";

// include base style into each seat style so hover border works correctly
const QString ChangeBookingDialog::BUSINESS_AVAILABLE_STYLE =
    ChangeBookingDialog::SEAT_BASE_STYLE +
    "QPushButton { background: #FFD740; } "
    "QPushButton:hover { background: #FFCA28; border: 2px solid #FF6B35; }";

const QString ChangeBookingDialog::ECONOMY_AVAILABLE_STYLE =
    ChangeBookingDialog::SEAT_BASE_STYLE +
    "QPushButton { background: #A5D6A7; } "
    "QPushButton:hover { background: #81C784; border: 2px solid #FF6B35; }";

const QString ChangeBookingDialog::BOOKED_STYLE =
    ChangeBookingDialog::SEAT_BASE_STYLE +
    "QPushButton { background: #BDBDBD; }";

const QString ChangeBookingDialog::LOCKED_STYLE =
    ChangeBookingDialog::SEAT_BASE_STYLE +
    "QPushButton { background: #EF5350; }";

const QString ChangeBookingDialog::SELECTED_STYLE_SUFFIX =
    " border: 2px solid #1E88E5;";

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
    setMinimumSize(800, 650);

    currentFlight_ = flightManager_->findFlightById(currentBooking_->getFlightId());

    setupUi();
    setupConnections();
    showStep(1);
}

void ChangeBookingDialog::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 0, 50, 20);
    mainLayout->setSpacing(0);

    // ---------- header: step indicator ----------
    QWidget* header = new QWidget(this);
    header->setFixedHeight(60);
    header->setStyleSheet("background: transparent; border: none;");
    
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setAlignment(Qt::AlignCenter);
    headerLayout->setContentsMargins(0, 20, 0, 10);
    headerLayout->setSpacing(0);

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

    headerLayout->addWidget(step1Label_);
    headerLayout->addWidget(line1_);
    headerLayout->addWidget(step2Label_);

    mainLayout->addWidget(header);

    // ---------- body ----------
    // Step1 container
    step1Container_ = new QWidget(this);
    QVBoxLayout* s1 = new QVBoxLayout(step1Container_);
    s1->setContentsMargins(0, 20, 0, 20);
    s1->setSpacing(25);

    QLabel* title1 = new QLabel("Chọn chuyến bay mới");
    title1->setStyleSheet("font-size: 20px; font-weight: bold; color: #1565C0; background: transparent; border: none;");
    title1->setAlignment(Qt::AlignCenter);
    s1->addWidget(title1);

    // --- INPUT ROW (Đã căn giữa) ---
    QHBoxLayout* inputRowWrapper = new QHBoxLayout();
    inputRowWrapper->addStretch(); // Đẩy từ trái

    QLabel* lblId = new QLabel("Nhập mã chuyến bay:");
    lblId->setStyleSheet("font-weight: bold; font-size: 13px; color: #1565C0; background: transparent; border: none;");
    
    flightIdEdit_ = new QLineEdit();
    flightIdEdit_->setPlaceholderText("FI-XXXXX");
    flightIdEdit_->setFixedWidth(300);
    flightIdEdit_->setStyleSheet(
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
        "}"
    );
    flightIdEdit_->setMinimumHeight(40);
    
    searchBtn_ = new QPushButton("Tìm kiếm");
    searchBtn_->setStyleSheet(
        "QPushButton { background: #608bc1; color: white; border: none; border-radius: 4px; padding: 8px 15px; font-size: 14px; font-weight: 600; }"
        "QPushButton:hover { background: #1565C0; }"
    );

    inputRowWrapper->addWidget(lblId);
    inputRowWrapper->addWidget(flightIdEdit_);
    inputRowWrapper->addWidget(searchBtn_);
    
    inputRowWrapper->addStretch(); // Đẩy từ phải -> Kết quả là nằm giữa
    s1->addLayout(inputRowWrapper);

    // --- INFO FRAME (Đã đồng bộ style với BookingDialog và căn giữa) ---
    QHBoxLayout* tableCenteringLayout = new QHBoxLayout();
    tableCenteringLayout->addStretch(); // Spacer trái

    QFrame* infoFrame = new QFrame();
    infoFrame->setObjectName("InfoFrame");
    infoFrame->setFixedWidth(600); // Giữ độ rộng cố định cho đẹp
    // STYLE CẬP NHẬT THEO BOOKINGDIALOG
    infoFrame->setStyleSheet(
        "#InfoFrame { "
        "  background: white; "
        "  border: 1px solid #BFCFE0; " // Màu viền nhạt hơn (#BFCFE0) giống file kia
        "  border-radius: 8px; "
        "}"
    );
    infoFrame->setFixedHeight(280);

    QGridLayout* infoGrid = new QGridLayout(infoFrame);
    infoGrid->setVerticalSpacing(15);
    infoGrid->setHorizontalSpacing(30);
    infoGrid->setContentsMargins(20, 20, 20, 20);

    // Quan trọng: Stretch cột 1 để đẩy text sang 2 bên
    infoGrid->setColumnStretch(1, 1); 

    auto addRow = [&](int row, QString label, QString value) {
        QLabel* lbl = new QLabel(label);
        // STYLE LABEL MỚI (#1565C0)
        lbl->setStyleSheet(
            "font-weight: bold; "
            "color: #1565C0; " 
            "font-size: 13px; "
            "background: none; "
            "border: none;"
        );
        
        QLabel* val = new QLabel(value);
        val->setObjectName(QString("finfo_%1").arg(row));
        // STYLE VALUE MỚI (#333)
        val->setStyleSheet(
            "color: #333; "
            "font-size: 13px; "
            "background: none; "
            "border: none;"
        );
        
        // Căn lề Label trái, Value phải (Giống BookingDialog)
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

    tableCenteringLayout->addWidget(infoFrame);
    tableCenteringLayout->addStretch(); // Spacer phải -> Bảng nằm chính giữa
    s1->addLayout(tableCenteringLayout);

    s1->addStretch();

    // next button
    QHBoxLayout* step1Btns = new QHBoxLayout();
    step1Btns->addStretch();
    
    step1NextBtn_ = new QPushButton("Trang sau");
    step1NextBtn_->setEnabled(false);
    step1NextBtn_->setStyleSheet(
        "QPushButton { background: #608bc1; color: white; border: none; border-radius: 4px; padding: 8px 15px; font-size: 14px; font-weight: 600; }"
        "QPushButton:hover { background: #1565C0; }"
        "QPushButton:disabled { background: #cbdceb; }"
    );
    step1Btns->addWidget(step1NextBtn_);
    // Thêm spacing để nút next thẳng hàng với cạnh phải bảng thông tin (khoảng 100px vì bảng rộng 600 trên tổng 800)
    step1Btns->addSpacing(50); 

    s1->addLayout(step1Btns);

    // Step2 container
    step2Container_ = new QWidget(this);
    QVBoxLayout* s2 = new QVBoxLayout(step2Container_);
    s2->setContentsMargins(0, 20, 0, 20);
    s2->setSpacing(15);

    QLabel* title2 = new QLabel("Chọn ghế ngồi");
    title2->setAlignment(Qt::AlignCenter);
    title2->setStyleSheet("font-size: 20px; font-weight: bold; color: #1565C0; background: transparent; border: none;");
    s2->addWidget(title2);

    // class selector
    QFrame* classFrame = new QFrame();
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
    
    QLabel* classLabel = new QLabel("Chọn hạng vé");
    classLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #1565C0; background: transparent; border: none;");
    
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

    QButtonGroup* classGrp = new QButtonGroup(this);
    classGrp->addButton(economyRadio_);
    classGrp->addButton(businessRadio_);

    fareLabel_ = new QLabel();
    fareLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #74cc00; background: white; border: none;");
    
    classLayout->addWidget(classLabel);
    classLayout->addWidget(economyRadio_);
    classLayout->addWidget(businessRadio_);
    classLayout->addStretch();
    classLayout->addWidget(fareLabel_);
    s2->addWidget(classFrame);

    // seat map scroll
    seatMapContainer_ = new QWidget;
    seatMapLayout_ = new QGridLayout(seatMapContainer_);
    seatMapLayout_->setSpacing(6);
    seatMapLayout_->setContentsMargins(10, 10, 10, 10);
    seatMapLayout_->setAlignment(Qt::AlignCenter);

    QScrollArea* scroll = new QScrollArea;
    scroll->setWidget(seatMapContainer_);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("border:none; background: transparent;");
    s2->addWidget(scroll, 1);

    // legend
    QHBoxLayout* legend = new QHBoxLayout();
    legend->setSpacing(20);
    legend->setAlignment(Qt::AlignCenter);

    auto addLegend = [&](QString color, QString text){
        QLabel* box = new QLabel;
        box->setFixedSize(14, 14);
        box->setStyleSheet("background:" + color + "; border-radius:3px;");
        QLabel* lab = new QLabel(text);
        lab->setStyleSheet("font-size: 11px; color: #555; background: transparent; border: none;");
        QHBoxLayout* h = new QHBoxLayout();
        h->addWidget(box);
        h->addWidget(lab);
        QWidget* w = new QWidget;
        w->setLayout(h);
        legend->addWidget(w);
    };

    addLegend("#A5D6A7", "Hạng phổ thông");
    addLegend("#FFD740", "Hạng thương gia");
    addLegend("#EF5350", "Không thể chọn");
    addLegend("#BDBDBD", "Đã đặt");
    s2->addLayout(legend);

    selectedSeatDisplayLabel_ = new QLabel("Hiện tại chưa chọn ghế");
    selectedSeatDisplayLabel_->setAlignment(Qt::AlignCenter);
    selectedSeatDisplayLabel_->setStyleSheet("font-weight:700; color:#1565C0; font-size: 13px;");
    s2->addWidget(selectedSeatDisplayLabel_);

    priceDiffLabel_ = new QLabel("");
    priceDiffLabel_->setAlignment(Qt::AlignCenter);
    priceDiffLabel_->setStyleSheet("font-size: 13px;");
    s2->addWidget(priceDiffLabel_);

    // bottom bar
    QWidget* bottomBar = new QWidget(this);
    bottomBar->setStyleSheet("background: transparent; border: none;");
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton* cancelBtn = new QPushButton("Hủy đặt vé");
    cancelBtn->setStyleSheet(
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

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    backBtn_ = new QPushButton("Trang trước");
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
    backBtn_->hide();

    confirmBtn_ = new QPushButton("Xác nhận đổi vé");
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
        "QPushButton:disabled { background: #cbdceb; }"
    );
    confirmBtn_->setEnabled(false);
    confirmBtn_->hide();

    step1NextBtn_->setParent(bottomBar);

    bottomLayout->addWidget(cancelBtn);
    bottomLayout->addStretch();
    bottomLayout->addWidget(backBtn_);
    bottomLayout->addWidget(step1NextBtn_);
    bottomLayout->addWidget(confirmBtn_);

    mainLayout->addWidget(step1Container_);
    mainLayout->addWidget(step2Container_);
    mainLayout->addStretch();
    mainLayout->addWidget(bottomBar);

    step2Container_->hide();
}

void ChangeBookingDialog::setupConnections()
{
    connect(searchBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSearchByIdClicked);
    connect(step1NextBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onSelectFoundFlight);

    connect(backBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onBackToStep1);
    connect(economyRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
    connect(businessRadio_, &QRadioButton::toggled, this, &ChangeBookingDialog::onClassChanged);
    connect(confirmBtn_, &QPushButton::clicked, this, &ChangeBookingDialog::onConfirmClicked);
}

QString ChangeBookingDialog::formatWithDot(int v) const { return dotFormat(v); }

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

void ChangeBookingDialog::onSelectFoundFlight()
{
    if (!selectedNewFlight_) return;

    economyRadio_->setChecked(true);

    renderSeatMap();
    updatePriceDifference();

    showStep(2);
}

void ChangeBookingDialog::showStep(int step)
{
    currentStep_ = step;

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

    if (step >= 1) step1Label_->setStyleSheet(activeStyle);
    if (step >= 2) {
        line1_->setStyleSheet(lineActive);
        step2Label_->setStyleSheet(activeStyle);
    }

    if (step == 1) {
        step1Container_->show();
        step2Container_->hide();

        step1NextBtn_->show();
        backBtn_->hide();
        confirmBtn_->hide();
    }
    else {
        step1Container_->hide();
        step2Container_->show();

        step1NextBtn_->hide();
        backBtn_->show();
        confirmBtn_->show();
    }
}

void ChangeBookingDialog::onBackToStep1()
{
    showStep(1);
}

void ChangeBookingDialog::onClassChanged()
{
    selectedSeatId_.clear();
    selectedSeatDisplayLabel_->setText("Hiện tại chưa chọn ghế");
    confirmBtn_->setEnabled(false);

    renderSeatMap();
    updatePriceDifference();
}

void ChangeBookingDialog::renderSeatMap()
{
    while (seatMapLayout_->count()) {
        QLayoutItem* it = seatMapLayout_->takeAt(0);
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }

    if (!selectedNewFlight_) return;

    SeatManager* sm = flightManager_->getSeatManager();
    if (!sm) return;

    if (!sm->loadSeatMapFor(selectedNewFlight_)) {
        QLabel* err = new QLabel("Không thể tải sơ đồ ghế");
        seatMapLayout_->addWidget(err, 0, 0);
        return;
    }

    auto* seatMap = sm->getActiveSeatMap();
    if (!seatMap) return;

    int cols = sm->getSeatColumns();
    BookingClass currentClass = economyRadio_->isChecked() ? BookingClass::Economy : BookingClass::Business;

    // Add column headers
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

        if (r != currentRow) {
            currentRow = r;
            QLabel* rLbl = new QLabel(QString::number(r + 1));
            rLbl->setAlignment(Qt::AlignCenter);
            rLbl->setStyleSheet("font-weight:700; color:#333;");
            seatMapLayout_->addWidget(rLbl, layoutRow, 0);
        }

        QPushButton* btn = new QPushButton();
        btn->setFixedSize(28, 28);
        btn->setCursor(Qt::PointingHandCursor);

        bool matchClass =
            (currentClass == BookingClass::Economy  && seat->getType() == SeatType::Economy) ||
            (currentClass == BookingClass::Business && seat->getType() == SeatType::Business);

        QString seatCode = QString("%1%2").arg(QChar('A' + c)).arg(r + 1, 2, 10, QChar('0'));

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
        btn->setProperty("seatId", QString::fromStdString(seat->getId()));
        btn->setProperty("baseStyle", style);

        if (enable) {
            connect(btn, &QPushButton::clicked, this, [this, btn, seatCode]() {
                for (int i = 0; i < seatMapLayout_->count(); ++i) {
                    QWidget* w = seatMapLayout_->itemAt(i)->widget();
                    QPushButton* b = qobject_cast<QPushButton*>(w);
                    if (b && b->property("baseStyle").isValid()) {
                        b->setStyleSheet(b->property("baseStyle").toString());
                    }
                }

                btn->setStyleSheet(btn->property("baseStyle").toString() + SELECTED_STYLE_SUFFIX);

                selectedSeatId_ = btn->property("seatId").toString();
                selectedSeatDisplayLabel_->setText("Bạn đã chọn ghế: " + seatCode);
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

    fareLabel_->setText(dotFormat(newFare) + " VNĐ");
    
    fareLabel_->setStyleSheet(
        eco
        ? "font-weight:700; color:#43A047; background: white; border: none;"
        : "font-weight:700; color:#FFB300; background: white; border: none;"
    );

    if (diff > 0) {
        priceDiffLabel_->setText("Phí phát sinh: " + dotFormat(diff) + " VNĐ");
        priceDiffLabel_->setStyleSheet("color:#1E88E5; font-weight:700;");
    }
    else if (diff < 0) {
        priceDiffLabel_->setText("Hoàn lại: " + dotFormat(-diff) + " VNĐ");
        priceDiffLabel_->setStyleSheet("color:#D32F2F; font-weight:700;");
    }
    else {
        priceDiffLabel_->setText("Không thay đổi giá");
        priceDiffLabel_->setStyleSheet("color:#43A047; font-weight:700;");
    }
}

void ChangeBookingDialog::onConfirmClicked()
{
    if (!selectedNewFlight_ || selectedSeatId_.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Bạn chưa chọn ghế.");
        return;
    }

    BookingClass bc = economyRadio_->isChecked() ?
        BookingClass::Economy : BookingClass::Business;

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
        QMessageBox::critical(this, "Thất bại", "Đổi vé thất bại.");
    }
}