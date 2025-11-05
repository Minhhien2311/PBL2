#include "LoginPage.h"

#include "core/AccountManager.h"
#include "entities/AccountAdmin.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QApplication>
#include <QSpacerItem>

LoginPage::LoginPage(AccountManager* accManager, QWidget *parent)
    : QWidget(parent),
      accountManager_(accManager)
{
    Q_ASSERT(accountManager_ != nullptr);
    setupUi();
    setupConnections();
}

void LoginPage::setupUi()
{
    // ---------- NỀN MÀU XANH RẤT NHẠT ----------
    this->setAutoFillBackground(true);
    this->setStyleSheet(
        "QWidget { background: #ffffffff; }"
        "#AppTitle { font-size: 26px; font-weight: 700; color: #133e87; background: transparent; }"
        "#LoginBox { background: white; border: 1px solid #133e87; border-radius: 12px; }"
        "#LoginHeader { background: transparent; font-size: 40px; font-weight: 600; color: #133e87; }"
        "#LoginBoxTitle { font-size: 14px; font-weight: 600; color: #133e87; }"
        "QLineEdit { height: 30px; background: #f5f8fb; border: 1px solid #133e87; border-radius: 3px; padding-left: 10px; }"
        "QPushButton#LoginButton { background: #608bc1; color: white; height: 40px; border-radius: 3px; font-weight: 600; font-size: 12px; }"
        "QPushButton#LoginButton:hover { background: #476c9a; }"
        "QLabel#ForgotLabel { color: #133e87; }"
        "QCheckBox { color: #133e87; }"
        "QCheckBox::indicator { width: 12px; height: 12px; border-radius: 2px; }"
        "QCheckBox::indicator:unchecked { border: 0.5px solid #133e87; background: white; }"
        "QCheckBox::indicator:checked { border: 0.5px solid #133e87; background: #608bc1; }"
    );

    // ---------- LAYOUT GỐC: căn giữa ngang ----------
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 20, 0, 20);
    rootLayout->setAlignment(Qt::AlignCenter);

    // ---------- CỘT CHÍNH ----------
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Tiêu đề
    QLabel *appTitle = new QLabel("PHẦN MỀM QUẢN LÝ BÁN VÉ MÁY BAY", this);
    appTitle->setObjectName("AppTitle");
    appTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(appTitle);
    mainLayout->addSpacing(60);

    // ---------- HỘP ĐĂNG NHẬP ----------
    // Hộp ngoài: bo góc, có viền xanh
    QFrame *loginBox = new QFrame(this);
    loginBox->setObjectName("LoginBox");
    loginBox->setFixedSize(460, 360);                 // to hơn để giống ảnh 1
    loginBox->setFrameShape(QFrame::StyledPanel);

    // layout của hộp
    QVBoxLayout *boxLayout = new QVBoxLayout(loginBox);
    boxLayout->setContentsMargins(0, 0, 0, 12);       // để header dính mép trên
    boxLayout->setSpacing(0);

    // ---------- HEADER TRẮNG BÊN TRONG ----------
    // ở ảnh 1: phần "Đăng nhập" nằm trong 1 thanh trên cùng có đường kẻ dưới
    QWidget *header = new QWidget(loginBox);
    header->setObjectName("LoginHeader");
    header->setFixedHeight(58);

    QVBoxLayout *headerLayout = new QVBoxLayout(header);
    // left top right bottom    
    headerLayout->setContentsMargins(0, 10, 0, 10);  // <-- 10px trên dưới
    headerLayout->setSpacing(0);

    QLabel *title = new QLabel("Đăng nhập", header);
    title->setObjectName("LoginBoxTitle");
    title->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(title);


    // đường kẻ ngang
    QFrame *line = new QFrame(loginBox);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setStyleSheet("color: #133e87;");

    boxLayout->addWidget(header);
    boxLayout->addWidget(line);

    // ---------- PHẦN NỘI DUNG FORM ----------
    QWidget *content = new QWidget(loginBox);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(36, 22, 36, 0);
    contentLayout->setSpacing(14);

    // Username
    usernameEdit_ = new QLineEdit(content);
    usernameEdit_->setPlaceholderText("Username");
    usernameEdit_->setFixedWidth(350);
    // contentLayout->addWidget(usernameEdit_);
    {
        QHBoxLayout *row = new QHBoxLayout();
        row->addStretch();
        row->addWidget(usernameEdit_);
        row->addStretch();
        contentLayout->addLayout(row);
    }

    // Password
    passwordEdit_ = new QLineEdit(content);
    passwordEdit_->setPlaceholderText("Password");
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setFixedWidth(350);          // <--- thêm
    {
        QHBoxLayout *row = new QHBoxLayout();
        row->addStretch();
        row->addWidget(passwordEdit_);
        row->addStretch();
        contentLayout->addLayout(row);
    }

    // Checkbox hiển thị mật khẩu — căn thẳng với ô nhập
    QHBoxLayout *checkRow = new QHBoxLayout();
    checkRow->setContentsMargins(0, 0, 0, 0);

    // giống input: stretch nhỏ bên trái
    checkRow->addSpacing(20);        // thử 58–62 để thẳng hẳn

    showPasswordCheck_ = new QCheckBox("Hiển thị mật khẩu", content);
    checkRow->addWidget(showPasswordCheck_);

    checkRow->addStretch();
    contentLayout->addLayout(checkRow);


    // Nút đăng nhập
    loginButton_ = new QPushButton("Đăng nhập", content);
    loginButton_->setObjectName("LoginButton");
    loginButton_->setFixedWidth(180);           // <--- giống ảnh
    loginButton_->setFixedHeight(32);
    {
        QHBoxLayout *row = new QHBoxLayout();
        row->setContentsMargins(0, 20, 0, 0); // thêm 20px phía trên nút
        row->addStretch();
        row->addWidget(loginButton_);
        row->addStretch();
        contentLayout->addLayout(row);
    }

    // "Quên mật khẩu?"
    QLabel *forgotPasswordLabel = new QLabel("Quên mật khẩu?", content);
    forgotPasswordLabel->setObjectName("ForgotLabel");
    forgotPasswordLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(forgotPasswordLabel);

    contentLayout->addStretch();
    boxLayout->addWidget(content);

    // căn giữa hộp trong main
    mainLayout->addWidget(loginBox, 0, Qt::AlignHCenter);
    mainLayout->addStretch();

    rootLayout->addStretch();
    rootLayout->addLayout(mainLayout);
    rootLayout->addStretch();
}

void LoginPage::setupConnections()
{
    connect(loginButton_, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(showPasswordCheck_, &QCheckBox::toggled, this, &LoginPage::onShowPasswordToggled);
}

void LoginPage::onLoginClicked()
{
    QString username = usernameEdit_->text();
    QString password = passwordEdit_->text();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool success = accountManager_->login(username.toStdString(), password.toStdString());

    if (success) {
        Account* user = accountManager_->getCurrentUser();
        QString role = "agent";
        if (dynamic_cast<AccountAdmin*>(user) != nullptr)
            role = "admin";
        emit loginSuccess(role, "fake_token");
    } else {
        QMessageBox::warning(this, "Đăng nhập thất bại",
                             "Username hoặc Password không chính xác.");
    }

    QApplication::restoreOverrideCursor();
}

void LoginPage::onShowPasswordToggled(bool checked)
{
    passwordEdit_->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}
