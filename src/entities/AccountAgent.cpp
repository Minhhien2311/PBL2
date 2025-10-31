#include "entities/AccountAgent.h"
#include "utils/GenID.h"
#include <iostream>

// Constructor: gọi base Account với role = Agent
AccountAgent::AccountAgent(const std::string& username,
                           const std::string& passwordPlain,
                           const std::string& fullName,
                           const std::string& phone,
                           const std::string& email,
                           double totalSales,
                           bool active)
    : Account(IdGenerator::generateAgentId(), username, passwordPlain, Role::Agent, fullName, phone, email),
      totalSales(totalSales),
      isActive(active) {}

// --- Getter ---
double AccountAgent::getTotalSales() const { return totalSales; }
bool   AccountAgent::getIsActive()  const { return isActive;  }

// --- Setter ---
void AccountAgent::updateTotalSales(double amount) { if (amount > 0) totalSales += amount; }
void AccountAgent::setActive(bool status)          { isActive = status; }

// Chuyển đổi đối tượng AccountAgent thành một dòng string để lưu vào file.
std::string AccountAgent::toRecordLine() const {
    // Chuyển đổi enum Role và bool isActive thành số nguyên để lưu file
    std::string roleStr = std::to_string(static_cast<int>(this->role));
    std::string activeStr = this->isActive ? "1" : "0";

    // Nối các thuộc tính của lớp cha và các thuộc tính riêng của Agent
    return this->accountId + "|" +
           this->username + "|" +
           this->passwordHash + "|" +
           roleStr + "|" +
           this->fullName + "|" +
           this->phoneNumber + "|" +
           this->email + "|" +
           std::to_string(this->totalSales) + "|" + // Thuộc tính riêng
           activeStr;                             // Thuộc tính riêng
    }

    // Tạo đối tượng AccountAgent từ một dòng string đọc từ file.
    AccountAgent AccountAgent::fromRecordLine(const std::string& line) {
    size_t start = 0;
    size_t end = line.find('|');
    
    // Tách các trường dữ liệu từ chuỗi
    std::string id = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string username = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string passwordHash = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    // Bỏ qua role vì chúng ta biết đây là Agent
    start = end + 1;
    end = line.find('|', start);

    std::string fullName = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string phone = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string email = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    // Tách các thuộc tính riêng của Agent
    double totalSales = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.length();

    bool isActive = (line.substr(start, end - start) == "1");

    // Sử dụng kỹ thuật "tạo tạm rồi sửa lỗi" tương tự như AccountAdmin
    // 1. Tạo một đối tượng Agent tạm thời
    AccountAgent agent(username, "temp_password", fullName, phone, email, totalSales, isActive);

    // 2. Ghi đè lại ID và passwordHash bằng dữ liệu đã đọc từ file
    agent.overrideIdForLoad(id);
    agent.overridePasswordHashForLoad(passwordHash);

    return agent;
    }

/// --- Triển khai hàm ảo ---
// SỬA LỖI Ở ĐÂY: Bổ sung nội dung cho hàm displayInfo
void AccountAgent::displayInfo() const {
    std::cout << "===== AGENT ACCOUNT INFO =====" << std::endl;
    std::cout << "ID: \t\t" << this->getId() << std::endl;
    std::cout << "Username: \t" << this->getUsername() << std::endl;
    std::cout << "Full Name: \t" << this->getFullName() << std::endl;
    std::cout << "Role: \t\tAgent" << std::endl;
    std::cout << "Total Sales: \t" << this->getTotalSales() << std::endl;
    std::cout << "Status: \t" << (this->getIsActive() ? "Active" : "Inactive") << std::endl;
    std::cout << "============================" << std::endl;
}
