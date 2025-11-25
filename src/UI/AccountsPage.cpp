#include "AccountsPage.h"
#include "core/AccountManager.h"
#include "entities/Account.h"
#include "entities/AccountAdmin.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>
#include <QGroupBox>

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
    // Style chung cho trang
    this->setStyleSheet(
        "QWidget#MainBackground { background: #F2F6FD; }"
        
        // Container trắng (Card)
        "QFrame#SectionContainer { "
        "    background: white; "
        "    border: 1px solid #E0E0E0; "
        "    border-radius: 4px; "
        "}"
        
        // Tiêu đề Section (VD: Thông tin tài khoản)
        "QLabel#SectionTitle { "
        "    color: #5d6d7e; " 
        "    font-weight: 600; "
        "    font-size: 16px; "
        "    padding-bottom: 8px; "
        "    border-bottom: 1px solid #EEE; "
        "}"

        // Label trường nhập liệu (VD: Họ tên)
        "QLabel#FieldLabel { "
        "    color: #133e87; "
        "    font-weight: 700; "
        "    font-size: 13px; "
        "}"
        "QLabel#FieldLabelRed { " // Dấu * đỏ
        "    color: red; "
        "    font-weight: 700; "
        "    font-size: 13px; "
        "}"
        
        // Input fields
        "QLineEdit { "
        "    background: white; "
        "    border: 1px solid #D5DBE1; "
        "    border-radius: 3px; "
        "    height: 32px; "
        "    padding-left: 8px; "
        "    color: #333; "
        "}"
        "QLineEdit:read-only { "
        "    background: #F5F7FA; " // Màu xám nhẹ khi readonly
        "    color: #555; "
        "}"
        "QLineEdit:focus { border: 1px solid #4478BD; }"

        // Buttons
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

    // Layout chính với margin trái phải 24px
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20); 
    mainLayout->setSpacing(20);

    // Đặt ID để hứng CSS background
    this->setObjectName("MainBackground");

    // =========================================================
    // SECTION 1: THÔNG TIN TÀI KHOẢN
    // =========================================================
    QFrame* infoContainer = new QFrame(this);
    infoContainer->setObjectName("SectionContainer");
    
    // Layout bên trong Card 1
    QVBoxLayout* infoLayout = new QVBoxLayout(infoContainer);
    infoLayout->setContentsMargins(20, 15, 20, 20);
    infoLayout->setSpacing(15);

    // 1.1 Header
    QLabel* infoTitle = new QLabel("Thông tin tài khoản");
    infoTitle->setObjectName("SectionTitle");
    infoLayout->addWidget(infoTitle);

    // 1.2 Grid Fields (2 dòng, 3 cột)
    QGridLayout* infoGrid = new QGridLayout();
    infoGrid->setHorizontalSpacing(20);
    infoGrid->setVerticalSpacing(15);

    // Khởi tạo controls
    idEdit_ = new QLineEdit(); 
    fullnameEdit_ = new QLineEdit();
    userEdit_ = new QLineEdit();
    phoneEdit_ = new QLineEdit();
    roleEdit_ = new QLineEdit();
    emailEdit_ = new QLineEdit();

    // Setup ReadOnly ban đầu
    idEdit_->setReadOnly(true);       // Không bao giờ sửa
    userEdit_->setReadOnly(true);     // Không bao giờ sửa
    roleEdit_->setReadOnly(true);     // Không bao giờ sửa
    
    fullnameEdit_->setReadOnly(true); // Toggle được
    phoneEdit_->setReadOnly(true);    // Toggle được
    emailEdit_->setReadOnly(true);    // Toggle được

    // Hàng 1
    infoGrid->addWidget(createFieldWidget("ID tài khoản", idEdit_), 0, 0);
    infoGrid->addWidget(createFieldWidget("Họ và tên", fullnameEdit_, true), 0, 1); // Có dấu *
    infoGrid->addWidget(createFieldWidget("Tên tài khoản", userEdit_, true), 0, 2);

    // Hàng 2
    infoGrid->addWidget(createFieldWidget("Chức vụ", roleEdit_), 1, 0);
    infoGrid->addWidget(createFieldWidget("Số điện thoại", phoneEdit_), 1, 1);
    infoGrid->addWidget(createFieldWidget("Email", emailEdit_), 1, 2);

    infoLayout->addLayout(infoGrid);

    // 1.3 Button Sửa thông tin
    QHBoxLayout* infoBtnLayout = new QHBoxLayout();
    btnUpdateInfo_ = new QPushButton("Sửa thông tin");
    btnUpdateInfo_->setMinimumWidth(130);
    infoBtnLayout->addWidget(btnUpdateInfo_);
    infoBtnLayout->addStretch(); // Đẩy nút sang trái

    infoLayout->addLayout(infoBtnLayout);
    mainLayout->addWidget(infoContainer);

    // =========================================================
    // SECTION 2: ĐỔI MẬT KHẨU
    // =========================================================
    QFrame* pwdContainer = new QFrame(this);
    pwdContainer->setObjectName("SectionContainer");
    
    // Layout bên trong Card 2
    QVBoxLayout* pwdLayout = new QVBoxLayout(pwdContainer);
    pwdLayout->setContentsMargins(20, 15, 20, 20);
    pwdLayout->setSpacing(15);

    // 2.1 Header
    QLabel* pwdTitle = new QLabel("Đổi mật khẩu");
    pwdTitle->setObjectName("SectionTitle");
    pwdLayout->addWidget(pwdTitle);

    // 2.2 Fields (3 cột ngang)
    QHBoxLayout* pwdFieldsLayout = new QHBoxLayout();
    pwdFieldsLayout->setSpacing(20);

    currentPassEdit_ = new QLineEdit();
    currentPassEdit_->setEchoMode(QLineEdit::Password);
    
    newPassEdit_ = new QLineEdit();
    newPassEdit_->setEchoMode(QLineEdit::Password);
    
    confirmPassEdit_ = new QLineEdit();
    confirmPassEdit_->setEchoMode(QLineEdit::Password);

    // Thêm các widget vào layout ngang, tỷ lệ 1:1:1
    pwdFieldsLayout->addWidget(createFieldWidget("Mật khẩu hiện tại", currentPassEdit_, true), 1);
    pwdFieldsLayout->addWidget(createFieldWidget("Mật khẩu mới", newPassEdit_, true), 1);
    pwdFieldsLayout->addWidget(createFieldWidget("Xác nhận mật khẩu", confirmPassEdit_, true), 1);

    pwdLayout->addLayout(pwdFieldsLayout);

    // 2.3 Button Đổi mật khẩu
    QHBoxLayout* pwdBtnLayout = new QHBoxLayout();
    btnChangePass_ = new QPushButton("Đổi mật khẩu");
    btnChangePass_->setMinimumWidth(130);
    pwdBtnLayout->addWidget(btnChangePass_);
    pwdBtnLayout->addStretch();

    pwdLayout->addLayout(pwdBtnLayout);
    mainLayout->addWidget(pwdContainer);

    // Spacer dưới cùng để đẩy nội dung lên trên
    mainLayout->addStretch();
}

