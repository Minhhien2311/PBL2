#ifndef ACCOUNT_ADMIN_H
#define ACCOUNT_ADMIN_H

#include <string>
#include <vector>
#include "Account.h"
#include "AccountAgent.h"
#include "C:/PBL2/include/DSA/DynamicArray.h"

/*
 * AccountAdmin: tài khoản dành riêng cho Admin
 *  - Kế thừa từ Account (role = Role::Admin)
 *  - Có thể mở rộng thêm thuộc tính dành riêng cho admin (nếu cần)
 */
class AccountAdmin : public Account {
public:
    // Constructor: khởi tạo account admin, không cần truyền ID, sẽ tự động sinh ra
    AccountAdmin(const std::string& username,
                 const std::string& passwordPlain,
                 const std::string& fullName,
                 const std::string& phone,
                 const std::string& email);


    // Hàm đọc ghi file để tạo và lưu đối tượng
    static AccountAdmin AccountAdmin::fromRecordLine(const std::string& line);
    std::string AccountAdmin::toRecordLine() const;

    // --- Triển khai hàm ảo từ lớp cha ---
    // Thêm 'override' để trình biên dịch kiểm tra tính chính xác
    void displayInfo() const override;
};

#endif
