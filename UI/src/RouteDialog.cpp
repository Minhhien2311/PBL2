#include "RouteDialog.h"
#include "AirportComboBox.h"
#include "core/AirportManager.h"
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QThread>
#include <QVBoxLayout>

RouteDialog::RouteDialog(AirportManager* airportManager, QWidget* parent)
    : QDialog(parent),
      airportManager_(airportManager),
      isEditMode_(false)
{
    setupUi(false);
}

RouteDialog::RouteDialog(AirportManager* airportManager,
                         const QString& currentId,
                         const QString& currentFrom,
                         const QString& currentTo,
                         QWidget* parent)
    : QDialog(parent),
      airportManager_(airportManager),
      isEditMode_(true),
      currentId_(currentId)
{
    setupUi(true);
    fromCombo_->setSelectedIATA(currentFrom.toStdString());
    toCombo_->setSelectedIATA(currentTo.toStdString());
}

void RouteDialog::setupUi(bool isEditMode)
{
    setWindowTitle(isEditMode ? "Sửa tuyến bay" : "Thêm tuyến bay mới");
    setMinimumWidth(500);
    
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

    QLabel* titleLabel = new QLabel(isEditMode ? "Chỉnh sửa thông tin tuyến bay" : "Nhập thông tin tuyến bay mới");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #123B7A;");
    mainLayout->addWidget(titleLabel);

    auto* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight);

    fromCombo_ = new AirportComboBox(airportManager_);
    formLayout->addRow("Điểm đi:", fromCombo_);

    toCombo_ = new AirportComboBox(airportManager_);
    formLayout->addRow("Điểm đến:", toCombo_);

    mainLayout->addLayout(formLayout);

    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #d0342c; font-size: 13px; font-weight: 650;");
    mainLayout->addWidget(statusLabel_);

    if (isEditMode) {
        QLabel* noteLabel = new QLabel(
            QString("⚠️ <b>Lưu ý:</b> Đang sửa tuyến <b>%1</b>").arg(currentId_)
        );
        noteLabel->setStyleSheet("color: #d97706; font-size: 13px;");
        noteLabel->setWordWrap(true);
        mainLayout->addWidget(noteLabel);
    }

    mainLayout->addStretch();

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    cancelButton_ = new QPushButton("Hủy");
    cancelButton_->setObjectName("cancelBtn");
    
    saveButton_ = new QPushButton(isEditMode ? "💾 Lưu thay đổi" : "➕ Thêm tuyến");

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton_);
    buttonLayout->addWidget(saveButton_);

    mainLayout->addLayout(buttonLayout);

    connect(saveButton_, &QPushButton::clicked, this, &RouteDialog::onAccept);
    connect(cancelButton_, &QPushButton::clicked, this, &QDialog::reject);
}

void RouteDialog::onAccept()
{
    bool valid = true;
    statusLabel_->setText("");
    std::string fromIATA = fromCombo_->getSelectedIATA();
    std::string toIATA = toCombo_->getSelectedIATA();

    if (fromIATA.empty()) {
        statusLabel_->setText("Vui lòng chọn điểm đi!");
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
        valid = false;
    }

    if (toIATA.empty()) {
        statusLabel_->setText("Vui lòng chọn điểm đến!");
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
        valid = false;
    }

    if (fromIATA == toIATA) {
        statusLabel_->setText("Điểm đi và điểm đến phải khác nhau!");
        statusLabel_->setStyleSheet("color: #FF5F57; font-size: 13px; font-weight: 650;");
        valid = false;
    }

    if (valid) accept();
    else return;
}

QString RouteDialog::getFromIATA() const {
    return QString::fromStdString(fromCombo_->getSelectedIATA());
}

QString RouteDialog::getToIATA() const {
    return QString::fromStdString(toCombo_->getSelectedIATA());
}