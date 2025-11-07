#include "AdminInterface.h"

#include "DashboardPage.h"
#include "AccountsPage.h"
#include "RoutesPage.h"
#include "FlightsPage.h"

#include "core/AccountManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "core/AirportManager.h"

// Include Qt cần thiết
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

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
    // Layout chính: sidebar + stack
    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Sidebar
    sidebar_ = new QFrame(this);
    sidebar_->setFixedWidth(220);
    sidebar_->setStyleSheet("background-color: #2c3e50;");

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

    // --- Sidebar menu ---
    auto *menuWidget = new QWidget(sidebar_);
    auto *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(0,0,0,0);
    menuLayout->setSpacing(2);

    // Header QUẢN LÝ (không hover, không click)
    auto *header1 = new QLabel("QUẢN LÝ", sidebar_);
    header1->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent;");
    menuLayout->addWidget(header1);

    // Các nút QUẢN LÝ
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

    btnDashboard_ = new QPushButton("Trang tổng quan");
    btnRoutes_    = new QPushButton("Quản lý tuyến bay");
    btnFlights_   = new QPushButton("Quản lý chuyến bay");

    btnDashboard_->setStyleSheet(btnStyle);
    btnRoutes_->setStyleSheet(btnStyle);
    btnFlights_->setStyleSheet(btnStyle);

    btnDashboard_->setCheckable(true);
    btnRoutes_->setCheckable(true);
    btnFlights_->setCheckable(true);

    menuLayout->addWidget(btnDashboard_);
    menuLayout->addWidget(btnRoutes_);
    menuLayout->addWidget(btnFlights_);

    // Header TÀI KHOẢN (không hover, không click)
    auto *header2 = new QLabel("TÀI KHOẢN", sidebar_);
    header2->setStyleSheet("color: #6f99c6; font-weight: bold; background: transparent;");
    menuLayout->addWidget(header2);

    btnAccounts_ = new QPushButton("Quản lý tài khoản");
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

    // Stack chứa các trang
    stack_ = new QStackedWidget(this);
    stack_->addWidget(new DashboardPage(accountManager_, reportManager_, this)); // 0
    stack_->addWidget(new RoutesPage(flightManager_, airportManager_, this));   // 1
    stack_->addWidget(new FlightsPage(flightManager_, airportManager_, this));  // 2
    stack_->addWidget(new AccountsPage(accountManager_, this));                 // 3

    root->addWidget(stack_, 1);

    setupConnections();

    // Chọn mặc định
    stack_->setCurrentIndex(0);
    btnDashboard_->setChecked(true);
}

void AdminInterface::setupConnections()
{
    auto switchPage = [this](QPushButton* btn, int index){
        stack_->setCurrentIndex(index);
        btnDashboard_->setChecked(btn == btnDashboard_);
        btnRoutes_->setChecked(btn == btnRoutes_);
        btnFlights_->setChecked(btn == btnFlights_);
        btnAccounts_->setChecked(btn == btnAccounts_);
    };

    connect(btnDashboard_, &QPushButton::clicked, [=](){ switchPage(btnDashboard_,0); });
    connect(btnRoutes_,    &QPushButton::clicked, [=](){ switchPage(btnRoutes_,1); });
    connect(btnFlights_,   &QPushButton::clicked, [=](){ switchPage(btnFlights_,2); });
    connect(btnAccounts_,  &QPushButton::clicked, [=](){ switchPage(btnAccounts_,3); });

    connect(logoutBtn_, &QPushButton::clicked, this, &AdminInterface::logoutClicked);
}
