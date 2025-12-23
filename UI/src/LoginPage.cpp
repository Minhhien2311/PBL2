#include "LoginPage.h"
#include "core/AccountManager.h"
#include "entities/AccountAdmin.h"
#include <QApplication>
#include <QCheckBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QTimer>
#include <QVBoxLayout>
// ========== CONSTRUCTOR ==========
LoginPage::LoginPage(AccountManager* accManager, QWidget *parent)
    : QWidget(parent),
      accountManager_(accManager)
{
    Q_ASSERT(accountManager_ != nullptr);
    setupUi();
    setupConnections();
}

// ========== UI SETUP ==========

void LoginPage::setupUi()
{
    this->setAutoFillBackground(true);
    this->setStyleSheet(
        "QWidget { background: #ffffffff; }"
        "#AppTitle { font-size: 26px; font-weight: 750; color: #133e87; background: transparent; margin-top: 20px; margin-bottom: 20px; }"
        "#LoginBox { background: white; border: 1.6px solid #133e87; border-radius: 12px; }"
        "#LoginHeader { background: transparent; font-size: 40px; font-weight: 700; color: #133e87; }"
        "#LoginBoxTitle { font-size: 18px; font-weight: 600; color: #133e87; }"
        "QLineEdit { height: 36px; background: #f5f8fb; border: 1.2px solid #133e87; border-radius: 4px; padding-left: 10px; font-size: 13px; }"
        "QLineEdit:focus { border: 1.5px solid #133e87; background: #ffffff; }"
        "QPushButton#LoginButton { background: #608bc1; color: white; height: 30px; border-radius: 4px; font-weight: 650; font-size: 14px; }"
        "QPushButton#LoginButton:hover { background: #476c9a; }"
        "QPushButton#LoginButton:pressed { background: #365a9e; }"
        "QLabel#ForgotLabel { color: #133e87; font-size: 13px; }"
        "QCheckBox { color: #133e87; font-size: 13px; spacing: 8px; }"
        "QCheckBox::indicator { width: 10px; height: 10px; border-radius: 10px; font-weight: 550; }"
        "QCheckBox::indicator:unchecked { border: 1.2px solid #133e87; background: white; }"
        "QCheckBox::indicator:checked { border: 1.2px solid #133e87; background: #608bc1; }"
    );

    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 40, 0, 0);
    rootLayout->setAlignment(Qt::AlignCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(30);

    QLabel *appTitle = new QLabel("PHẦN MỀM QUẢN LÝ BÁN VÉ MÁY BAY", this);
    appTitle->setObjectName("AppTitle");
    appTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(appTitle);

    QFrame *loginBox = new QFrame(this);
    loginBox->setObjectName("LoginBox");
    loginBox->setFixedSize(460, 400); // Tăng chiều cao lên xíu cho thoáng (360 -> 400)
    loginBox->setFrameShape(QFrame::StyledPanel);

    // layout của hộp
    QVBoxLayout *boxLayout = new QVBoxLayout(loginBox);
    boxLayout->setContentsMargins(0, 0, 0, 15);       
    boxLayout->setSpacing(0);

    // ---------- HEADER TRẮNG BÊN TRONG ----------
    QWidget *header = new QWidget(loginBox);
    header->setObjectName("LoginHeader");
    header->setFixedHeight(58);

    QVBoxLayout *headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(0, 10, 0, 10); 
    headerLayout->setSpacing(0);

    QLabel *title = new QLabel("Đăng nhập", header);
    title->setObjectName("LoginBoxTitle");
    title->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(title);

    // Đường kẻ ngang
    QFrame *line = new QFrame(loginBox);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setStyleSheet("color: #133e87;");

    boxLayout->addWidget(header);
    boxLayout->addWidget(line);

    QWidget *content = new QWidget(loginBox);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    
    contentLayout->setContentsMargins(45, 36, 45, 10);
    contentLayout->setSpacing(15);

    usernameEdit_ = new QLineEdit(content);
    usernameEdit_->setPlaceholderText("Username");
    contentLayout->addWidget(usernameEdit_);

    passwordEdit_ = new QLineEdit(content);
    passwordEdit_->setPlaceholderText("Password");
    passwordEdit_->setEchoMode(QLineEdit::Password);
    contentLayout->addWidget(passwordEdit_);

    NotiLabel = new QLabel("", content);
    NotiLabel->setStyleSheet("color: #d32f2f; font-weight: 600; font-size: 12px;");
    NotiLabel->setWordWrap(true);
    NotiLabel->setVisible(false);
    contentLayout->addWidget(NotiLabel);

    showPasswordCheck_ = new QCheckBox("Hiển thị mật khẩu", content);
    showPasswordCheck_->setCursor(Qt::PointingHandCursor);
    contentLayout->addWidget(showPasswordCheck_);

    contentLayout->addSpacing(8);

    loginButton_ = new QPushButton("Đăng nhập", content);
    loginButton_->setObjectName("LoginButton");
    loginButton_->setCursor(Qt::PointingHandCursor);
    contentLayout->addWidget(loginButton_);

    forgotPasswordLabel_ = new QLabel(R"(<a href="#">Quên mật khẩu?</a>)", content);
    forgotPasswordLabel_->setTextInteractionFlags(Qt::TextBrowserInteraction);
    forgotPasswordLabel_->setObjectName("ForgotLabel");
    forgotPasswordLabel_->setAlignment(Qt::AlignCenter);
    forgotPasswordLabel_->setCursor(Qt::PointingHandCursor);
    forgotPasswordLabel_->setStyleSheet("QLabel#ForgotLabel { color: #133e87; text-decoration: none; font-size: 13px; } QLabel#ForgotLabel:hover { text-decoration: underline; }");
    
    contentLayout->addWidget(forgotPasswordLabel_);

    contentLayout->addStretch();

    boxLayout->addWidget(content);

    mainLayout->addWidget(loginBox, 0, Qt::AlignHCenter);
    mainLayout->addStretch();

    rootLayout->addStretch();
    rootLayout->addLayout(mainLayout);
    rootLayout->addStretch();
}

// ========== CONNECTIONS ==========

void LoginPage::setupConnections()
{
    connect(loginButton_, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(showPasswordCheck_, &QCheckBox::toggled, this, &LoginPage::onShowPasswordToggled);
    connect(forgotPasswordLabel_, &QLabel::linkActivated, this, &LoginPage::onForgotPasswordClicked);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
}

// ========== SLOTS ==========

void LoginPage::onLoginClicked()
{
    QString username = usernameEdit_->text();
    QString password = passwordEdit_->text();
    
    NotiLabel->setText("");

    if (username.isEmpty() || password.isEmpty()) {
        NotiLabel->setText("Vui lòng nhập đầy đủ thông tin.");
        NotiLabel->setVisible(true);
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool success = accountManager_->login(username.toStdString(), password.toStdString());

    if (success) {
        Account* user = accountManager_->getCurrentUser();
        QString role = "agent";
        if (dynamic_cast<AccountAdmin*>(user) != nullptr)
            role = "admin";
        
        emit loginSuccess(role, "fake_token");
    } else {
        NotiLabel->setText("Tên đăng nhập hoặc mật khẩu không đúng!");
        NotiLabel->setVisible(true);
        QTimer::singleShot(5000, NotiLabel, &QWidget::hide);
    }

    QApplication::restoreOverrideCursor();
}

void LoginPage::onShowPasswordToggled(bool checked)
{
    passwordEdit_->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void LoginPage::onForgotPasswordClicked()
{
    emit forgotPasswordClicked();
}