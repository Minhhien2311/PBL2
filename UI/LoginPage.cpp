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
#include <QTimer>

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
    // Tinh chỉnh lại CSS một chút cho Input và Button đẹp hơn
    this->setStyleSheet(
        "QWidget { background: #ffffffff; }"
        "#AppTitle { font-size: 26px; font-weight: 750; color: #133e87; background: transparent; margin-top: 20px; margin-bottom: 20px; }"
        "#LoginBox { background: white; border: 1.6px solid #133e87; border-radius: 12px; }"
        "#LoginHeader { background: transparent; font-size: 40px; font-weight: 700; color: #133e87; }"
        "#LoginBoxTitle { font-size: 18px; font-weight: 600; color: #133e87; }"
        // Input: tăng chiều cao lên 36px cho dễ nhập
        "QLineEdit { height: 36px; background: #f5f8fb; border: 1.2px solid #133e87; border-radius: 4px; padding-left: 10px; font-size: 13px; }"
        "QLineEdit:focus { border: 1.5px solid #133e87; background: #ffffff; }"
        // Button: tăng chiều cao, font chữ
        "QPushButton#LoginButton { background: #608bc1; color: white; height: 30px; border-radius: 4px; font-weight: 650; font-size: 14px; }"
        "QPushButton#LoginButton:hover { background: #476c9a; }"
        "QPushButton#LoginButton:pressed { background: #365a9e; }"
        "QLabel#ForgotLabel { color: #133e87; font-size: 13px; }"
        "QCheckBox { color: #133e87; font-size: 13px; spacing: 8px; }"
        "QCheckBox::indicator { width: 10px; height: 10px; border-radius: 10px; font-weight: 550; }"
        "QCheckBox::indicator:unchecked { border: 1.2px solid #133e87; background: white; }"
        "QCheckBox::indicator:checked { border: 1.2px solid #133e87; background: #608bc1; }"
    );

    // ---------- LAYOUT GỐC: căn giữa ngang ----------
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 40, 0, 0);
    rootLayout->setAlignment(Qt::AlignCenter);

    // ---------- CỘT CHÍNH ----------
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(30); // Khoảng cách giữa tiêu đề to và box đăng nhập

    // Tiêu đề App
    QLabel *appTitle = new QLabel("PHẦN MỀM QUẢN LÝ BÁN VÉ MÁY BAY", this);
    appTitle->setObjectName("AppTitle");
    appTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(appTitle);

    // ---------- HỘP ĐĂNG NHẬP ----------
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

    // ---------- PHẦN NỘI DUNG FORM (QUAN TRỌNG NHẤT) ----------
    QWidget *content = new QWidget(loginBox);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    
    // THỦ THUẬT CĂN THẲNG HÀNG:
    // Set margin trái phải là 45px. Tất cả các widget bên trong sẽ tự động cách lề 45px.
    // Input và Button sẽ tự giãn ra cho đầy phần còn lại -> Bằng nhau chằn chặn.
    contentLayout->setContentsMargins(45, 36, 45, 10); 
    contentLayout->setSpacing(15); // Khoảng cách giữa các ô input

    // 1. Username
    usernameEdit_ = new QLineEdit(content);
    usernameEdit_->setPlaceholderText("Username");
    // Bỏ setFixedWidth -> Để layout tự co giãn
    contentLayout->addWidget(usernameEdit_);

    // 2. Password
    passwordEdit_ = new QLineEdit(content);
    passwordEdit_->setPlaceholderText("Password");
    passwordEdit_->setEchoMode(QLineEdit::Password);
    contentLayout->addWidget(passwordEdit_);

    // 3. Thông báo lỗi (Ẩn/Hiện)
    NotiLabel = new QLabel("", content);
    // Style: Chữ đỏ, in đậm
    NotiLabel->setStyleSheet("color: #d32f2f; font-weight: 600; font-size: 12px;");
    NotiLabel->setWordWrap(true); // Tự xuống dòng nếu lỗi dài
    NotiLabel->setVisible(false); // Mặc định ẩn
    contentLayout->addWidget(NotiLabel);

    // 4. Checkbox hiển thị mật khẩu
    showPasswordCheck_ = new QCheckBox("Hiển thị mật khẩu", content);
    showPasswordCheck_->setCursor(Qt::PointingHandCursor);
    // Checkbox tự động nằm bên trái (AlignLeft mặc định của QVBoxLayout)
    contentLayout->addWidget(showPasswordCheck_);

    // Khoảng cách nhỏ trước nút bấm
    contentLayout->addSpacing(8);

    // 5. Nút đăng nhập
    loginButton_ = new QPushButton("Đăng nhập", content);
    loginButton_->setObjectName("LoginButton");
    loginButton_->setCursor(Qt::PointingHandCursor);
    contentLayout->addWidget(loginButton_);

    // 6. "Quên mật khẩu?"
    forgotPasswordLabel_ = new QLabel(R"(<a href="#">Quên mật khẩu?</a>)", content);
    forgotPasswordLabel_->setTextInteractionFlags(Qt::TextBrowserInteraction);
    forgotPasswordLabel_->setObjectName("ForgotLabel");
    forgotPasswordLabel_->setAlignment(Qt::AlignCenter); // Giữ căn giữa cho dòng này
    forgotPasswordLabel_->setCursor(Qt::PointingHandCursor);
    forgotPasswordLabel_->setStyleSheet("QLabel#ForgotLabel { color: #133e87; text-decoration: none; font-size: 13px; } QLabel#ForgotLabel:hover { text-decoration: underline; }");
    
    contentLayout->addWidget(forgotPasswordLabel_);

    contentLayout->addStretch();

    // Thêm content vào box layout
    boxLayout->addWidget(content);

    // Căn giữa hộp trong layout chính
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
    connect(forgotPasswordLabel_, &QLabel::linkActivated, this, &LoginPage::onForgotPasswordClicked);
    // Thêm: Nhấn Enter ở ô pass cũng login luôn cho tiện
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
}

void LoginPage::onLoginClicked()
{
    QString username = usernameEdit_->text();
    QString password = passwordEdit_->text();
    
    // Clear thông báo lỗi cũ
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
    // Bắn tín hiệu ra ngoài để MainWindow (hoặc AppController) chuyển trang
    emit forgotPasswordClicked();
}