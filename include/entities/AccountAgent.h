#ifndef ACCOUNT_AGENT_H
#define ACCOUNT_AGENT_H

#include <string>
#include "Account.h"

/*
 * AccountAgent: tài khoản dành riêng cho Agent
 *  - Kế thừa từ Account (role = Role::Agent)
 *  - Có thêm thông tin kinh doanh: tổng doanh số, trạng thái hoạt động
 */
class AccountAgent : public Account {
private:
    double totalSales; // tổng số tiền bán vé
    bool isActive;     // đại lý còn hoạt động hay đã bị khóa

public:
    // Constructor: khởi tạo account agent
    AccountAgent(const std::string& username,
                 const std::string& passwordPlain,
                 const std::string& fullName,
                 const std::string& phone,
                 const std::string& email,
                 double totalSales = 0.0,
                 bool active = true);

    // Getter
    double getTotalSales() const;
    bool   getIsActive() const;

    // Setter
    void updateTotalSales(double amount); // cộng thêm vào doanh số
    void setActive(bool status);

    // Hiển thị thông tin agent (có thể override hàm riêng nếu cần)
    void displayInfo() const;
};

#endif // ACCOUNT_AGENT_H
