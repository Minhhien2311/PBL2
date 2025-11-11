#include "Application.h"

// Include các trang con cấp cao
#include "LoginPage.h"
#include "AdminInterface.h"
#include "AgentInterface.h"

// Include các manager (để C++ biết các class này là gì)
#include "core/AccountManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "core/AirportManager.h"

#include <QTimer> // Required for QTimer::singleShot
#include <QStackedWidget>

// Constructor
Application::Application(AccountManager* accManager, 
                         FlightManager* flManager, 
                         BookingManager* bkManager,
                         ReportManager* reportManager,
                         AirportManager* airportManager,
                         QWidget *parent)
    : QMainWindow(parent), 
      accountManager_(accManager), 
      flightManager_(flManager),
      bookingManager_(bkManager),
      reportManager_(reportManager),
      airportManager_(airportManager)
{
    setWindowTitle("Hệ thống Quản lý Bán vé máy bay");
    // Xóa dòng resize
    // resize(1100, 700);  // <-- XÓA DÒNG NÀY

    setupUi();
    setupConnections();
    
    // Trì hoãn maximize để layout được tính toán xong
    QTimer::singleShot(0, this, [this]() {
        showMaximized();
    });
}

Application::~Application() {}

void Application::setupUi()
{
    // Khởi tạo các trang và "truyền" manager vào
    loginPage_ = new LoginPage(accountManager_, this); 
    
    // Truyền cả 4 manager cho AdminInterface
    adminInterface_ = new AdminInterface(accountManager_, 
                                         flightManager_, 
                                         bookingManager_,
                                         reportManager_,
                                         airportManager_,
                                         this);
    
    // Truyền cả 4 manager cho AgentInterface
    agentInterface_ = new AgentInterface(accountManager_, flightManager_, 
                                         bookingManager_, reportManager_,
                                         airportManager_, this);

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

// Kết nối tín hiệu
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
