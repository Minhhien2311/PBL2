#include "AgentInterface.h"

// --- Include các Trang con MỚI ---
#include "SearchBookPage.h"    // Trang tìm kiếm & đặt vé
#include "AgentBookingsPage.h" // Trang quản lý vé đã đặt
#include "AccountsPage.h"      // Tái sử dụng trang tài khoản
#include "DashboardPage.h"    // Trang tổng quan

// (Include các manager để truyền đi)
#include "core/AccountManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QFrame>

// <--- CẬP NHẬT: Constructor nhận cả 4 manager
AgentInterface::AgentInterface(AccountManager* accManager,
                               FlightManager* flManager,
                               BookingManager* bkManager,
                               ReportManager* reportManager,
                               QWidget *parent) 
    : QWidget(parent),
      accountManager_(accManager),
      flightManager_(flManager),
      bookingManager_(bkManager),
      reportManager_(reportManager)
{
    // Đảm bảo manager không null
    Q_ASSERT(accountManager_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(reportManager_ != nullptr);

    setupUi(); // Gọi hàm tạo UI
}

void AgentInterface::setupUi()
{
    // Bố cục chính
    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // --- Sidebar (Giao diện Agent) ---
    sidebar_ = new QFrame(this);
    sidebar_->setObjectName("Sidebar");
    sidebar_->setFixedWidth(220);
    
    auto *sideLay = new QVBoxLayout(sidebar_);
    sideLay->setContentsMargins(16,20,16,16);
    sideLay->setSpacing(8);

    // Thông tin user (Giống Admin)
    auto *userIcon = new QLabel(this); 
    userIcon->setText("👤"); 
    userIcon->setStyleSheet("font-size: 32px; color: white; background: transparent;");
    userIcon->setAlignment(Qt::AlignCenter);
    
    auto *userName = new QLabel("Xin chào, Đại lý", this); // Đổi text
    userName->setStyleSheet("color: white; font-weight: 600; background: transparent;");
    userName->setAlignment(Qt::AlignCenter);
    
    sideLay->addWidget(userIcon);
    sideLay->addWidget(userName);
    sideLay->addSpacing(15);

    // Helper tạo label "NGHIỆP VỤ", "TÀI KHOẢN"...
    auto mkSection = [](const QString& s){
        auto *l=new QLabel(s); 
        l->setStyleSheet("color:#BFD4FF;font-weight:600;margin:4px 0; background: transparent;"); 
        return l;
    };
    
    // --- Navigation (Sidebar của Agent) ---
    nav_ = new QListWidget(sidebar_);
    
    // sideLay->addWidget(mkSection("NGHIỆP VỤ"));
    nav_->addItem("Tổng quan");
    nav_->addItem("Tìm & Đặt vé");        // Index 0
    nav_->addItem("Quản lý Đặt chỗ");    // Index 1
    
    // sideLay->addWidget(mkSection("TÀI KHOẢN"));
    nav_->addItem("Thông tin tài khoản");   // Index 2

    sideLay->addWidget(nav_);
    sideLay->addStretch(); 

    logoutBtn_ = new QPushButton("Đăng xuất", sidebar_);
    sideLay->addWidget(logoutBtn_);

    // --- Stack (Nội dung chính của Agent) ---
    stack_ = new QStackedWidget(this);
    
    stack_->addWidget(new DashboardPage(accountManager_, reportManager_, this));

    // 0. Trang Tìm & Đặt vé
    stack_->addWidget(new SearchBookPage(flightManager_, bookingManager_, accountManager_, this));
    
    // 1. Trang Quản lý Đặt chỗ
    stack_->addWidget(new AgentBookingsPage(bookingManager_, flightManager_, accountManager_, this));
    
    // 2. Tái sử dụng Trang Tài khoản
    stack_->addWidget(new AccountsPage(accountManager_, this)); 

    nav_->setCurrentRow(0);
    stack_->setCurrentIndex(0);

    // --- Gắn vào layout chính ---
    root->addWidget(sidebar_);
    root->addWidget(stack_, 1); // 1 = co giãn

    // --- Kết nối Signal/Slot ---
    setupConnections();
}


void AgentInterface::setupConnections()
{
    // Kết nối sidebar nav với stack
    connect(nav_, &QListWidget::currentRowChanged, stack_, &QStackedWidget::setCurrentIndex);
    
    // Nút đăng xuất sẽ phát tín hiệu 'logoutClicked'
    connect(logoutBtn_, &QPushButton::clicked, this, &AgentInterface::logoutClicked);
}