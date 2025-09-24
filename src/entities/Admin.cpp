#include "Admin.h"
#include "../utils/DatabaseManager.h"
#include "../utils/Helpers.h"
#include "../entities/ActivityLog.h"
#include "../entities/Agent.h"
#include <iostream>
#include <iomanip>

// Constructor implementation
Admin::Admin(const std::string& id, const std::string& name, const std::string& dob, 
             const std::string& email, const std::string& phone, const std::string& addr,
             const std::string& user, const std::string& pwdHash, 
             const std::string& regDate)
    : Account(id, name, dob, email, phone, addr, user, pwdHash, regDate, "ADMIN") {
    // Constructor specific to Admin can be expanded here if needed
}

// Manage flights - Add, edit, delete flights
void Admin::manageFlights() {
    std::cout << "\n=== QUẢN LÝ CHUYẾN BAY ===" << std::endl;
    std::cout << "1. Thêm chuyến bay mới" << std::endl;
    std::cout << "2. Sửa thông tin chuyến bay" << std::endl;
    std::cout << "3. Xóa chuyến bay" << std::endl;
    std::cout << "4. Xem danh sách chuyến bay" << std::endl;
    std::cout << "5. Quản lý lịch bay" << std::endl;
    std::cout << "0. Quay lại" << std::endl;
    
    int choice;
    std::cout << "Chọn chức năng: ";
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            addNewFlight();
            break;
        case 2:
            editFlight();
            break;
        case 3:
            deleteFlight();
            break;
        case 4:
            viewAllFlights();
            break;
        case 5:
            manageFlightInstances();
            break;
        case 0:
            return;
        default:
            std::cout << "Lựa chọn không hợp lệ!" << std::endl;
    }
}

// Manage agents - Add, edit, delete agent accounts
void Admin::manageAgents() {
    std::cout << "\n=== QUẢN LÝ ĐẠI LÝ ===" << std::endl;
    std::cout << "1. Thêm đại lý mới" << std::endl;
    std::cout << "2. Sửa thông tin đại lý" << std::endl;
    std::cout << "3. Xóa đại lý" << std::endl;
    std::cout << "4. Xem danh sách đại lý" << std::endl;
    std::cout << "5. Xem báo cáo đại lý" << std::endl;
    std::cout << "0. Quay lại" << std::endl;
    
    int choice;
    std::cout << "Chọn chức năng: ";
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            addNewAgent();
            break;
        case 2:
            editAgent();
            break;
        case 3:
            deleteAgent();
            break;
        case 4:
            viewAllAgents();
            break;
        case 5:
            viewAgentReports();
            break;
        case 0:
            return;
        default:
            std::cout << "Lựa chọn không hợp lệ!" << std::endl;
    }
}

// Generate various reports
void Admin::generateReports() {
    std::cout << "\n=== BÁO CÁO VÀ THỐNG KÊ ===" << std::endl;
    std::cout << "1. Báo cáo doanh thu theo ngày" << std::endl;
    std::cout << "2. Báo cáo doanh thu theo tuần" << std::endl;
    std::cout << "3. Báo cáo doanh thu theo tháng" << std::endl;
    std::cout << "4. Thống kê số lượng vé bán" << std::endl;
    std::cout << "5. Thống kê theo đại lý" << std::endl;
    std::cout << "6. Xem nhật ký hoạt động" << std::endl;
    std::cout << "0. Quay lại" << std::endl;
    
    int choice;
    std::cout << "Chọn chức năng: ";
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            generateDailyRevenueReport();
            break;
        case 2:
            generateWeeklyRevenueReport();
            break;
        case 3:
            generateMonthlyRevenueReport();
            break;
        case 4:
            generateTicketSalesReport();
            break;
        case 5:
            generateAgentPerformanceReport();
            break;
        case 6:
            viewActivityLogs();
            break;
        case 0:
            return;
        default:
            std::cout << "Lựa chọn không hợp lệ!" << std::endl;
    }
}

// Configure system settings
void Admin::configureSystemSettings() {
    std::cout << "\n=== CẤU HÌNH HỆ THỐNG ===" << std::endl;
    std::cout << "1. Quản lý khuyến mãi" << std::endl;
    std::cout << "2. Thiết lập quy định chuyến bay" << std::endl;
    std::cout << "3. Cấu hình giá vé" << std::endl;
    std::cout << "4. Backup dữ liệu" << std::endl;
    std::cout << "5. Restore dữ liệu" << std::endl;
    std::cout << "0. Quay lại" << std::endl;
    
    int choice;
    std::cout << "Chọn chức năng: ";
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            managePromotions();
            break;
        case 2:
            configureFlightRules();
            break;
        case 3:
            configurePricing();
            break;
        case 4:
            backupData();
            break;
        case 5:
            restoreData();
            break;
        case 0:
            return;
        default:
            std::cout << "Lựa chọn không hợp lệ!" << std::endl;
    }
}

