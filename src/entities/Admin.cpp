#include "C:/PBL2/include/entities/Admin.h"
#include <iostream>

// Constructor: chuyển phần chung cho Person, giữ Account theo composition
Admin::Admin(const std::string& id,
             const std::string& name,
             const std::string& dob,
             Gender gender,
             const std::string& phone,
             const Account& acc)
    : Person(id, name, dob, gender, phone), account(acc)
{}

// Getter cho Account
const Account& Admin::getAccount() const { return account; }    // Read-only
Account&       Admin::getAccount()       { return account; }    // Full-access

// Helper nội bộ: chuyển Role -> string để hiển thị
static const char* roleToString(Role r) {
    switch (r) {
        case Role::Admin: return "Admin";
        case Role::Agent: return "Agent";
        default:          return "Unknown";
    }
}

// Hiển thị thông tin: dùng Person::displayInfo rồi in thêm Account
void Admin::displayInfo() const {
    Person::displayInfo();
    std::cout << "Account ID: " << account.getId()       << "\n"
              << "Username:   " << account.getUsername() << "\n"
              << "Role:       " << roleToString(account.getRole()) << "\n";
}

