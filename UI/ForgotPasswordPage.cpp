#include "ForgotPasswordPage.h"
#include "core/AccountManager.h" 

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>

ForgotPasswordPage::ForgotPasswordPage(AccountManager* accManager, QWidget *parent)
    : QWidget(parent),
      accountManager_(accManager)
{
    Q_ASSERT(accountManager_ != nullptr);
    setupUi();
    setupConnections();
}

ForgotPasswordPage::~ForgotPasswordPage()
{
}

void ForgotPasswordPage::setupUi()
{
    // ---------- 1. STYLE ----------
    this->setAutoFillBackground(true);
    this->setStyleSheet(
        "QWidget { background: #ffffffff; }"
        "#AppTitle { font-size: 26px; font-weight: 750; color: #133e87; background: transparent; margin-top: 20px; margin-bottom: 20px; }"
        "#LoginBox { background: white; border: 1.6px solid #133e87; border-radius: 12px; }"
        
        // --- SỬA LỖI TẠI ĐÂY: HeaderContainer background transparent ---
        "#HeaderContainer { background: transparent; }" 
        
        "#BoxTitle { font-size: 18px; font-weight: 600; color: #133e87; }"
        // Input styling
        "QLineEdit { height: 36px; background: #f5f8fb; border: 1.2px solid #133e87; border-radius: 4px; padding-left: 10px; font-size: 13px; }"
        "QLineEdit:focus { border: 1.5px solid #133e87; background: #ffffff; }"

        // Checkbox styling
        "QCheckBox { color: #133e87; font-size: 13px; spacing: 8px; }"
        "QCheckBox::indicator { width: 10px; height: 10px; border-radius: 10px; font-weight: 550; }"
        "QCheckBox::indicator:unchecked { border: 1.2px solid #133e87; background: white; }"
        "QCheckBox::indicator:checked { border: 1.2px solid #133e87; background: #608bc1; }"
        // Back link styling
        "QLabel#BackLabel { color: #133e87; font-size: 13px; text-decoration: none; }"
        "QLabel#BackLabel:hover { text-decoration: underline; cursor: pointer; }"
    );

    // ---------- LAYOUT GỐC: căn giữa ngang ----------
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 40, 0, 0);
    rootLayout->setAlignment(Qt::AlignCenter);

    // ---------- CỘT CHÍNH ----------
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(30);

    // Tiêu đề App
    QLabel *appTitle = new QLabel("PHẦN MỀM QUẢN LÝ BÁN VÉ MÁY BAY", this);
    appTitle->setObjectName("AppTitle");
    appTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(appTitle);

    // ---------- HỘP (BOX) ----------
    QFrame *boxFrame = new QFrame(this);
    boxFrame->setObjectName("LoginBox"); 
    boxFrame->setFixedSize(460, 450); 
    boxFrame->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout *boxLayout = new QVBoxLayout(boxFrame);
    boxLayout->setContentsMargins(0, 0, 0, 15);
    boxLayout->setSpacing(0);

    // --- Header Box (SỬA LỖI: Thêm ObjectName) ---
    QWidget *header = new QWidget(boxFrame);
    header->setObjectName("HeaderContainer"); // <--- Thêm tên để set style transparent
    header->setFixedHeight(58);
    
    QVBoxLayout *headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(0, 10, 0, 10);
    
    QLabel *title = new QLabel("Đặt lại mật khẩu", header);
    title->setObjectName("BoxTitle");
    title->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(title);

    // Đường kẻ ngang
    QFrame *line = new QFrame(boxFrame);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setStyleSheet("color: #133e87;");

    boxLayout->addWidget(header);
    boxLayout->addWidget(line);

    // --- Content ---
    QWidget *content = new QWidget(boxFrame);
    // Lưu ý: Content cũng nên set transparent nếu cần, nhưng thường nó nằm dưới line nên ko ảnh hưởng border trên
    content->setStyleSheet("background: transparent;"); 
    
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(45, 25, 45, 10); 
    contentLayout->setSpacing(15);

    // Input 1: Email
    emailEdit_ = new QLineEdit(content);
    emailEdit_->setPlaceholderText("Nhập Email đăng ký tài khoản");
    contentLayout->addWidget(emailEdit_);

    // Input 2: Mật khẩu mới
    newPassEdit_ = new QLineEdit(content);
    newPassEdit_->setPlaceholderText("Mật khẩu mới");
    newPassEdit_->setEchoMode(QLineEdit::Password);
    contentLayout->addWidget(newPassEdit_);

    // Input 3: Xác nhận mật khẩu
    confirmPassEdit_ = new QLineEdit(content);
    confirmPassEdit_->setPlaceholderText("Xác nhận mật khẩu mới");
    confirmPassEdit_->setEchoMode(QLineEdit::Password);
    contentLayout->addWidget(confirmPassEdit_);

    // Label thông báo lỗi
    notiLabel_ = new QLabel("", content);
    notiLabel_->setStyleSheet("color: #d32f2f; font-weight: 600; font-size: 12px;");
    notiLabel_->setWordWrap(true);
    notiLabel_->setVisible(false);
    contentLayout->addWidget(notiLabel_);

    // Checkbox hiển thị mật khẩu
    showPasswordCheck_ = new QCheckBox("Hiển thị mật khẩu", content);
    showPasswordCheck_->setCursor(Qt::PointingHandCursor);
    contentLayout->addWidget(showPasswordCheck_);

    contentLayout->addSpacing(8);

    // Button Xác nhận
    resetButton_ = new QPushButton("Xác nhận đổi mật khẩu", content);
    resetButton_->setObjectName("ResetButton");
    resetButton_->setStyleSheet(
        "QPushButton { background: #608bc1; color: white; height: 36px; border-radius: 4px; font-weight: 650; font-size: 14px; }"
        "QPushButton:hover { background: #476c9a; }"
        "QPushButton:pressed { background: #365a9e; }"
    );
    resetButton_->setCursor(Qt::PointingHandCursor);
    contentLayout->addWidget(resetButton_);

    // Link quay lại đăng nhập
    backLabel_ = new QLabel("<a href='#'>Quay lại đăng nhập</a>", content);
    backLabel_->setObjectName("BackLabel");
    backLabel_->setAlignment(Qt::AlignCenter);
    backLabel_->setTextInteractionFlags(Qt::TextBrowserInteraction);
    backLabel_->setCursor(Qt::PointingHandCursor);
    contentLayout->addWidget(backLabel_);

    contentLayout->addStretch();
    boxLayout->addWidget(content);

    // Thêm box vào layout chính
    mainLayout->addWidget(boxFrame, 0, Qt::AlignHCenter);
    mainLayout->addStretch();

    rootLayout->addStretch();
    rootLayout->addLayout(mainLayout);
    rootLayout->addStretch();
}

