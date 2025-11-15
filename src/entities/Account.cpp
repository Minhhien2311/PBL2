#include "entities/Account.h"
#include <iostream>

// Constructor 
Account::Account(const std::string& id,
                 const std::string& username,
                 const std::string& passwordPlain, // sẽ hash ở .cpp
                 Role role,
                 const std::string& name,
                 const std::string& phone,
                 const std::string& email)
    : accountId(id),
      username(username),
      passwordHash(hashPassword(passwordPlain)),
      role(role),
      fullName(name),
      phoneNumber(phone),
      email(email) {}

/// --- Internal helpers ---

// Hàm băm mật khẩu (FNV-1a 64-bit + pepper)
// Đây là hàm tĩnh, chỉ được dùng bên trong lớp Account.
std::string Account::hashPassword(const std::string& plain) {
    // Pepper nội bộ
    const char* PEPPER = "pepper_demo_2025";

    // Hằng số FNV-1a 64-bit
    const unsigned long long OFFSET = 1469598103934665603ull;
    const unsigned long long PRIME  = 1099511628211ull;

    // Khởi tạo
    unsigned long long h = OFFSET;

    // Trộn pepper trước (không cần bộ nhớ phụ, chỉ duyệt ký tự)
    for (const char* p = PEPPER; *p; ++p) {
        h ^= (unsigned char)(*p);
        h *= PRIME;
    }

    // Băm mật khẩu: duyệt trực tiếp trên `plain` (không tạo bản sao -> tiết kiệm bộ nhớ)
    for (size_t i = 0; i < plain.size(); ++i) {
        h ^= (unsigned char)plain[i];
        h *= PRIME;
    }

    // Khuếch tán nhẹ (3 vòng)
    for (int i = 0; i < 3; ++i) {
        h ^= (h >> 32);
        h *= PRIME;
        h = (h << 13) | (h >> (64 - 13)); // xoay trái 13 bit
    }

    // Đổi sang hex lowercase (16 ký tự cho 64-bit)
    static const char* HEX = "0123456789abcdef";
    std::string out; out.resize(16);
    for (int i = 15; i >= 0; --i) {
        unsigned int nibble = (unsigned int)(h & 0xFULL);
        out[i] = HEX[nibble];
        h >>= 4;
    }

    return out;
}

bool Account::verifyPassword(const std::string& plain, const std::string& hash) {
    return hash == hashPassword(plain);
}

// --- Getters --- 
const std::string& Account::getPasswordHash() const {
    return this->passwordHash;
}//debug
const std::string& Account::getId() const { return accountId; }
const std::string& Account::getUsername() const { return username; }
Role Account::getRole() const { return role; }
const std::string& Account::getFullName() const { return fullName; }
const std::string& Account::getPhone() const { return phoneNumber; }
const std::string& Account::getEmail() const { return email; }

// --- Setters ---
void Account::setFullName(const std::string& name) {
    this->fullName = name;
}
void Account::setPhone(const std::string& phone) {
    this->phoneNumber = phone;
}
void Account::setEmail(const std::string& email) {
    this->email = email;
}
void Account::setPassword(const std::string& newPassword) {
    // Mã hóa mật khẩu mới trước khi lưu (nếu cần)
    passwordHash = hashPassword(newPassword);
}

// --- Chức năng chính ---
// bool Account::authenticate(const std::string& passwordPlain) const {
//     return verifyPassword(passwordPlain, this->passwordHash);
// }

// ==== DEBUG ======
bool Account::authenticate(const std::string& passwordPlain) const {
    // 1. Hash mật khẩu người dùng nhập vào
    std::string hashed_input = hashPassword(passwordPlain);
    
    // 2. <<< THÊM CÔNG CỤ THÁM TỬ CUỐI CÙNG VÀO ĐÂY
    std::cout << "[DEBUG]   -> Hash tu input: |" << hashed_input << "|\n";

    // 3. Thực hiện so sánh
    return this->passwordHash == hashed_input;
}


void Account::changePassword(const std::string& newPasswordPlain) {
    // Chỉ thay đổi mật khẩu nếu mật khẩu mới không rỗng
    if (!newPasswordPlain.empty()) {
        this->passwordHash = hashPassword(newPasswordPlain);
    }
}

void Account::overrideIdForLoad(const std::string& existingId) {
    this->accountId = existingId;
}
void Account::overridePasswordHashForLoad(const std::string& existingHash) {
    this->passwordHash = existingHash;
}
