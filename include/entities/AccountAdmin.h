#ifndef ACCOUNT_ADMIN_H
#define ACCOUNT_ADMIN_H

#include <string>
#include <vector>
#include "Account.h"
#include "AccountAgent.h"

/*
 * AccountAdmin: tài khoản dành riêng cho Admin
 *  - Kế thừa từ Account (role = Role::Admin)
 *  - Có thể mở rộng thêm thuộc tính dành riêng cho admin (nếu cần)
 */
class AccountAdmin : public Account {
public:
    // Constructor: khởi tạo account admin
    AccountAdmin(const std::string& username,
                 const std::string& passwordPlain,
                 const std::string& fullName,
                 const std::string& phone,
                 const std::string& email);

    // Quản lý agent
    void addAgent(const AccountAgent& agent);
    void removeAgent(const std::string& agentId);

    // Xem danh sách agent
    const std::vector<AccountAgent>& getAgents() const;

    // Hiển thị thông tin admin
    void displayInfo() const;
};

#endif // ACCOUNT_ADMIN_H
