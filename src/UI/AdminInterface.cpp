#include "AdminInterface.h"
#include "DashboardPage.h"
#include "AccountsPage.h"
#include "RoutesPage.h"
#include "FlightsPage.h"
#include "AdminTicketsReportPage.h"  // Mới
#include "AdminRevenueReportPage.h"  // Mới
#include "FlightRulesPage.h"         // Tạo mới - placeholder
#include "PromotionsPage.h"          // Tạo mới - placeholder
#include "AgentListPage.h"           // Tạo mới - placeholder
#include "PageRefresher.h"

#include "core/AccountManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "core/AirportManager.h"
#include "core/PassengerManager.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>

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
    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Sidebar
    sidebar_ = new QFrame(this);
    sidebar_->setFixedWidth(180);  // Tăng chiều rộng một chút
    sidebar_->setStyleSheet("background-color: #133e87;");

    auto *sideLay = new QVBoxLayout(sidebar_);
    sideLay->setContentsMargins(16, 20, 16, 16);
    sideLay->setSpacing(8);

    // Thông tin user
    auto *userIcon = new QLabel("👤", sidebar_);
    userIcon->setAlignment(Qt::AlignCenter);
    userIcon->setStyleSheet("font-size: 32px; color: white; background: transparent;");
    auto *userName = new QLabel("Xin chào, Admin", sidebar_);
    userName->setAlignment(Qt::AlignCenter);
    userName->setStyleSheet("color: white; font-weight: 600; background: transparent;");

    sideLay->addWidget(userIcon);
    sideLay->addWidget(userName);
    sideLay->addSpacing(15);

    // Menu sidebar
    auto *menuWidget = new QWidget(sidebar_);
    auto *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(0,0,0,0);
    menuLayout->setSpacing(2);

    QString btnStyle = R"(
        QPushButton {
            color: white;
            background: transparent;
            border: none;
            text-align: left;
            padding: 8px;
        }
        QPushButton:hover {
            background-color: #6f99c6;
            border-radius: 6px;
        }
        QPushButton:checked {
            background-color: #6f99c6;
        }
    )";

    // --- QUẢN LÝ ---
    auto *header1 = new QLabel("QUẢN LÝ", sidebar_);
    header1->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent; margin-top: 10px;");
    menuLayout->addWidget(header1);

    btnDashboard_ = new QPushButton("Tổng quan");
    btnRoutes_    = new QPushButton("Quản lý tuyến bay");
    btnFlights_   = new QPushButton("Quản lý chuyến bay");
    btnFlightRules_ = new QPushButton("Quản lý luật bay");
    btnPromotions_ = new QPushButton("Quản lý khuyến mãi");
    
    btnDashboard_->setStyleSheet(btnStyle); 
    btnRoutes_->setStyleSheet(btnStyle); 
    btnFlights_->setStyleSheet(btnStyle);
    btnFlightRules_->setStyleSheet(btnStyle);
    btnPromotions_->setStyleSheet(btnStyle);
    
    btnDashboard_->setCheckable(true); 
    btnRoutes_->setCheckable(true); 
    btnFlights_->setCheckable(true);
    btnFlightRules_->setCheckable(true);
    btnPromotions_->setCheckable(true);
    
    menuLayout->addWidget(btnDashboard_);
    menuLayout->addWidget(btnRoutes_);
    menuLayout->addWidget(btnFlights_);
    menuLayout->addWidget(btnFlightRules_);
    menuLayout->addWidget(btnPromotions_);

    // --- THỐNG KÊ ---
    auto *header2 = new QLabel("THỐNG KÊ", sidebar_);
    header2->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent; margin-top: 10px;");
    menuLayout->addWidget(header2);

    btnTicketsReport_ = new QPushButton("Vé đã bán");
    btnRevenueReport_ = new QPushButton("Doanh thu");
    
    btnTicketsReport_->setStyleSheet(btnStyle);
    btnRevenueReport_->setStyleSheet(btnStyle);
    
    btnTicketsReport_->setCheckable(true);
    btnRevenueReport_->setCheckable(true);
    
    menuLayout->addWidget(btnTicketsReport_);
    menuLayout->addWidget(btnRevenueReport_);

    // --- TÀI KHOẢN ---
    auto *header3 = new QLabel("TÀI KHOẢN", sidebar_);
    header3->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent; margin-top: 10px;");
    menuLayout->addWidget(header3);

    btnAccounts_ = new QPushButton("Quản lý tài khoản");
    btnAgentList_ = new QPushButton("Danh sách đại lý");
    
    btnAccounts_->setStyleSheet(btnStyle);
    btnAgentList_->setStyleSheet(btnStyle);
    
    btnAccounts_->setCheckable(true);
    btnAgentList_->setCheckable(true);
    
    menuLayout->addWidget(btnAccounts_);
    menuLayout->addWidget(btnAgentList_);

    menuLayout->addStretch();
    menuWidget->setLayout(menuLayout);
    sideLay->addWidget(menuWidget);

    // Logout
    logoutBtn_ = new QPushButton("Đăng xuất", sidebar_);
    logoutBtn_->setStyleSheet(R"(
        QPushButton {
            color: white;
            background: transparent;
            border: 1px solid #6f99c6;
            border-radius: 6px;
            padding: 8px;
            margin-top: 10px;
        }
        QPushButton:hover {
            background-color: #6f99c6;
            color: white;
        }
    )");
    sideLay->addWidget(logoutBtn_);

    root->addWidget(sidebar_);

    // Stack - Thêm các trang mới
    stack_ = new QStackedWidget(this);
    stack_->addWidget(new DashboardPage(accountManager_, reportManager_, this)); // 0
    stack_->addWidget(new RoutesPage(flightManager_, airportManager_, this));   // 1
    stack_->addWidget(new FlightsPage(flightManager_, airportManager_, this));  // 2
    stack_->addWidget(new FlightRulesPage(flightManager_, this));               // 3 - placeholder
    stack_->addWidget(new PromotionsPage(this));                                // 4 - placeholder
    stack_->addWidget(new AdminTicketsReportPage(accountManager_, bookingManager_, reportManager_, this)); // 5
    stack_->addWidget(new AdminRevenueReportPage(accountManager_, bookingManager_, reportManager_, this)); // 6
    stack_->addWidget(new AccountsPage(accountManager_, this));                 // 7
    stack_->addWidget(new AgentListPage(accountManager_, this));                // 8 - placeholder

    root->addWidget(stack_, 1);

    setupConnections();

    stack_->setCurrentIndex(0);
    btnDashboard_->setChecked(true);
}

