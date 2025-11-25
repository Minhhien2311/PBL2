#pragma once

#include <string>
#include "Account.h"

// Tài khoản đại lý (Agent), kế thừa từ Account
class AccountAgent : public Account {
private:
    double totalSales;  // Tổng doanh số
    bool isActive;      // Trạng thái hoạt động

public:
    // --- Constructor ---
    AccountAgent(const std::string& username, const std::string& passwordPlain,
                 const std::string& fullName, const std::string& phone,
                 const std::string& email, double totalSales = 0.0, bool active = true);

    // --- Getters ---
    double getTotalSales() const;
    bool getIsActive() const;

    // --- Setters ---
    void updateTotalSales(double amount);
    void setActive(bool status);

    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static AccountAgent fromRecordLine(const std::string& line);
};