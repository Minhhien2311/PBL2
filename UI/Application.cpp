#include "Application.h"

// Include các trang con cấp cao
#include "LoginPage.h"
#include "ForgotPasswordPage.h" // <--- THÊM INCLUDE NÀY
#include "AdminInterface.h"
#include "AgentInterface.h"

// Include các manager (để C++ biết các class này là gì)
#include "core/AccountManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "core/AirportManager.h"
#include "core/PassengerManager.h"

#include <QTimer> // Required for QTimer::singleShot
#include <QStackedWidget>

// Constructor
Application::Application(AccountManager* accManager, 
                         FlightManager* flManager, 
                         BookingManager* bkManager,
                         ReportManager* reportManager,
                         AirportManager* airportManager,
                         PassengerManager* passengerManager,
                         QWidget *parent)
    : QMainWindow(parent), 
      accountManager_(accManager), 
      flightManager_(flManager),
      bookingManager_(bkManager),
      reportManager_(reportManager),
      airportManager_(airportManager),
      passengerManager_(passengerManager)
{
    setWindowTitle("Hệ thống Quản lý Bán vé máy bay");
    
    setupUi();
    setupConnections();

    // --- QSS giao diện (Giữ nguyên phần style cũ của bạn) ---
        QString qss = R"(
            /* === STYLE CƠ BẢN === */
            QWidget { background: white; color: #333; font-family: 'Segoe UI'; }
            QMainWindow { background: white; }
            QFrame#Sidebar { background: #133e87; }
            QLabel#AppTitle { color:#123B7A; font-weight:700; font-size:24px; }
            QFrame#LoginBox { border: 1px solid #C9D6EB; border-radius: 8px; background: white; color: #333; }
            
            /* === STYLE SIDEBAR LIST === */
            QListWidget { 
                border: none; color: white; background: transparent; outline: 0; padding: 0px; 
            }
            QListWidget::item { 
                padding: 8px 15px; margin: 0px; border: none;
            }
            QListWidget::item:selected, QListWidget::item:hover { 
                background: #1C4E99; border: none; 
            }
            
            /* === STYLE BUTTON, INPUT === */
            QPushButton { 
                border:1px solid #7AA0D4; border-radius:8px; padding:6px 10px; 
                background:#5B86C6; color:white; font-weight:600;
            }
            QPushButton:hover { background:#6B97D8; }
            
            QLineEdit, QComboBox, QDateEdit, QTimeEdit, QSpinBox { 
                border: 1px solid #608bc1; border-radius: 6px; padding: 4px 10px; 
                background: white; color: #333; height: 30px; font-size: 13px;
            }
            QLineEdit:focus, QComboBox:focus { border: 1.5px solid #4472C4; }

            /* === STYLE TABLE === */
            QTableView {
                border: 1px solid #133e87; gridline-color: transparent; background: white; outline: 0;
            }
            QTableView::item { border-bottom: 1px solid #C9D6EB; border-right: none; }
            QHeaderView::section {
                background-color: #133e87; color: white; padding: 8px; border: none; font-weight: 600; 
            }
            QTableView::item:hover { background-color: #EAF2F8; color: #123B7A; }
            QTableView::item:selected { background-color: #EAF2F8; color: black; }

            /* === SCROLLBAR === */
            QScrollBar:vertical { border: none; background: #cbdceb; width: 6px; margin: 0; }
            QScrollBar::handle:vertical { background: #608bc1; border-radius: 3px; min-height: 20px; }
            QScrollBar:horizontal { border: none; background: #cbdceb; height: 6px; margin: 0; }
            QScrollBar::handle:horizontal { background: #608bc1; border-radius: 3px; min-width: 20px; }
            )";
        this->setStyleSheet(qss);
    
    QTimer::singleShot(0, this, [this]() {
        showMaximized();
    });
}

Application::~Application() {}

void Application::setupUi()
{
    // 1. Khởi tạo LoginPage
    loginPage_ = new LoginPage(accountManager_, this); 

    // 2. Khởi tạo ForgotPasswordPage (THÊM MỚI)
    forgotPasswordPage_ = new ForgotPasswordPage(accountManager_, this);
    
    // 3. Khởi tạo AdminInterface
    adminInterface_ = new AdminInterface(accountManager_, 
                                         flightManager_, 
                                         bookingManager_,
                                         reportManager_,
                                         airportManager_,
                                         passengerManager_,
                                         this);
    
    // 4. Khởi tạo AgentInterface
    agentInterface_ = new AgentInterface(accountManager_, flightManager_, 
                                         bookingManager_, reportManager_,
                                         airportManager_, passengerManager_, this);

    // 5. Thêm tất cả vào StackedWidget
    stack_ = new QStackedWidget(this);
    stack_->addWidget(loginPage_);          // Index 0
    stack_->addWidget(adminInterface_);     // Index 1
    stack_->addWidget(agentInterface_);     // Index 2
    stack_->addWidget(forgotPasswordPage_); // Index 3 (THÊM MỚI)

    setCentralWidget(stack_);
    stack_->setCurrentWidget(loginPage_);
}

void Application::setupConnections()
{
    // --- LOGIN PAGE ---
    connect(loginPage_, &LoginPage::loginSuccess, this, &Application::onLoginSuccess);

    // [MỚI] Chuyển từ Login sang Forgot Password
    connect(loginPage_, &LoginPage::forgotPasswordClicked, this, [this]() {
        forgotPasswordPage_->clearFields(); // Xóa dữ liệu cũ nếu có
        stack_->setCurrentWidget(forgotPasswordPage_);
    });

    // --- FORGOT PASSWORD PAGE ---
    // [MỚI] Chuyển từ Forgot Password quay lại Login
    connect(forgotPasswordPage_, &ForgotPasswordPage::backToLogin, this, [this]() {
        stack_->setCurrentWidget(loginPage_);
    });

    // --- INTERFACES ---
    connect(adminInterface_, &AdminInterface::logoutClicked, this, &Application::onLogout);
    connect(agentInterface_, &AgentInterface::logoutClicked, this, &Application::onLogout);
}

void Application::onLoginSuccess(const QString& role, const QString& token)
{
    if (role == "admin") {
        stack_->setCurrentWidget(adminInterface_);
    } 
    else if (role == "agent") {
        stack_->setCurrentWidget(agentInterface_);
    }
}

void Application::onLogout()
{
    if(accountManager_) {
        accountManager_->logout();
    }
    // Quay về trang login
    stack_->setCurrentWidget(loginPage_);
}