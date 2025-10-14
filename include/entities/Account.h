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

private:
    // Helpers nội bộ, chỉ lớp Account mới truy cập được
    static std::string hashPassword(const std::string& plain);
    static bool verifyPassword(const std::string& plain, const std::string& hash);

public:
    // Không cho phép tạo đối tượng Account rỗng
    Account() = delete;

    explicit Account(const std::string& id,
                     const std::string& username,
                     const std::string& passwordPlain, // sẽ hash ở .cpp
                     Role role,
                     const std::string& name,
                     const std::string& phone,
                     const std::string& email);

    // --- Getters ---
    const std::string& getId()        const;
    const std::string& getUsername()  const;
    Role               getRole()      const;
    const std::string& getFullName()  const;
    const std::string& getPhone()     const;
    const std::string& getEmail()     const;

    // Destructor ảo: Bắt buộc phải có cho lớp cơ sở có hàm ảo
    virtual ~Account() = default;

    // --- Setters ---
    void setFullName(const std::string& name);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);

    // --- Chức năng chính ---
    bool authenticate(const std::string& passwordPlain) const;
    void changePassword(const std::string& newPasswordPlain);

    // Chỉ dùng cho việc nạp dữ liệu từ file.
    void overrideIdForLoad(const std::string& existingId);
    void overridePasswordHashForLoad(const std::string& existingHash);

    // --- Hàm ảo thuần túy ---
    // Biến lớp Account thành lớp trừu tượng.
    // Các lớp con (Admin, Agent) BẮT BUỘC phải định nghĩa lại hàm này.
    virtual void displayInfo() const = 0;
};

#endif // ACCOUNT_H
