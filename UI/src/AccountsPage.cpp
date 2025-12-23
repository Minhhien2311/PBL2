#include "AccountsPage.h"

#include "core/AccountManager.h"
#include "entities/Account.h"
#include "entities/AccountAdmin.h"
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

AccountsPage::AccountsPage(AccountManager* accManager, QWidget* parent)
    : QWidget(parent),
      accountManager_(accManager)
{
    Q_ASSERT(accountManager_ != nullptr);
    setupUi();
    setupConnections();
}

void AccountsPage::setupUi()
{
    this->setStyleSheet(
        "QWidget#MainBackground { background: #F2F6FD; }"
        "QFrame#SectionContainer { "
        "    background: white; "
        "    border: 1px solid #E0E0E0; "
        "    border-radius: 4px; "
        "}"
        "QLabel#SectionTitle { "
        "    color: #133e87; " 
        "    font-weight: 600; "
        "    font-size: 16px; "
        "    padding-bottom: 8px; "
        "    border-bottom: 1px solid #EEE; "
        "}"
        "QLabel#FieldLabel { "
        "    color: #5d6d7e; "
        "    font-weight: 700; "
        "    font-size: 13px; "
        "}"
        "QLabel#FieldLabelRed { "
        "    color: red; "
        "    font-weight: 700; "
        "    font-size: 13px; "
        "}"
        "QLineEdit { "
        "    background: white; "
        "    border: 1px solid #D5DBE1; "
        "    border-radius: 3px; "
        "    height: 32px; "
        "    padding-left: 8px; "
        "    color: #333; "
        "}"
        "QLineEdit:read-only { "
        "    background: #F5F7FA; "
        "    color: #555; "
        "}"
        "QLineEdit:focus { border: 1px solid #4478BD; }"
        "QPushButton { "
        "    background: #4478BD; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 4px; "
        "    height: 36px; "
        "    font-weight: 600; "
        "    padding: 0 16px; "
        "}"
        "QPushButton:hover { background: #365a9e; }"
        "QPushButton:disabled { background: #BDC3C7; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 40, 24, 20);
    mainLayout->setSpacing(20);

    this->setObjectName("MainBackground");

    // ========== ACCOUNT INFORMATION SECTION ==========
    QFrame* infoContainer = new QFrame(this);
    infoContainer->setObjectName("SectionContainer");

    QVBoxLayout* infoLayout = new QVBoxLayout(infoContainer);
    infoLayout->setContentsMargins(20, 15, 20, 20);
    infoLayout->setSpacing(15);

    QLabel* infoTitle = new QLabel("Thông tin tài khoản");
    infoTitle->setObjectName("SectionTitle");
    infoLayout->addWidget(infoTitle);

    QGridLayout* infoGrid = new QGridLayout();
    infoGrid->setHorizontalSpacing(20);
    infoGrid->setVerticalSpacing(15);

    idEdit_ = new QLineEdit(); 
    fullnameEdit_ = new QLineEdit();
    userEdit_ = new QLineEdit();
    phoneEdit_ = new QLineEdit();
    roleEdit_ = new QLineEdit();
    emailEdit_ = new QLineEdit();

    idEdit_->setReadOnly(true);
    userEdit_->setReadOnly(true);
    roleEdit_->setReadOnly(true);

    fullnameEdit_->setReadOnly(true);
    phoneEdit_->setReadOnly(true);
    emailEdit_->setReadOnly(true);

    infoGrid->addWidget(createFieldWidget("ID tài khoản", idEdit_), 0, 0);
    infoGrid->addWidget(createFieldWidget("Họ và tên", fullnameEdit_, true), 0, 1);
    infoGrid->addWidget(createFieldWidget("Tên tài khoản", userEdit_, true), 0, 2);
    infoGrid->addWidget(createFieldWidget("Chức vụ", roleEdit_), 1, 0);
    infoGrid->addWidget(createFieldWidget("Số điện thoại", phoneEdit_), 1, 1);
    infoGrid->addWidget(createFieldWidget("Email", emailEdit_), 1, 2);

    infoLayout->addLayout(infoGrid);

    QHBoxLayout* infoBtnLayout = new QHBoxLayout();
    btnUpdateInfo_ = new QPushButton("Sửa thông tin");
    btnUpdateInfo_->setMinimumWidth(130);
    infoBtnLayout->addWidget(btnUpdateInfo_);
    infoBtnLayout->addStretch();

    infoLayout->addLayout(infoBtnLayout);
    mainLayout->addWidget(infoContainer);

    // ========== PASSWORD CHANGE SECTION ==========
    QFrame* pwdContainer = new QFrame(this);
    pwdContainer->setObjectName("SectionContainer");

    QVBoxLayout* pwdLayout = new QVBoxLayout(pwdContainer);
    pwdLayout->setContentsMargins(20, 15, 20, 20);
    pwdLayout->setSpacing(15);

    QLabel* pwdTitle = new QLabel("Đổi mật khẩu");
    pwdTitle->setObjectName("SectionTitle");
    pwdLayout->addWidget(pwdTitle);

    QHBoxLayout* pwdFieldsLayout = new QHBoxLayout();
    pwdFieldsLayout->setSpacing(20);

    currentPassEdit_ = new QLineEdit();
    currentPassEdit_->setEchoMode(QLineEdit::Password);

    newPassEdit_ = new QLineEdit();
    newPassEdit_->setEchoMode(QLineEdit::Password);

    confirmPassEdit_ = new QLineEdit();
    confirmPassEdit_->setEchoMode(QLineEdit::Password);

    pwdFieldsLayout->addWidget(createFieldWidget("Mật khẩu hiện tại", currentPassEdit_, true), 1);
    pwdFieldsLayout->addWidget(createFieldWidget("Mật khẩu mới", newPassEdit_, true), 1);
    pwdFieldsLayout->addWidget(createFieldWidget("Xác nhận mật khẩu", confirmPassEdit_, true), 1);

    pwdLayout->addLayout(pwdFieldsLayout);

    QHBoxLayout* pwdBtnLayout = new QHBoxLayout();
    btnChangePass_ = new QPushButton("Đổi mật khẩu");
    btnChangePass_->setMinimumWidth(130);
    pwdBtnLayout->addWidget(btnChangePass_);
    pwdBtnLayout->addStretch();

    pwdLayout->addLayout(pwdBtnLayout);
    mainLayout->addWidget(pwdContainer);

    mainLayout->addStretch();
}

QWidget* AccountsPage::createFieldWidget(const QString& labelText, QWidget* inputWidget, bool isMandatory)
{
    QWidget* wrapper = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(wrapper);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    QLabel* label = new QLabel(labelText);
    if (isMandatory) {
        label->setText(labelText + " <font color='red'>*</font>");
        label->setTextFormat(Qt::RichText);
    }
    label->setObjectName("FieldLabel");

    layout->addWidget(label);
    layout->addWidget(inputWidget);
    
    return wrapper;
}

void AccountsPage::setupConnections()
{
    connect(btnUpdateInfo_, &QPushButton::clicked, this, &AccountsPage::onUpdateInfoClicked);
    connect(btnChangePass_, &QPushButton::clicked, this, &AccountsPage::onChangePasswordClicked);
}

void AccountsPage::loadAccountData()
{
    Account* user = accountManager_->getCurrentUser();
    if (user) {
        idEdit_->setText(QString::fromStdString(user->getId()));
        userEdit_->setText(QString::fromStdString(user->getUsername()));
        fullnameEdit_->setText(QString::fromStdString(user->getFullName()));
        phoneEdit_->setText(QString::fromStdString(user->getPhone()));
        emailEdit_->setText(QString::fromStdString(user->getEmail()));

        if (dynamic_cast<AccountAdmin*>(user)) {
            roleEdit_->setText("Quản trị viên");
        } else {
            roleEdit_->setText("Đại lý");
        }
    }
}

void AccountsPage::showEvent(QShowEvent *event)
{
    loadAccountData();
    fullnameEdit_->setReadOnly(true);
    phoneEdit_->setReadOnly(true);
    emailEdit_->setReadOnly(true);
    btnUpdateInfo_->setText("Sửa thông tin");

    currentPassEdit_->clear();
    newPassEdit_->clear();
    confirmPassEdit_->clear();

    QWidget::showEvent(event);
}

// ========== UPDATE INFO LOGIC ==========
void AccountsPage::onUpdateInfoClicked()
{
    bool isReadOnly = fullnameEdit_->isReadOnly();

    if (isReadOnly) {
        fullnameEdit_->setReadOnly(false);
        phoneEdit_->setReadOnly(false);
        emailEdit_->setReadOnly(false);

        fullnameEdit_->setFocus();
        btnUpdateInfo_->setText("Lưu thông tin");
    } else {
        std::string id = idEdit_->text().toStdString();
        std::string newFull = fullnameEdit_->text().trimmed().toStdString();
        std::string newPhone = phoneEdit_->text().trimmed().toStdString();
        std::string newEmail = emailEdit_->text().trimmed().toStdString();

        if (newFull.empty()) {
            QMessageBox::warning(this, "Cảnh báo", "Họ tên không được để trống.");
            fullnameEdit_->setFocus();
            return;
        }
        accountManager_->updateProfile(id, newFull, newPhone, newEmail);

        fullnameEdit_->setReadOnly(true);
        phoneEdit_->setReadOnly(true);
        emailEdit_->setReadOnly(true);
        btnUpdateInfo_->setText("Sửa thông tin");

        loadAccountData();
    }
}

// ========== CHANGE PASSWORD LOGIC ==========
void AccountsPage::onChangePasswordClicked()
{
    QString oldp = currentPassEdit_->text();
    QString newp = newPassEdit_->text();
    QString confirm = confirmPassEdit_->text();

    if (oldp.isEmpty() || newp.isEmpty() || confirm.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập đầy đủ thông tin mật khẩu.");
        return;
    }

    if (newp != confirm) {
        QMessageBox::warning(this, "Lỗi", "Mật khẩu mới và xác nhận mật khẩu không khớp.");
        return;
    }

    Account* current = accountManager_->getCurrentUser();
    if (!current) {
        QMessageBox::warning(this, "Lỗi", "Không có người dùng đang đăng nhập.");
        return;
    }

    if (!current->authenticate(oldp.toStdString())) {
        QMessageBox::warning(this, "Lỗi", "Mật khẩu hiện tại không đúng.");
        currentPassEdit_->setFocus();
        currentPassEdit_->selectAll();
        return;
    }

    bool ok = accountManager_->changePassword(
        current->getId(), 
        oldp.toStdString(), 
        newp.toStdString()
    );

    if (ok) {
        QMessageBox::information(this, "Thành công", "Đổi mật khẩu thành công!");
        currentPassEdit_->clear();
        newPassEdit_->clear();
        confirmPassEdit_->clear();
    } else {
        QMessageBox::critical(this, "Lỗi", "Đổi mật khẩu thất bại. Vui lòng thử lại.");
    }
}