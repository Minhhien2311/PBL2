#include "AgentInterface.h"

// --- Include các trang con ---
#include "SearchBookPage.h"
#include "AgentBookingsPage.h"
#include "AccountsPage.h"
#include "DashboardPage.h"
#include "AgentReportPage.h"

// Managers
#include "core/AccountManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "core/AirportManager.h"

// Qt Widgets
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

AgentInterface::AgentInterface(AccountManager* accManager,
                               FlightManager* flManager,
                               BookingManager* bkManager,
                               ReportManager* reportManager,
                               AirportManager* airportManager,
                               QWidget *parent)
    : QWidget(parent),
      accountManager_(accManager),
      flightManager_(flManager),
      bookingManager_(bkManager),
      reportManager_(reportManager),
      airportManager_(airportManager)
{
    Q_ASSERT(accountManager_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(reportManager_ != nullptr);

    // Layout chính
    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // Sidebar
    sidebar_ = new QFrame(this);
    sidebar_->setFixedWidth(220);
    sidebar_->setStyleSheet("background-color: #133e87;");

    auto *sideLay = new QVBoxLayout(sidebar_);
    sideLay->setContentsMargins(16,20,16,16);
    sideLay->setSpacing(8);

    // Thông tin user
    auto *userIcon = new QLabel("👤", sidebar_);
    userIcon->setAlignment(Qt::AlignCenter);
    userIcon->setStyleSheet("font-size: 32px; color: white; background: transparent;");

    auto *userName = new QLabel("Xin chào, Đại lý", sidebar_);
    userName->setAlignment(Qt::AlignCenter);
    userName->setStyleSheet("color: white; font-weight: 600; background: transparent;");

    sideLay->addWidget(userIcon);
    sideLay->addWidget(userName);
    sideLay->addSpacing(15);

    // --- Menu Sidebar ---
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

    // Header NGHIỆP VỤ
    auto *header1 = new QLabel("NGHIỆP VỤ", sidebar_);
    header1->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent;");
    menuLayout->addWidget(header1);

    // Các nút nghiệp vụ
    btnDashboard_ = new QPushButton("Tổng quan");
    btnSearchBook_ = new QPushButton("Tìm & Đặt vé");
    btnBookings_ = new QPushButton("Quản lý Đặt chỗ");

    btnDashboard_->setStyleSheet(btnStyle);
    btnSearchBook_->setStyleSheet(btnStyle);
    btnBookings_->setStyleSheet(btnStyle);

    btnDashboard_->setCheckable(true);
    btnSearchBook_->setCheckable(true);
    btnBookings_->setCheckable(true);

    menuLayout->addWidget(btnDashboard_);
    menuLayout->addWidget(btnSearchBook_);
    menuLayout->addWidget(btnBookings_);

    // --- Thêm Thống kê ---
    auto *header3 = new QLabel("THỐNG KÊ", sidebar_);
    header3->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent;");
    menuLayout->addWidget(header3);

    btnReport_ = new QPushButton("Báo cáo tổng hợp");
    btnReport_->setStyleSheet(btnStyle);
    btnReport_->setCheckable(true);
    menuLayout->addWidget(btnReport_);

    // Header TÀI KHOẢN
    auto *header2 = new QLabel("TÀI KHOẢN", sidebar_);
    header2->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent;");
    menuLayout->addWidget(header2);

    btnAccounts_ = new QPushButton("Thông tin tài khoản");
    btnAccounts_->setStyleSheet(btnStyle);
    btnAccounts_->setCheckable(true);
    menuLayout->addWidget(btnAccounts_);

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
            padding: 6px;
        }
        QPushButton:hover {
            background-color: #6f99c6;
            color: white;
        }
    )");
    sideLay->addWidget(logoutBtn_);

    root->addWidget(sidebar_);

    // --- Stack ---
    stack_ = new QStackedWidget(this);
    
    // Create pages and store references for refreshing
    dashboardPage_ = new DashboardPage(accountManager_, reportManager_, this);
    searchBookPage_ = new SearchBookPage(flightManager_, bookingManager_, accountManager_, airportManager_, this);
    agentBookingsPage_ = new AgentBookingsPage(bookingManager_, flightManager_, accountManager_, airportManager_, this);
    
    stack_->addWidget(dashboardPage_); // 0
    stack_->addWidget(searchBookPage_); // 1
    stack_->addWidget(agentBookingsPage_); // 2
    stack_->addWidget(new AccountsPage(accountManager_, this)); // 3
    // stack_->addWidget(new AgentReportPage(accountManager_->getCurrentUser()->getId(), reportManager_, this)); //4


    root->addWidget(stack_,1);

    setupConnections();

    // Chọn mặc định
    stack_->setCurrentIndex(0);
    btnDashboard_->setChecked(true);
}

