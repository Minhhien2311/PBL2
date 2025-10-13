#include "AccountAgent.h"
#include "C:/PBL2/include/utils/GenID.h"
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
