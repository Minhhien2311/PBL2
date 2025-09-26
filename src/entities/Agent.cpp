#include "C:/PBL2/include/entities/Agent.h"
#include <iostream>

// Constructor: gọi Person trước, rồi copy Account + khởi tạo thông tin kinh doanh
Agent::Agent(const std::string& id,
             const std::string& name,
             const std::string& dob,
             Gender gender,
             const std::string& phone,
             const Account& acc,
             double totalSales,
             bool active)
    : Person(id, name, dob, gender, phone),
      account(acc),          // copy Account
      totalSales(totalSales),
      isActive(active) {}

// Getter cho Account
const Account& Agent::getAccount() const { return account; }
Account&       Agent::getAccount()       { return account; }

// Getter cho business fields
double Agent::getTotalSales() const { return totalSales; }
bool   Agent::getIsActive() const   { return isActive; }

// Setter / updater
void Agent::updateTotalSales(double amount) {
    // Cộng thêm doanh số (gọi từ Booking/Ticket Service khi xuất vé)
    totalSales += amount;
}

void Agent::setActive(bool status) {
    isActive = status;
}

// Helper: Role -> string (tái dùng từ Admin)
static const char* roleToString(Role r) {
    switch (r) {
        case Role::Admin: return "Admin";
        case Role::Agent: return "Agent";
        default:          return "Unknown";
    }
}

// In thông tin Agent
void Agent::displayInfo() const {
    Person::displayInfo();
    std::cout << "Account ID: " << account.getId()       << "\n"
              << "Username:   " << account.getUsername() << "\n"
              << "Role:       " << roleToString(account.getRole()) << "\n"
              << "TotalSales: " << totalSales            << "\n"
              << "Active:     " << (isActive ? "Yes" : "No") << "\n";
}
