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
    // Xóa dòng resize
    // resize(1100, 700);  // <-- XÓA DÒNG NÀY

    setupUi();
    setupConnections();

    // --- QSS giao diện ---
        QString qss = R"(
            /* === STYLE CƠ BẢN === */
            QWidget { background: #FFFFFF; color: #333; }
            QMainWindow { background: #FFFFFF; }
            QFrame#Sidebar { background:#0E3B7C; }
            QLabel#AppTitle { color:#123B7A; font-weight:700; font-size:24px; }
            QFrame#LoginBox { border: 1px solid #C9D6EB; border-radius: 8px; background: white; color: #333; }
            
            /* === STYLE BUTTON, LINEEDIT (Lấy từ global) === */
            QPushButton { 
                border:1px solid #7AA0D4; 
                border-radius:8px; 
                padding:8px 14px; 
                background:#5B86C6; 
                color:white; 
                font-weight:600; 
            }
            QPushButton:hover { background:#6B97D8; }
            
            QLineEdit, QComboBox, QDateEdit, QTimeEdit, QSpinBox { 
                border:1px solid #C9D6EB; 
                border-radius:8px; 
                padding:6px 10px; 
                background:white; 
                color: #333; 
                height: 30px; /* Thêm chiều cao chuẩn */
            }
            QLineEdit::placeholder { color: #999; }

            /* === STYLE LIST/TABLE (GOM LẠI) === */
            QListWidget { border:none; color:white; background:transparent; }
            QListWidget::item { padding:8px 10px; }
            QListWidget::item:selected { background:#1C4E99; border-radius:6px; }
            
            QTableView {
                border: 1px solid #C9D6EB;
                gridline-color: transparent; /* cố gắng ẩn grid mặc định */
                background: white;
                outline: 0; /* <--- QUAN TRỌNG: Tắt viền nét đứt/xanh khi click vào ô */
            }

            /* Vẽ đường ngang dưới mỗi ô; không vẽ đường dọc */
            QTableView::item {
                border-bottom: 1px solid #C9D6EB;
                border-right: none;
            }

            /* Header có đường phân cách */
            QHeaderView::section {
                border-bottom: 1px solid #C9D6EB;
            }

            QHeaderView::section { 
                background-color: #EBF1F9; 
                padding: 8px; 
                border: none; 
                font-weight: 600; 
                color: #123B7A; 
            }
            
            /* === THÊM: STYLE HOVER/SELECTED === */
            QTableView::item:hover,
            QListView::item:hover {
                background-color: #EAF2F8;
                color: #123B7A;
            }
            
            /* ĐÃ SỬA: In đậm chữ và xóa border để nhìn liền mạch */
            QTableView::item:selected,
            QListView::item:selected {
                background-color: #EAF2F8;
                color: black;
            }
            
            /* * ========================================
            * STYLE SCROLLBAR (THANH DỌC) - GIỮ NGUYÊN
            * ======================================== */
            QScrollBar:vertical {
                border: none;
                background: #cbdceb; 
                width: 6px;          
                margin: 0;
            }
            
            QScrollBar::handle:vertical {
                background: #608bc1; 
                border-radius: 3px;  
                min-height: 20px;    
            }
            
            QScrollBar::handle:vertical:hover {
                background: #466a9a; 
            }
            
            /* === NÚT MŨI TÊN (DỌC) === */
            QScrollBar::sub-line:vertical {
                background: none; 
                height: 0;      
                subcontrol-origin: margin;
                subcontrol-position: top;
            }
            
            QScrollBar::add-line:vertical {
                background: none; 
                height: 0;      
                subcontrol-origin: margin;
                subcontrol-position: bottom;
            }

            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
                background: none;
            }


            /* * ========================================
            * STYLE SCROLLBAR (THANH NGANG) - GIỮ NGUYÊN
            * ======================================== */
            QScrollBar:horizontal {
                border: none;
                background: #cbdceb; 
                height: 6px;         
                margin: 0;
            }
            
            QScrollBar::handle:horizontal {
                background: #608bc1; 
                border-radius: 3px;  
                min-width: 20px;     
            }
            
            QScrollBar::handle:horizontal:hover {
                background: #466a9a; 
            }
            
            /* === NÚT MŨI TÊN (NGANG) === */
            QScrollBar::sub-line:horizontal {
                background: none; 
                width: 0;       
                subcontrol-origin: margin;
                subcontrol-position: left;
            }
            
            QScrollBar::add-line:horizontal {
                background: none; 
                width: 0;       
                subcontrol-origin: margin;
                subcontrol-position: right;
            }

            QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
                background: none;
            }
            )";
        this->setStyleSheet(qss);
    
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
                                         airportManager_, passengerManager_, this);

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
