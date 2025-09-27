#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

enum class Role { Admin, Agent };

class Account {
protected:
    // Thông tin đăng nhập 
    std::string accountId;       // ID tài khoản (unique)
    std::string username;
    std::string passwordHash;    // chỉ lưu hash
    Role        role;

    // Thông tin cá nhân cơ bản 
    std::string fullName;        // Họ tên của admin/agent
    std::string phoneNumber;     // Số điện thoại
    std::string email;           // Email liên hệ

    // Helpers
    static std::string hashPassword(const std::string& plain);
    static bool verifyPassword(const std::string& plain, const std::string& hash);

public:
    Account() = delete;

    explicit Account(const std::string& id,
                     const std::string& username,
                     const std::string& passwordPlain, // sẽ hash ở .cpp
                     Role role,
                     const std::string& name,
                     const std::string& phone,
                     const std::string& email);

    // Getters
    const std::string& getId()        const;
    const std::string& getUsername()  const;
    Role               getRole()      const;
    const std::string& getFullName()  const;
    const std::string& getPhone()     const;
    const std::string& getEmail()     const;

    // Setters cho profile (tùy chọn)
    void setFullName(const std::string& name);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);

    // Auth API
    bool authenticate(const std::string& passwordPlain) const;
    void changePassword(const std::string& newPasswordPlain);
};

#endif // ACCOUNT_H
