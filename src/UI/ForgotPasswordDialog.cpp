#include "ForgotPasswordDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

ForgotPasswordDialog::ForgotPasswordDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Quên mật khẩu"));
    setModal(true);
    setFixedSize(400, 300);

    // Apply styling
    setStyleSheet(
        "QDialog { background: white; }"
        "QLabel { color: #133e87; font-weight: 600; }"
        "QLineEdit { height: 30px; background: #f5f8fb; border: 1px solid #133e87; border-radius: 3px; padding-left: 10px; }"
        "QPushButton { background: #608bc1; color: white; height: 35px; border-radius: 3px; font-weight: 600; min-width: 100px; }"
        "QPushButton:hover { background: #476c9a; }"
        "QPushButton#CancelButton { background: #6c757d; }"
        "QPushButton#CancelButton:hover { background: #5a6268; }"
    );

    QLabel *lblEmail = new QLabel(tr("Email:"));
    QLabel *lblNew = new QLabel(tr("Mật khẩu mới:"));
    QLabel *lblConfirm = new QLabel(tr("Xác nhận mật khẩu:"));

    emailEdit_ = new QLineEdit();
    emailEdit_->setPlaceholderText("Nhập email của bạn");
    
    newPasswordEdit_ = new QLineEdit();
    newPasswordEdit_->setPlaceholderText("Nhập mật khẩu mới");
    newPasswordEdit_->setEchoMode(QLineEdit::Password);
    
    confirmPasswordEdit_ = new QLineEdit();
    confirmPasswordEdit_->setPlaceholderText("Nhập lại mật khẩu mới");
    confirmPasswordEdit_->setEchoMode(QLineEdit::Password);

    resetButton_ = new QPushButton(tr("Đặt lại mật khẩu"));
    cancelButton_ = new QPushButton(tr("Hủy"));
    cancelButton_->setObjectName("CancelButton");

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(lblEmail, 0, 0);
    gridLayout->addWidget(emailEdit_, 0, 1);
    gridLayout->addWidget(lblNew, 1, 0);
    gridLayout->addWidget(newPasswordEdit_, 1, 1);
    gridLayout->addWidget(lblConfirm, 2, 0);
    gridLayout->addWidget(confirmPasswordEdit_, 2, 1);

    gridLayout->setVerticalSpacing(15);
    gridLayout->setHorizontalSpacing(10);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(resetButton_);
    buttonLayout->addWidget(cancelButton_);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(gridLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);

    connect(resetButton_, &QPushButton::clicked, this, &ForgotPasswordDialog::onResetClicked);
    connect(cancelButton_, &QPushButton::clicked, this, &ForgotPasswordDialog::onCancelClicked);
}

void ForgotPasswordDialog::onResetClicked()
{
    QString email = emailEdit_->text().trimmed();
    QString newPassword = newPasswordEdit_->text();
    QString confirmPassword = confirmPasswordEdit_->text();

    // Validation
    if (email.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, tr("Lỗi"), tr("Vui lòng nhập đầy đủ thông tin"));
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, tr("Lỗi"), tr("Mật khẩu mới và xác nhận không khớp"));
        return;
    }

    if (newPassword.length() < 6) {
        QMessageBox::warning(this, tr("Lỗi"), tr("Mật khẩu phải có ít nhất 6 ký tự"));
        return;
    }

    accept();
}

QString ForgotPasswordDialog::getEmail() const { return emailEdit_->text().trimmed(); }
QString ForgotPasswordDialog::getNewPassword() const { return newPasswordEdit_->text(); }

void ForgotPasswordDialog::onCancelClicked()
{
    reject();
}