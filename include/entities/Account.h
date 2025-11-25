#pragma once

#include <string>

enum class Role { Admin, Agent };

class Account {
protected:
    // --- Thông tin đăng nhập ---
    std::string accountId;       // ID duy nhất
    std::string username;
    std::string passwordHash;    // Mật khẩu đã mã hóa
    Role        role;

    // --- Thông tin cá nhân ---
    std::string fullName;
    std::string phoneNumber;
    std::string email;

private:
    // --- Helpers nội bộ ---
    static std::string hashPassword(const std::string& plain);
    static bool verifyPassword(const std::string& plain, const std::string& hash);

public:
    // --- Constructor & Destructor ---
    Account() = delete; // Không cho phép tạo rỗng
    explicit Account(const std::string& id, const std::string& username,
                     const std::string& passwordPlain, Role role,
                     const std::string& name, const std::string& phone,
                     const std::string& email);
    
    virtual ~Account() = default;

    // --- Getters ---
    const std::string& getId() const;
    const std::string& getUsername() const;
    const std::string& getPasswordHash() const;
    Role getRole() const;
    const std::string& getFullName() const;
    const std::string& getPhone() const;
    const std::string& getEmail() const;

    // --- Setters ---
    void setFullName(const std::string& name);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);
    void setPassword(const std::string& newPassword);
    
    // --- Chức năng chính ---
    bool authenticate(const std::string& passwordPlain) const;
    void changePassword(const std::string& newPasswordPlain);

    // --- Hỗ trợ nạp dữ liệu ---
    void overrideIdForLoad(const std::string& existingId);
    void overridePasswordHashForLoad(const std::string& existingHash);
};