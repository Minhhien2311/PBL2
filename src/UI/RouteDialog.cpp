#include "RouteDialog.h"
#include "AirportComboBox.h"
#include "core/AirportManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>

// Constructor cho THÊM MỚI
RouteDialog::RouteDialog(AirportManager* airportManager, QWidget* parent)
    : QDialog(parent),
      airportManager_(airportManager),
      isEditMode_(false)
{
    setupUi(false);
}

// Constructor cho SỬA
RouteDialog::RouteDialog(AirportManager* airportManager,
                         const QString& currentId,
                         const QString& currentAirline,
                         const QString& currentFrom,
                         const QString& currentTo,
                         QWidget* parent)
    : QDialog(parent),
      airportManager_(airportManager),
      isEditMode_(true),
      currentId_(currentId)
{
    setupUi(true);
    
    // Set giá trị hiện tại
    int airlineIndex = airlineCombo_->findText(currentAirline);
    if (airlineIndex >= 0) {
        airlineCombo_->setCurrentIndex(airlineIndex);
    }
    
    fromCombo_->setSelectedIATA(currentFrom.toStdString());
    toCombo_->setSelectedIATA(currentTo.toStdString());
}

void RouteDialog::setupUi(bool isEditMode)
{
    setWindowTitle(isEditMode ? "Sửa tuyến bay" : "Thêm tuyến bay mới");
    setMinimumWidth(500);
    
    // Style
    setStyleSheet(
        "QDialog { background: #F2F6FD; }"
        "QLabel { color: #123B7A; font-weight: 600; }"
        "QLineEdit, QComboBox { "
        "  background: white; "
        "  border: 1px solid #608bc1; "
        "  border-radius: 4px; "
        "  height: 32px; "
        "  padding-left: 8px; "
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
        "QPushButton#cancelBtn { background: #999; }"
        "QPushButton#cancelBtn:hover { background: #777; }"
    );

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Tiêu đề
    QLabel* titleLabel = new QLabel(isEditMode ? "Chỉnh sửa thông tin tuyến bay" : "Nhập thông tin tuyến bay mới");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #123B7A;");
    mainLayout->addWidget(titleLabel);

    // Form nhập liệu
    auto* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight);

    // Hãng hàng không (Dropdown)
    airlineCombo_ = new QComboBox();
    airlineCombo_->addItem("-- Chọn hãng hàng không --", "");
    airlineCombo_->addItem("Vietnam Airlines", "Vietnam Airlines");
    airlineCombo_->addItem("VietJet Air", "VietJet Air");
    airlineCombo_->addItem("Bamboo Airways", "Bamboo Airways");
    airlineCombo_->addItem("Vietravel Airlines", "Vietravel Airlines");
    airlineCombo_->addItem("Pacific Airlines", "Pacific Airlines");
    formLayout->addRow("Hãng hàng không:", airlineCombo_);

    // Điểm đi
    fromCombo_ = new AirportComboBox(airportManager_);
    formLayout->addRow("Điểm đi:", fromCombo_);

    // Điểm đến
    toCombo_ = new AirportComboBox(airportManager_);
    formLayout->addRow("Điểm đến:", toCombo_);

    mainLayout->addLayout(formLayout);

    // Thông báo (nếu edit)
    if (isEditMode) {
        QLabel* noteLabel = new QLabel(
            QString("⚠️ <b>Lưu ý:</b> Đang sửa tuyến <b>%1</b>").arg(currentId_)
        );
        noteLabel->setStyleSheet("color: #d97706; font-size: 12px;");
        noteLabel->setWordWrap(true);
        mainLayout->addWidget(noteLabel);
    }

    mainLayout->addStretch();

    // Nút bấm
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    cancelButton_ = new QPushButton("Hủy");
    cancelButton_->setObjectName("cancelBtn");
    
    saveButton_ = new QPushButton(isEditMode ? "💾 Lưu thay đổi" : "➕ Thêm tuyến");

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton_);
    buttonLayout->addWidget(saveButton_);

    mainLayout->addLayout(buttonLayout);

    // Kết nối signals
    connect(saveButton_, &QPushButton::clicked, this, &RouteDialog::onAccept);
    connect(cancelButton_, &QPushButton::clicked, this, &QDialog::reject);
}

void RouteDialog::onAccept()
{
    // Validate
    if (airlineCombo_->currentIndex() == 0) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng chọn hãng hàng không.");
        return;
    }

    std::string fromIATA = fromCombo_->getSelectedIATA();
    std::string toIATA = toCombo_->getSelectedIATA();

    if (fromIATA.empty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng chọn điểm đi.");
        return;
    }

    if (toIATA.empty()) {
        QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng chọn điểm đến.");
        return;
    }

    if (fromIATA == toIATA) {
        QMessageBox::warning(this, "Dữ liệu không hợp lệ", 
            "Điểm đi và điểm đến phải khác nhau.");
        return;
    }

    accept();
}

// Getters
QString RouteDialog::getAirline() const {
    return airlineCombo_->currentData().toString();
}

QString RouteDialog::getFromIATA() const {
    return QString::fromStdString(fromCombo_->getSelectedIATA());
}

QString RouteDialog::getToIATA() const {
    return QString::fromStdString(toCombo_->getSelectedIATA());
}