void AgentInterface::setupConnections()
{
    auto switchPage = [this](QPushButton* btn, int index){
        stack_->setCurrentIndex(index);
        btnDashboard_->setChecked(btn==btnDashboard_);
        btnSearchBook_->setChecked(btn==btnSearchBook_);
        btnBookings_->setChecked(btn==btnBookings_);
        btnAccounts_->setChecked(btn==btnAccounts_);
        btnReport_->setChecked(btn==btnReport_);
    };

    connect(btnDashboard_, &QPushButton::clicked, [=](){ switchPage(btnDashboard_,0); });
    connect(btnSearchBook_, &QPushButton::clicked, [=](){ switchPage(btnSearchBook_,1); });
    connect(btnBookings_, &QPushButton::clicked, [=](){ switchPage(btnBookings_,2); });
    connect(btnAccounts_, &QPushButton::clicked, [=](){ switchPage(btnAccounts_,3); });
    connect(btnReport_, &QPushButton::clicked, [=](){ switchPage(btnReport_,4); });

    connect(logoutBtn_, &QPushButton::clicked, this, &AgentInterface::logoutClicked);
    
    // Connect AccountManager signals to refresh pages when user changes
    connect(accountManager_, &AccountManager::userLoggedIn, this, &AgentInterface::onUserChanged);
    connect(accountManager_, &AccountManager::userLoggedOut, this, &AgentInterface::onUserChanged);
    
    // Connect stack currentChanged signal to refresh page when switching
    connect(stack_, &QStackedWidget::currentChanged, this, &AgentInterface::onPageChanged);
}

/**
 * @brief Handle user login/logout - refresh all pages
 */
void AgentInterface::onUserChanged() {
    // Refresh all pages when user changes
    if (dashboardPage_) {
        dashboardPage_->refreshPage();
    }
    if (agentBookingsPage_) {
        agentBookingsPage_->refreshPage();
    }
    if (searchBookPage_) {
        searchBookPage_->refreshPage();
    }
}

/**
 * @brief Handle page switching - refresh current page
 */
void AgentInterface::onPageChanged(int index) {
    // Get current page and refresh it
    QWidget* currentPage = stack_->widget(index);
    
    if (currentPage == dashboardPage_) {
        dashboardPage_->refreshPage();
    } else if (currentPage == agentBookingsPage_) {
        agentBookingsPage_->refreshPage();
    } else if (currentPage == searchBookPage_) {
        searchBookPage_->refreshPage();
    }
}

// #include "AgentInterface.h"

// #include "SearchBookPage.h"
// #include "AgentBookingsPage.h"
// #include "AccountsPage.h"
// #include "DashboardPage.h"
// #include "AgentReportPage.h" // <-- Thêm

// #include "core/AccountManager.h"
// #include "core/FlightManager.h"
// #include "core/BookingManager.h"
// #include "core/ReportManager.h"
// #include "core/AirportManager.h"

// #include <QStackedWidget>
// #include <QPushButton>
// #include <QLabel>
// #include <QFrame>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QDebug>

// AgentInterface::AgentInterface(AccountManager* accManager,
//                                FlightManager* flManager,
//                                BookingManager* bkManager,
//                                ReportManager* reportManager,
//                                AirportManager* airportManager,
//                                QWidget *parent)
//     : QWidget(parent),
//       accountManager_(accManager),
//       flightManager_(flManager),
//       bookingManager_(bkManager),
//       reportManager_(reportManager),
//       airportManager_(airportManager)
// {
//     auto *root = new QHBoxLayout(this);
//     root->setContentsMargins(0,0,0,0);
//     root->setSpacing(0);

//     sidebar_ = new QFrame(this);
//     sidebar_->setFixedWidth(220);
//     sidebar_->setStyleSheet("background-color: #2c3e50;");

//     auto *sideLay = new QVBoxLayout(sidebar_);
//     sideLay->setContentsMargins(16,20,16,16);
//     sideLay->setSpacing(8);

//     auto *userIcon = new QLabel("👤", sidebar_);
//     userIcon->setAlignment(Qt::AlignCenter);
//     userIcon->setStyleSheet("font-size: 32px; color: white; background: transparent;");
//     auto *userName = new QLabel("Xin chào, Đại lý", sidebar_);
//     userName->setAlignment(Qt::AlignCenter);
//     userName->setStyleSheet("color: white; font-weight: 600; background: transparent;");

//     sideLay->addWidget(userIcon);
//     sideLay->addWidget(userName);
//     sideLay->addSpacing(15);

//     auto *menuWidget = new QWidget(sidebar_);
//     auto *menuLayout = new QVBoxLayout(menuWidget);
//     menuLayout->setContentsMargins(0,0,0,0);
//     menuLayout->setSpacing(2);