// Display admin information
void Admin::displayInfo() const {
    Account::displayInfo(); // Call base class display
    std::cout << "=== THÔNG TIN QUẢN TRỊ ===" << std::endl;
    std::cout << "Vai trò: Quản trị viên hệ thống" << std::endl;
}

// ===== PRIVATE METHOD IMPLEMENTATIONS =====

void Admin::addNewFlight() {
    std::cout << "\n--- THÊM CHUYẾN BAY MỚI ---" << std::endl;
    
    std::string flightNumber, airline, aircraftType, departure, arrival;
    double basePrice;
    int totalSeats, businessSeats, economySeats;
    
    std::cin.ignore(); // Clear buffer
    std::cout << "Số hiệu chuyến bay: ";
    std::getline(std::cin, flightNumber);
    
    std::cout << "Hãng hàng không: ";
    std::getline(std::cin, airline);
    
    std::cout << "Loại máy bay: ";
    std::getline(std::cin, aircraftType);
    
    std::cout << "Sân bay đi: ";
    std::getline(std::cin, departure);
    
    std::cout << "Sân bay đến: ";
    std::getline(std::cin, arrival);
    
    std::cout << "Giá vé cơ bản: ";
    std::cin >> basePrice;
    
    std::cout << "Tổng số ghế: ";
    std::cin >> totalSeats;
    
    std::cout << "Số ghế hạng thương gia: ";
    std::cin >> businessSeats;
    
    std::cout << "Số ghế hạng phổ thông: ";
    std::cin >> economySeats;
    
    // Create new flight
    std::string flightId = std::to_string(DatabaseManager::getNextId("flights.txt"));
    Flight newFlight(flightId, flightNumber, airline, aircraftType, departure, 
                    arrival, basePrice, totalSeats, businessSeats, economySeats);
    
    if (DatabaseManager::saveFlight(newFlight)) {
        std::cout << "Thêm chuyến bay thành công!" << std::endl;
        ActivityLog::logFlightManagement(getId(), "ADD_FLIGHT", flightId);
    } else {
        std::cout << "Lỗi khi thêm chuyến bay!" << std::endl;
    }
}

void Admin::viewAllFlights() {
    std::cout << "\n--- DANH SÁCH CHUYẾN BAY ---" << std::endl;
    
    auto flights = DatabaseManager::loadAllFlights();
    if (flights.empty()) {
        std::cout << "Không có chuyến bay nào!" << std::endl;
        return;
    }
    
    std::cout << "==========================================================================================================" << std::endl;
    std::cout << "| ID | Số hiệu | Hãng bay    | Máy bay | Đi -> Đến     | Giá cơ bản | Tổng ghế | TG | PT |" << std::endl;
    std::cout << "==========================================================================================================" << std::endl;
    
    for (const auto& flight : flights) {
        printf("| %-2s | %-7s | %-11s | %-7s | %-13s | %10.0f | %8d | %2d | %2d |\n",
               flight.getFlightId().c_str(),
               flight.getFlightNumber().c_str(),
               flight.getAirline().c_str(),
               flight.getAircraftType().c_str(),
               (flight.getDepartureAirport() + "->" + flight.getArrivalAirport()).c_str(),
               flight.getBasePrice(),
               flight.getTotalSeats(),
               flight.getBusinessSeats(),
               flight.getEconomySeats());
    }
    std::cout << "==========================================================================================================" << std::endl;
}

void Admin::addNewAgent() {
    std::cout << "\n--- THÊM ĐẠI LÝ MỚI ---" << std::endl;
    
    std::string username, password, fullName, email, phone, address;
    
    std::cin.ignore();
    std::cout << "Tên đăng nhập: ";
    std::getline(std::cin, username);
    
    std::cout << "Mật khẩu: ";
    std::getline(std::cin, password);
    
    std::cout << "Họ tên: ";
    std::getline(std::cin, fullName);
    
    std::cout << "Email: ";
    std::getline(std::cin, email);
    
    std::cout << "Số điện thoại: ";
    std::getline(std::cin, phone);
    
    std::cout << "Địa chỉ: ";
    std::getline(std::cin, address);
    
    // Create new agent
    std::string agentId = std::to_string(DatabaseManager::getNextId("users.txt"));
    std::string hashedPassword = Helpers::hashPassword(password);
    std::string regDate = Helpers::getCurrentDateTime();
    
    Agent newAgent(agentId, fullName, "1990-01-01", email, phone, address,
                  username, hashedPassword, regDate, 0.05, 0.0, true);
    
    if (DatabaseManager::saveUser(newAgent)) {
        std::cout << "Thêm đại lý thành công!" << std::endl;
        ActivityLog::logFlightManagement(getId(), "ADD_AGENT", agentId);
    } else {
        std::cout << "Lỗi khi thêm đại lý!" << std::endl;
    }
}

