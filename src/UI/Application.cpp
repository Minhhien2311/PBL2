#include "Application.h"

// Include các trang
#include "LoginPage.h"
#include "AdminInterface.h"
#include "AgentInterface.h"

// Include các manager (để C++ biết các class này là gì)
#include "core/AccountManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"

#include <QStackedWidget>

// Constructor mới
Application::Application(AccountManager* accManager, 
                         FlightManager* flManager, 
                         BookingManager* bkManager,
                         ReportManager* reportManager,
                         QWidget *parent)
    : QMainWindow(parent), 
      accountManager_(accManager), 
      flightManager_(flManager),    // <--- Sửa lỗi: Tên nhất quán (số ít)
      bookingManager_(bkManager),
      reportManager_(reportManager)
{
    setWindowTitle("Hệ thống Quản lý Bán vé máy bay");
    resize(1100, 700); 

    setupUi();
    setupConnections();
}

Application::~Application() { }

void Application::setupUi()
{
    // Khởi tạo các trang và "truyền" manager vào
    // <--- Sửa lỗi: Truyền AccountManager cho LoginPage
    loginPage_ = new LoginPage(accountManager_, this); 
    
    // <--- Sửa lỗi: Truyền cả 4 manager cho AdminInterface
    adminInterface_ = new AdminInterface(accountManager_, 
                                         flightManager_, 
                                         bookingManager_,
                                         reportManager_,
                                         this);
    
    // <--- Sửa lỗi: Truyền cả 4 manager cho AgentInterface
    agentInterface_ = new AgentInterface(accountManager_, flightManager_, bookingManager_, reportManager_, this);

    // Khởi tạo QStackedWidget
    stack_ = new QStackedWidget(this);
    stack_->addWidget(loginPage_);      // Index 0
    stack_->addWidget(adminInterface_); // Index 1
    stack_->addWidget(agentInterface_); // Index 2

    // Đặt làm widget trung tâm
    setCentralWidget(stack_);

    // Mặc định hiển thị trang đăng nhập
    stack_->setCurrentWidget(loginPage_);
}

void Application::setupConnections()
{
    // Kết nối tín hiệu đăng nhập thành công từ LoginPage
    connect(loginPage_, &LoginPage::loginSuccess, this, &Application::onLoginSuccess);

    // Kết nối tín hiệu đăng xuất từ cả 2 interface
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
    // Gọi hàm logout của core để xóa session
    if(accountManager_) {
        accountManager_->logout();
    }
    stack_->setCurrentWidget(loginPage_);
}

