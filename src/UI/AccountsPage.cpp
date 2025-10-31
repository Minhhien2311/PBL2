#include "AccountsPage.h"

// <--- Sửa lỗi: Include các file cần thiết
#include "core/AccountManager.h"
#include "entities/Account.h"
#include "entities/AccountAdmin.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

AccountsPage::AccountsPage(AccountManager* accManager, QWidget* parent)
    : QWidget(parent),
      accountManager_(accManager)
{
    Q_ASSERT(accountManager_ != nullptr);

    // style giống route/flight
    this->setStyleSheet(
        "QWidget { background:#F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:18px; }"
        "QLineEdit { background:white; border:1px solid #608bc1; border-radius:4px; height:28px; padding-left:8px; }"
        "QPushButton.MainBtn { background:#5886C0; color:white; border:none; border-radius:10px; height:34px; font-weight:600; }"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // ====== top: tiêu đề ======
    QWidget *top = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(top);
    topLayout->setContentsMargins(24, 20, 24, 16);   // giống 2 page kia, sát trái 24
    topLayout->setSpacing(16);

    QLabel *title = new QLabel("Thông tin tài khoản", this);
    title->setProperty("class", "PageTitle");
    topLayout->addWidget(title);

    mainLayout->addWidget(top);

    // ====== form 2 cột ======
    QWidget *form = new QWidget(this);
    QGridLayout *grid = new QGridLayout(form);
    grid->setContentsMargins(24, 10, 24, 0);
    grid->setHorizontalSpacing(50);
    grid->setVerticalSpacing(20);           // 👈 giảm spacing dọc
    // mỗi hàng cách vừa phải, không bị "hở" như trước

    // khởi tạo lineedit
    idEdit_       = new QLineEdit(this); idEdit_->setPlaceholderText("ID tài khoản");
    fullnameEdit_ = new QLineEdit(this); fullnameEdit_->setPlaceholderText("Họ và tên");
    userEdit_     = new QLineEdit(this); userEdit_->setPlaceholderText("Tên tài khoản");
    phoneEdit_    = new QLineEdit(this); phoneEdit_->setPlaceholderText("Số điện thoại");
    roleEdit_     = new QLineEdit(this); roleEdit_->setPlaceholderText("Chức vụ");
    emailEdit_    = new QLineEdit(this); emailEdit_->setPlaceholderText("Email");

    // hàng 1
    grid->addWidget(new QLabel("ID tài khoản"), 0, 0);
    grid->addWidget(new QLabel("Họ và tên"),    0, 1);
    grid->addWidget(idEdit_,                    1, 0);
    grid->addWidget(fullnameEdit_,              1, 1);

    // hàng 2
    grid->addWidget(new QLabel("Tên tài khoản"), 2, 0);
    grid->addWidget(new QLabel("Số điện thoại"), 2, 1);
    grid->addWidget(userEdit_,                   3, 0);
    grid->addWidget(phoneEdit_,                  3, 1);

    // hàng 3
    grid->addWidget(new QLabel("Chức vụ"), 4, 0);
    grid->addWidget(new QLabel("Email"),   4, 1);
    grid->addWidget(roleEdit_,             5, 0);
    grid->addWidget(emailEdit_,            5, 1);

    mainLayout->addWidget(form);

    // ====== nút ======
    QWidget *btnBar = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnBar);
    btnLayout->setContentsMargins(24, 18, 24, 20);
    btnLayout->setSpacing(20);
    btnLayout->addStretch();

    btnUpdate_ = new QPushButton(tr(u8"Cập nhật thông tin"), this);
    QPushButton *btnPwd = new QPushButton(tr(u8"Đổi mật khẩu"), this);
    btnUpdate_->setProperty("class", "MainBtn");
    btnPwd->setProperty("class", "MainBtn");
    btnUpdate_->setMinimumWidth(210);
    btnPwd->setMinimumWidth(210);

    btnLayout->addWidget(btnUpdate_);
    btnLayout->addWidget(btnPwd);
    btnLayout->addStretch();

    mainLayout->addWidget(btnBar);

    // ====== trạng thái mặc định ======
    idEdit_->setReadOnly(true);
    userEdit_->setReadOnly(true);
    roleEdit_->setReadOnly(true);
    fullnameEdit_->setReadOnly(true);
    phoneEdit_->setReadOnly(true);
    emailEdit_->setReadOnly(true);
    
    // Tải dữ liệu người dùng
    loadAccountData();
    
    connect(btnUpdate_,&QPushButton::clicked,this,[=]{
        bool ro = fullnameEdit_->isReadOnly(); // Lấy trạng thái
        
        // Đảo trạng thái ReadOnly
        fullnameEdit_->setReadOnly(!ro);
        phoneEdit_->setReadOnly(!ro);
        emailEdit_->setReadOnly(!ro);
        
        if (ro) { // Nếu vừa chuyển sang chế độ SỬA
            btnUpdate_->setText("Lưu thay đổi");
        } else { // Nếu vừa bấm LƯU
            // Lưu thông tin đã cập nhật
            accountManager_->updateAgentProfile(
                idEdit_->text().toStdString(),
                fullnameEdit_->text().toStdString(),
                phoneEdit_->text().toStdString(),
                emailEdit_->text().toStdString()
            );
            
            btnUpdate_->setText("Cập nhật thông tin");
            QMessageBox::information(this, "Thành công", "Đã cập nhật thông tin.");
        }
    });
    
    // Nút "Đổi mật khẩu" không có chức năng theo yêu cầu
    // (Chỗ nối API cho btnPwd - để trống)
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
        
        // Kiểm tra vai trò
        if (dynamic_cast<AccountAdmin*>(user)) {
            roleEdit_->setText("Quản trị viên");
        } else {
            roleEdit_->setText("Đại lý");
        }
    }
}