void Admin::viewAllAgents() {
    std::cout << "\n--- DANH SÁCH ĐẠI LÝ ---" << std::endl;
    
    auto users = DatabaseManager::loadAllUsers();
    if (users.empty()) {
        std::cout << "Không có đại lý nào!" << std::endl;
        return;
    }
    
    std::cout << "====================================================================" << std::endl;
    std::cout << "| ID | Tên đăng nhập | Họ tên       | Email           | Trạng thái |" << std::endl;
    std::cout << "====================================================================" << std::endl;
    
    for (const auto& user : users) {
        if (user.getRole() == "AGENT") {
            printf("| %-2s | %-13s | %-12s | %-15s | %-10s |\n",
                   user.getId().c_str(),
                   user.getUsername().c_str(),
                   user.getFullName().c_str(),
                   user.getEmail().c_str(),
                   "ACTIVE");
        }
    }
    std::cout << "====================================================================" << std::endl;
}

void Admin::generateDailyRevenueReport() {
    std::string date;
    std::cout << "\nNhập ngày (YYYY-MM-DD): ";
    std::cin >> date;
    
    std::cout << "\n--- BÁO CÁO DOANH THU NGÀY " << date << " ---" << std::endl;
    
    // Implementation for daily revenue report
    // This would typically query the database and calculate revenue
    std::cout << "Doanh thu: 15,000,000 VND" << std::endl;
    std::cout << "Số vé bán: 10" << std::endl;
    std::cout << "Vé đã hủy: 2" << std::endl;
    
    ActivityLog::logReportGeneration(getId(), "DAILY_REVENUE_" + date);
}

void Admin::viewActivityLogs() {
    std::cout << "\n--- NHẬT KÝ HOẠT ĐỘNG ---" << std::endl;
    
    auto logs = ActivityLog::getRecentActivities(20); // Get last 20 activities
    ActivityLog::displayLogsTable(logs);
}

void Admin::managePromotions() {
    std::cout << "\n--- QUẢN LÝ KHUYẾN MÃI ---" << std::endl;
    // Implementation for promotion management
    std::cout << "Chức năng đang được phát triển..." << std::endl;
}

void Admin::configureFlightRules() {
    std::cout << "\n--- THIẾT LẬP QUY ĐỊNH ---" << std::endl;
    // Implementation for flight rules configuration
    std::cout << "Chức năng đang được phát triển..." << std::endl;
}

// Stub implementations for other methods
// void Admin::editFlight() {
//     std::cout << "Chức năng sửa chuyến bay đang được phát triển..." << std::endl;
// }

void Admin::deleteFlight() {
    std::cout << "Chức năng xóa chuyến bay đang được phát triển..." << std::endl;
}

void Admin::manageFlightInstances() {
    std::cout << "Chức năng quản lý lịch bay đang được phát triển..." << std::endl;
}

void Admin::editAgent() {
    std::cout << "Chức năng sửa đại lý đang được phát triển..." << std::endl;
}

void Admin::deleteAgent() {
    std::cout << "Chức năng xóa đại lý đang được phát triển..." << std::endl;
}

void Admin::viewAgentReports() {
    std::cout << "Chức năng xem báo cáo đại lý đang được phát triển..." << std::endl;
}

void Admin::generateWeeklyRevenueReport() {
    std::cout << "Chức năng báo cáo tuần đang được phát triển..." << std::endl;
}

void Admin::generateMonthlyRevenueReport() {
    std::cout << "Chức năng báo cáo tháng đang được phát triển..." << std::endl;
}

void Admin::generateTicketSalesReport() {
    std::cout << "Chức năng thống kê vé bán đang được phát triển..." << std::endl;
}

void Admin::generateAgentPerformanceReport() {
    std::cout << "Chức năng thống kê đại lý đang được phát triển..." << std::endl;
}

void Admin::configurePricing() {
    std::cout << "Chức năng cấu hình giá đang được phát triển..." << std::endl;
}

void Admin::backupData() {
    std::cout << "Chức năng backup đang được phát triển..." << std::endl;
}

void Admin::restoreData() {
    std::cout << "Chức năng restore đang được phát triển..." << std::endl;
}