void ForgotPasswordPage::setupConnections()
{
    connect(resetButton_, &QPushButton::clicked, this, &ForgotPasswordPage::onResetClicked);
    connect(backLabel_, &QLabel::linkActivated, this, &ForgotPasswordPage::onBackClicked);
    connect(showPasswordCheck_, &QCheckBox::toggled, this, &ForgotPasswordPage::onShowPasswordToggled);
    
    // Nhấn Enter ở ô confirm cũng trigger submit
    connect(confirmPassEdit_, &QLineEdit::returnPressed, this, &ForgotPasswordPage::onResetClicked);
}

void ForgotPasswordPage::clearFields()
{
    emailEdit_->clear();
    newPassEdit_->clear();
    confirmPassEdit_->clear();
    notiLabel_->clear();
    notiLabel_->setVisible(false);
    showPasswordCheck_->setChecked(false);
}

void ForgotPasswordPage::onShowPasswordToggled(bool checked)
{
    QLineEdit::EchoMode mode = checked ? QLineEdit::Normal : QLineEdit::Password;
    newPassEdit_->setEchoMode(mode);
    confirmPassEdit_->setEchoMode(mode);
}

void ForgotPasswordPage::onBackClicked()
{
    // Phát tín hiệu để MainWindow chuyển Scene
    emit backToLogin();
}

void ForgotPasswordPage::onResetClicked()
{
    QString email = emailEdit_->text().trimmed();
    QString newPass = newPassEdit_->text();
    QString confirm = confirmPassEdit_->text();

    notiLabel_->setVisible(false);

    // 1. Validate đầu vào
    if (email.isEmpty() || newPass.isEmpty() || confirm.isEmpty()) {
        notiLabel_->setText("Vui lòng nhập đầy đủ thông tin.");
        notiLabel_->setVisible(true);
        QTimer::singleShot(3000, notiLabel_, &QWidget::hide); // Use QTimer::singleShot for non-blocking delay
        return;
    }

    if (newPass != confirm) {
        notiLabel_->setText("Mật khẩu xác nhận không trùng khớp!");
        notiLabel_->setVisible(true);
        return;
    }

    if (newPass.length() < 6) { 
        notiLabel_->setText("Mật khẩu mới phải có ít nhất 6 ký tự.");
        notiLabel_->setVisible(true);
        return;
    }

    // 2. Gọi AccountManager thực hiện reset
    bool success = accountManager_->resetPassword(email.toStdString(), newPass.toStdString());

    if (success) {
        notiLabel_->setText("Đặt lại mật khẩu thành công! Đang quay về trang đăng nhập...");
        notiLabel_->setStyleSheet("color: #00bf63; font-weight: 600; font-size: 12px;");
        notiLabel_->setVisible(true);
        QTimer::singleShot(2000, this, [this]() {
            clearFields();
            emit backToLogin();
        });
    } else {
        notiLabel_->setText("Email không tồn tại hoặc lỗi hệ thống.");
        notiLabel_->setVisible(true);
    }
}