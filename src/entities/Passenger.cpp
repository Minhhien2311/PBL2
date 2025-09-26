#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

enum class Role { Admin, Agent };

class Account {
private:
    std::string accountId;       // ID tài khoản (khác ID của Person)
    std::string username;
    std::string passwordHash;    // không expose ra ngoài
    Role        role;

    // Internal helpers (dùng chung cho mọi đối tượng Account)
    // Đặt private để không ai bên ngoài tự băm/verify trái quy trình.
    static std::string hashPassword(const std::string& plain);
    static bool verifyPassword(const std::string& plain, const std::string& hash);

public:
    Account() = delete;

    explicit Account(const std::string& id,
                     const std::string& username,
                     const std::string& passwordPlain, // sẽ hash nội bộ ở .cpp
                     Role role);

    // Getter (không có getPassword)
    const std::string& getId()           const;
    const std::string& getUsername()     const;
    Role               getRole()         const;

    // Auth API
    bool authenticate(const std::string& passwordPlain) const; // so sánh bằng verifyPassword
    void changePassword(const std::string& newPasswordPlain);  // cập nhật passwordHash = hashPassword(newPassword)
};

#endif // ACCOUNT_H