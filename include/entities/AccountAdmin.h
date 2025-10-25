// #ifndef ACCOUNT_ADMIN_H
// #define ACCOUNT_ADMIN_H

// #include <string>
// #include <vector>
// #include "Account.h"
// #include "AccountAgent.h"
// #include "C:/PBL2/include/DSA/DynamicArray.h"

// /*
//  * AccountAdmin: tài khoản dành riêng cho Admin
//  *  - Kế thừa từ Account (role = Role::Admin)
//  *  - Có thể mở rộng thêm thuộc tính dành riêng cho admin (nếu cần)
//  */
// class AccountAdmin : public Account {
// public:
//     // Constructor: khởi tạo account admin, không cần truyền ID, sẽ tự động sinh ra
//     AccountAdmin(const std::string& username,
//                  const std::string& passwordPlain,
//                  const std::string& fullName,
//                  const std::string& phone,
//                  const std::string& email);


//     // Hàm đọc ghi file để tạo và lưu đối tượng
//     // static AccountAdmin AccountAdmin::fromRecordLine(const std::string& line);// dòng cũ
//     // std::string AccountAdmin::toRecordLine() const; // dòng cũ

//     static AccountAdmin fromRecordLine(const std::string& line);//dòng mới
//     std::string toRecordLine() const;//dòng mới

//     // --- Triển khai hàm ảo từ lớp cha ---
//     // Thêm 'override' để trình biên dịch kiểm tra tính chính xác
//     void displayInfo() const override;
// };

// #endif

#ifndef ACCOUNT_ADMIN_H
#define ACCOUNT_ADMIN_H

#include <string>
#include <vector>
#include "Account.h"
#include "AccountAgent.h"
#include "C:/PBL2/include/DSA/DynamicArray.h"

class AccountAdmin : public Account {
public:
    AccountAdmin(const std::string& username,
                 const std::string& passwordPlain,
                 const std::string& fullName,
                 const std::string& phone,
                 const std::string& email);

    AccountAdmin() = default;

    static AccountAdmin fromRecordLine(const std::string& line);
    std::string toRecordLine() const;
    
    void displayInfo() const override;
};

#endif