//     QString btnStyle = R"(
//         QPushButton {
//             color: white;
//             background: transparent;
//             border: none;
//             text-align: left;
//             padding: 8px;
//         }
//         QPushButton:hover {
//             background-color: #6f99c6;
//             border-radius: 6px;
//         }
//         QPushButton:checked {
//             background-color: #6f99c6;
//         }
//     )";

//     auto *header1 = new QLabel("NGHIỆP VỤ", sidebar_);
//     header1->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent;");
//     menuLayout->addWidget(header1);

//     btnDashboard_ = new QPushButton("Tổng quan");
//     btnSearchBook_ = new QPushButton("Tìm & Đặt vé");
//     btnBookings_ = new QPushButton("Quản lý Đặt chỗ");
//     btnDashboard_->setStyleSheet(btnStyle); btnSearchBook_->setStyleSheet(btnStyle); btnBookings_->setStyleSheet(btnStyle);
//     btnDashboard_->setCheckable(true); btnSearchBook_->setCheckable(true); btnBookings_->setCheckable(true);

//     menuLayout->addWidget(btnDashboard_);
//     menuLayout->addWidget(btnSearchBook_);
//     menuLayout->addWidget(btnBookings_);

//     auto *header2 = new QLabel("TÀI KHOẢN", sidebar_);
//     header2->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent;");
//     menuLayout->addWidget(header2);

//     btnAccounts_ = new QPushButton("Thông tin tài khoản");
//     btnAccounts_->setStyleSheet(btnStyle);
//     btnAccounts_->setCheckable(true);
//     menuLayout->addWidget(btnAccounts_);

//     // --- Thêm Thống kê ---
//     auto *header3 = new QLabel("THỐNG KÊ", sidebar_);
//     header3->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent;");
//     menuLayout->addWidget(header3);

//     btnReport_ = new QPushButton("Báo cáo tổng hợp");
//     btnReport_->setStyleSheet(btnStyle);
//     btnReport_->setCheckable(true);
//     menuLayout->addWidget(btnReport_);

//     menuLayout->addStretch();
//     menuWidget->setLayout(menuLayout);
//     sideLay->addWidget(menuWidget);

//     logoutBtn_ = new QPushButton("Đăng xuất", sidebar_);
//     logoutBtn_->setStyleSheet(R"(
//         QPushButton {
//             color: white;
//             background: transparent;
//             border: 1px solid #6f99c6;
//             border-radius: 6px;
//             padding: 6px;
//         }
//         QPushButton:hover {
//             background-color: #6f99c6;
//             color: white;
//         }
//     )");
//     sideLay->addWidget(logoutBtn_);
//     root->addWidget(sidebar_);

//     // Stack
//     stack_ = new QStackedWidget(this);
//     stack_->addWidget(new DashboardPage(accountManager_, reportManager_, this)); //0
//     stack_->addWidget(new SearchBookPage(flightManager_, bookingManager_, accountManager_, airportManager_, this)); //1
//     stack_->addWidget(new AgentBookingsPage(bookingManager_, flightManager_, accountManager_, airportManager_, this)); //2
//     stack_->addWidget(new AccountsPage(accountManager_, this)); //3
//     stack_->addWidget(new AgentReportPage(accountManager_->getCurrentUser()->getId(), reportManager_, this)); //4

//     root->addWidget(stack_,1);

//     setupConnections();
//     stack_->setCurrentIndex(0);
//     btnDashboard_->setChecked(true);
// }

// void AgentInterface::setupConnections()
// {
//     auto switchPage = [this](QPushButton* btn, int index){
//         stack_->setCurrentIndex(index);
//         btnDashboard_->setChecked(btn==btnDashboard_);
//         btnSearchBook_->setChecked(btn==btnSearchBook_);
//         btnBookings_->setChecked(btn==btnBookings_);
//         btnAccounts_->setChecked(btn==btnAccounts_);
//         btnReport_->setChecked(btn==btnReport_);
//     };

//     connect(btnDashboard_, &QPushButton::clicked, [=](){ switchPage(btnDashboard_,0); });
//     connect(btnSearchBook_, &QPushButton::clicked, [=](){ switchPage(btnSearchBook_,1); });
//     connect(btnBookings_, &QPushButton::clicked, [=](){ switchPage(btnBookings_,2); });
//     connect(btnAccounts_, &QPushButton::clicked, [=](){ switchPage(btnAccounts_,3); });
//     connect(btnReport_, &QPushButton::clicked, [=](){ switchPage(btnReport_,4); });

//     connect(logoutBtn_, &QPushButton::clicked, this, &AgentInterface::logoutClicked);
// }
