#include "AdminInterface.h"

// Include các trang con của Admin
#include "DashboardPage.h"
#include "AccountsPage.h"
#include "RoutesPage.h"  // <--- Sửa lỗi: Bỏ comment
#include "FlightsPage.h" // <--- Sửa lỗi: Bỏ comment

// Include các manager (để dùng khi tạo trang con)
#include "core/AccountManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "core/AirportManager.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QFrame>

// <--- Sửa lỗi: Xung đột Constructor (Lỗi 1)
AdminInterface::AdminInterface(AccountManager* accManager,
                               FlightManager* flManager,
                               BookingManager* bkManager,
                               ReportManager* reportManager,
                               AirportManager* airportManager,
                               QWidget* parent) 
    : QWidget(parent),
      accountManager_(accManager),
      flightManager_(flManager),
      bookingManager_(bkManager),
      reportManager_(reportManager),
      airportManager_(airportManager)
{
    // Bố cục chính là QHBoxLayout (Sidebar + Stack)
    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // --- Sidebar (Dựa trên code và ảnh của bạn) ---
    sidebar_ = new QFrame(this);
    sidebar_->setObjectName("Sidebar");
    sidebar_->setFixedWidth(220);
    
    auto *sideLay = new QVBoxLayout(sidebar_);
    sideLay->setContentsMargins(16,20,16,16);
    sideLay->setSpacing(8);

    auto mkSection = [](const QString& s){
        auto *l=new QLabel(s); 
        l->setStyleSheet("color:#BFD4FF;font-weight:600;margin:4px 0; background: transparent;"); 
        return l;
    };

    // Thông tin user
    auto *userIcon = new QLabel(this); 
    userIcon->setText("👤"); 
    userIcon->setStyleSheet("font-size: 32px; color: white; background: transparent;");
    userIcon->setAlignment(Qt::AlignCenter);
    
    auto *userName = new QLabel("Xin chào, Admin", this);
    userName->setStyleSheet("color: white; font-weight: 600; background: transparent;");
    userName->setAlignment(Qt::AlignCenter);
    
    sideLay->addWidget(userIcon);
    sideLay->addWidget(userName);
    sideLay->addSpacing(15);
    
    // --- Sửa lỗi: Cập nhật Sidebar theo file bạn gửi
    nav_ = new QListWidget(sidebar_);
    
    sideLay->addWidget(mkSection("QUẢN LÝ"));
    nav_->addItem("Trang tổng quan");       // Index 0
    nav_->addItem("Quản lý tuyến bay");    // Index 1
    nav_->addItem("Quản lý chuyến bay");   // Index 2
    
    sideLay->addWidget(mkSection("TÀI KHOẢN"));
    nav_->addItem("Quản lý tài khoản");     // Index 3

    sideLay->addWidget(nav_);
    sideLay->addStretch(); 

    logoutBtn_ = new QPushButton("Đăng xuất", sidebar_);
    sideLay->addWidget(logoutBtn_);

    // --- Stack (Nội dung chính) ---
    stack_ = new QStackedWidget(this);
    
    // <--- Sửa lỗi: Cập nhật StackWidget theo Sidebar
    stack_->addWidget(new DashboardPage(accountManager_, reportManager_, this));   // Index 0
    
    // Truyền FlightManager cho cả RoutesPage (vì nó quản lý tuyến)
    stack_->addWidget(new RoutesPage(flightManager_, airportManager_, this)); // Index 1
    
    // Truyền FlightManager cho FlightsPage (vì nó quản lý chuyến)
    stack_->addWidget(new FlightsPage(flightManager_, airportManager_, this)); // Index 2
    
    // Truyền AccountManager cho AccountsPage
    stack_->addWidget(new AccountsPage(accountManager_, this));    // Index 3

    nav_->setCurrentRow(0);
    stack_->setCurrentIndex(0);

    // --- Gắn vào layout chính ---
    root->addWidget(sidebar_);
    root->addWidget(stack_, 1); // 1 = co giãn

    // --- Kết nối Signal/Slot ---
    setupConnections();
}

void AdminInterface::setupConnections()
{
    // Kết nối sidebar nav với stack
    connect(nav_, &QListWidget::currentRowChanged, stack_, &QStackedWidget::setCurrentIndex);
    
    // Nút đăng xuất sẽ phát tín hiệu 'logoutClicked'
    connect(logoutBtn_, &QPushButton::clicked, this, &AdminInterface::logoutClicked);
}
