#include "AccountAgent.h"
#include <iostream>

// Constructor: gọi base Account với role = Agent
AccountAgent::AccountAgent(const std::string& id,
                           const std::string& username,
                           const std::string& passwordPlain,
                           const std::string& fullName,
                           const std::string& phone,
                           const std::string& email,
                           double totalSales,
                           bool active)
    : Account(id, username, passwordPlain, Role::Agent),
      totalSales(totalSales),
      isActive(active)
{
    // Lưu ý: nếu Account không lưu fullName/phone/email thì các tham số này chỉ để tương thích chữ ký.
}

// Getter
double AccountAgent::getTotalSales() const { return totalSales; }
bool   AccountAgent::getIsActive()  const { return isActive;  }

// Setter
void AccountAgent::updateTotalSales(double amount) { totalSales += amount; }
void AccountAgent::setActive(bool status)          { isActive = status;   }

// Hiển thị thông tin cơ bản của agent
void AccountAgent::displayInfo() const {
    std::cout << "[Agent] id=" << getId()
              << " username=" << getUsername()
              << " totalSales=" << totalSales
              << " active=" << (isActive ? "true" : "false")
              << "\n";
}
