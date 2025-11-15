#include "AccountsPage.h"
#include "core/AccountManager.h"
#include "entities/Account.h"
#include "entities/AccountAdmin.h"
#include "ChangePasswordDialog.h" // thêm dialog đổi mật khẩu
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
    topLayout->setContentsMargins(24, 20, 24, 16);    // giống 2 page kia, sát trái 24
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
    grid->setVerticalSpacing(20);               // giảm spacing dọc
    // khởi tạo lineedit
    idEdit_        = new QLineEdit(this); idEdit_->setPlaceholderText("ID tài khoản");
    fullnameEdit_  = new QLineEdit(this); fullnameEdit_->setPlaceholderText("Họ và tên");
    userEdit_      = new QLineEdit(this); userEdit_->setPlaceholderText("Tên tài khoản");
    phoneEdit_     = new QLineEdit(this); phoneEdit_->setPlaceholderText("Số điện thoại");
    roleEdit_      = new QLineEdit(this); roleEdit_->setPlaceholderText("Chức vụ");
    emailEdit_     = new QLineEdit(this); emailEdit_->setPlaceholderText("Email");
    // hàng 1
    grid->addWidget(new QLabel("ID tài khoản"), 0, 0);
    grid->addWidget(new QLabel("Họ và tên"),0,1);
    grid->addWidget(idEdit_,1,0);
    grid->addWidget(fullnameEdit_,1,1);
    // hàng 2
    grid->addWidget(new QLabel("Tên tài khoản"), 2, 0);
    grid->addWidget(new QLabel("Số điện thoại"), 2, 1);
    grid->addWidget(userEdit_,3, 0);
    grid->addWidget(phoneEdit_,3, 1);
    // hàng 3
    grid->addWidget(new QLabel("Chức vụ"), 4, 0);
    grid->addWidget(new QLabel("Email"),4, 1);
    grid->addWidget(roleEdit_,5, 0);
    grid->addWidget(emailEdit_,5, 1);
    mainLayout->addWidget(form);
    // ====== nút ======
    QWidget *btnBar = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnBar);
    btnLayout->setContentsMargins(24, 18, 24, 20);
    btnLayout->setSpacing(20);
    btnLayout->addStretch();
    btnUpdate_ = new QPushButton(tr(u8"Sửa thông tin"), this);
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
    // Những ô không được sửa: id, username, role
    idEdit_->setReadOnly(true);
    userEdit_->setReadOnly(true);
    roleEdit_->setReadOnly(true);
    // Các ô có thể chỉnh (mặc định readOnly=true) -> sẽ bật khi bấm Cập nhật
    fullnameEdit_->setReadOnly(true);
    phoneEdit_->setReadOnly(true);
    emailEdit_->setReadOnly(true);
    // Ngoài readOnly, đảm bảo widgets enabled để tránh trường hợp parent disabled
    idEdit_->setEnabled(true);
    userEdit_->setEnabled(true);
    roleEdit_->setEnabled(true);
    fullnameEdit_->setEnabled(true);
    phoneEdit_->setEnabled(true);
    emailEdit_->setEnabled(true);

    // // Tải dữ liệu người dùng
    // loadAccountData();

    // Nút Cập nhật: toggle chế độ chỉnh sửa -> lưu khi bấm lần nữa
    connect(btnUpdate_, &QPushButton::clicked, this, [this]() {
    bool currentlyReadOnly = fullnameEdit_->isReadOnly();
    if (currentlyReadOnly) {
        // Bật chế độ sửa
        fullnameEdit_->setReadOnly(false);
        phoneEdit_->setReadOnly(false);
        emailEdit_->setReadOnly(false);
        // đảm bảo có thể nhập (enabled)
        fullnameEdit_->setEnabled(true);
        phoneEdit_->setEnabled(true);
        emailEdit_->setEnabled(true);
        // đặt focus vào ô họ tên để người dùng nhập ngay
        fullnameEdit_->setFocus();
        btnUpdate_->setText("Lưu thay đổi");
    } else {
        // Lưu thay đổi: gọi AccountManager
        const std::string id = idEdit_->text().toStdString();
        const std::string newFull = fullnameEdit_->text().toStdString();
        const std::string newPhone = phoneEdit_->text().toStdString();
        const std::string newEmail = emailEdit_->text().toStdString();
        // Gọi hàm update (trong repo có updateAgentProfile)
        accountManager_->updateProfile(id, newFull, newPhone, newEmail);
        // Sau khi lưu, khóa lại các ô
        fullnameEdit_->setReadOnly(true);
        phoneEdit_->setReadOnly(true);
        emailEdit_->setReadOnly(true);
        btnUpdate_->setText("Sửa thông tin");
        // Reload data (đảm bảo hiển thị đúng dữ liệu đã lưu/ghi file)
        loadAccountData();
        QMessageBox::information(this, "Thành công", "Đã cập nhật thông tin.");
    }
    });
    // Nút "Đổi mật khẩu": mở dialog đổi mật khẩu (sử dụng con trỏ accountManager_ hiện có)
    connect(btnPwd, &QPushButton::clicked, this, [this]() {
    Account* user = accountManager_->getCurrentUser();
    if (!user) {
        QMessageBox::warning(this, "Lỗi", "Không có người dùng đang đăng nhập.");
        return;
    }
    QString userId = QString::fromStdString(user->getId());
    ChangePasswordDialog dlg(accountManager_, userId, this);
    dlg.exec();
    });
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

void AccountsPage::showEvent(QShowEvent *event)
{
    // 1. Luôn tải dữ liệu mới nhất khi trang được hiển thị
    loadAccountData();

    // 2. Đảm bảo trang luôn ở chế độ "chỉ xem" (read-only) khi mới vào
    // (Phòng trường hợp người dùng đang sửa, rồi chuyển sang tab khác và quay lại)
    fullnameEdit_->setReadOnly(true);
    phoneEdit_->setReadOnly(true);
    emailEdit_->setReadOnly(true);

    // 3. Đặt lại tên nút về trạng thái ban đầu ("Sửa thông tin")
    btnUpdate_->setText(tr(u8"Sửa thông tin"));

    // 4. Gọi hàm của lớp cha
    QWidget::showEvent(event);
}