void AdminInterface::setupConnections()
{
    auto switchPage = [this](QPushButton* btn, int index){
        stack_->setCurrentIndex(index);
        // Bỏ check tất cả các nút
        btnDashboard_->setChecked(false);
        btnRoutes_->setChecked(false);
        btnFlights_->setChecked(false);
        btnFlightRules_->setChecked(false);
        btnPromotions_->setChecked(false);
        btnTicketsReport_->setChecked(false);
        btnRevenueReport_->setChecked(false);
        btnAccounts_->setChecked(false);
        btnAgentList_->setChecked(false);
        
        // Check nút được chọn
        btn->setChecked(true);
    };

    connect(btnDashboard_, &QPushButton::clicked, [=](){ switchPage(btnDashboard_, 0); });
    connect(btnRoutes_, &QPushButton::clicked, [=](){ switchPage(btnRoutes_, 1); });
    connect(btnFlights_, &QPushButton::clicked, [=](){ switchPage(btnFlights_, 2); });
    connect(btnFlightRules_, &QPushButton::clicked, [=](){ switchPage(btnFlightRules_, 3); });
    connect(btnPromotions_, &QPushButton::clicked, [=](){ switchPage(btnPromotions_, 4); });
    connect(btnTicketsReport_, &QPushButton::clicked, [=](){ switchPage(btnTicketsReport_, 5); });
    connect(btnRevenueReport_, &QPushButton::clicked, [=](){ switchPage(btnRevenueReport_, 6); });
    connect(btnAccounts_, &QPushButton::clicked, [=](){ switchPage(btnAccounts_, 7); });
    connect(btnAgentList_, &QPushButton::clicked, [=](){ switchPage(btnAgentList_, 8); });

    connect(logoutBtn_, &QPushButton::clicked, this, &AdminInterface::logoutClicked);
}
