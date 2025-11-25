#pragma once

#include <string>
#include "Account.h"

// Tài khoản Admin, kế thừa từ Account
class AccountAdmin : public Account {
public:
    // --- Constructor ---
    AccountAdmin() = default;
    AccountAdmin(const std::string& username, const std::string& passwordPlain,
                 const std::string& fullName, const std::string& phone,
                 const std::string& email);

    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static AccountAdmin fromRecordLine(const std::string& line);
};