// Hàm Helper để tạo cụm: Label + Input
QWidget* AccountsPage::createFieldWidget(const QString& labelText, QWidget* inputWidget, bool isMandatory)
{
    QWidget* wrapper = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(wrapper);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6); // Khoảng cách giữa Label và Input

    // Tạo Label. Nếu bắt buộc thì thêm dấu * đỏ
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
    
    // Reset trạng thái Sửa thông tin về ReadOnly
    fullnameEdit_->setReadOnly(true);
    phoneEdit_->setReadOnly(true);
    emailEdit_->setReadOnly(true);
    btnUpdateInfo_->setText("Sửa thông tin");

    // Reset trạng thái form Đổi mật khẩu
    currentPassEdit_->clear();
    newPassEdit_->clear();
    confirmPassEdit_->clear();

    QWidget::showEvent(event);
}

// --- LOGIC XỬ LÝ SỬA THÔNG TIN (ĐÃ SỬA LỖI) ---
void AccountsPage::onUpdateInfoClicked()
{
    bool isReadOnly = fullnameEdit_->isReadOnly();

    if (isReadOnly) {
        // Bật chế độ sửa
        fullnameEdit_->setReadOnly(false);
        phoneEdit_->setReadOnly(false);
        emailEdit_->setReadOnly(false);
        
        fullnameEdit_->setFocus(); // Focus vào ô tên để nhập ngay
        btnUpdateInfo_->setText("Lưu thông tin");
    } else {
        // Thực hiện lưu
        std::string id = idEdit_->text().toStdString();
        std::string newFull = fullnameEdit_->text().trimmed().toStdString();
        std::string newPhone = phoneEdit_->text().trimmed().toStdString();
        std::string newEmail = emailEdit_->text().trimmed().toStdString();

        if (newFull.empty()) {
            QMessageBox::warning(this, "Cảnh báo", "Họ tên không được để trống.");
            fullnameEdit_->setFocus();
            return;
        }

        // --- SỬA LỖI TẠI ĐÂY: Gọi trực tiếp không qua IF ---
        // Vì updateProfile trả về void, ta chỉ gọi nó để thực thi.
        accountManager_->updateProfile(id, newFull, newPhone, newEmail);
        
        // Khóa lại các ô
        fullnameEdit_->setReadOnly(true);
        phoneEdit_->setReadOnly(true);
        emailEdit_->setReadOnly(true);
        btnUpdateInfo_->setText("Sửa thông tin");
        
        loadAccountData(); // Refresh hiển thị
    }
}

// --- LOGIC XỬ LÝ ĐỔI MẬT KHẨU ---
void AccountsPage::onChangePasswordClicked()
{
    QString oldp = currentPassEdit_->text();
    QString newp = newPassEdit_->text();
    QString confirm = confirmPassEdit_->text();

    // 1. Validate Input
    if (oldp.isEmpty() || newp.isEmpty() || confirm.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập đầy đủ thông tin mật khẩu.");
        return;
    }

    if (newp != confirm) {
        QMessageBox::warning(this, "Lỗi", "Mật khẩu mới và xác nhận mật khẩu không khớp.");
        return;
    }
    
    // 2. Kiểm tra User hiện tại
    Account* current = accountManager_->getCurrentUser();
    if (!current) {
        QMessageBox::warning(this, "Lỗi", "Không có người dùng đang đăng nhập.");
        return;
    }

    // 3. Xác thực mật khẩu cũ
    if (!current->authenticate(oldp.toStdString())) {
        QMessageBox::warning(this, "Lỗi", "Mật khẩu hiện tại không đúng.");
        currentPassEdit_->setFocus();
        currentPassEdit_->selectAll();
        return;
    }

    // 4. Gọi API đổi mật khẩu
    bool ok = accountManager_->changePassword(
        current->getId(), 
        oldp.toStdString(), 
        newp.toStdString()
    );

    if (ok) {
        QMessageBox::information(this, "Thành công", "Đổi mật khẩu thành công!");
        // Clear fields
        currentPassEdit_->clear();
        newPassEdit_->clear();
        confirmPassEdit_->clear();
    } else {
        QMessageBox::critical(this, "Lỗi", "Đổi mật khẩu thất bại. Vui lòng thử lại.");
    }
}