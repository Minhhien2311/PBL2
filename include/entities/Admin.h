#ifndef ADMIN_H
#define ADMIN_H

// #include <string> // string is already included by Person.h and Account.h
#include "Person.h"
#include "Account.h"

class Admin : public Person {
private:
    Account account;  // HAS-A Account (Role::Admin)

public:
    // Tạo Admin từ Person fields + Account (đã khởi tạo với Role::Admin)
    Admin(const std::string& id,
          const std::string& name,
          const std::string& dob,
          Gender gender,
          const std::string& phone,
          const Account& acc);

    // Truy cập account (đọc/ghi tuỳ nhu cầu)
    const Account& getAccount() const;
    Account&       getAccount();

    // Hiển thị thông tin (in Person + username/role, không in hash)
    void displayInfo() const override;

    // (Tùy chọn) Nếu cần kiểm tra quyền ngay trong entity:
    // bool isAdmin() const { return getAccount().getRole() == Role::Admin; }
};

#endif // ADMIN_H
