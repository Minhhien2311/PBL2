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
private:
    // Thuộc tính riêng: danh sách các agent mà admin này quản lý
    std::vector<AccountAgent> managedAgents;
public:
    // Constructor: khởi tạo account admin, không cần truyền ID, sẽ tự động sinh ra
    AccountAdmin(const std::string& username,
                 const std::string& passwordPlain,
                 const std::string& fullName,
                 const std::string& phone,
                 const std::string& email);

    // --- Chức năng quản lý Agent ---
    void addAgent(const AccountAgent& agent);
    bool removeAgent(const std::string& agentId);

    // --- Getter cho danh sách Agent ---
    // Trả về một tham chiếu hằng, cho phép xem nhưng không cho sửa đổi từ bên ngoài
    const std::vector<AccountAgent>& getManagedAgents() const;

    // --- Triển khai hàm ảo từ lớp cha ---
    // Thêm 'override' để trình biên dịch kiểm tra tính chính xác
    void displayInfo() const override;
};

#endif
