#include "C:/PBL2/include/entities/Account.h"

// Constructor 
Account::Account(const std::string& id, const std::string& username,
                 const std::string& passwordPlain, // sẽ hash ở .cpp
                 Role role, const std::string& name,
                 const std::string& phone, const std::string& email)
    : accountId(id),
      username(username),
      passwordHash(hashPassword(passwordPlain)),
      role(role),
      fullName(name),
      phoneNumber(phone),
      email(email) {}

// Destructor 
// (được = default trong header, nên không cần viết lại ở .cpp)

// Internal helpers 
std::string Account::hashPassword(const std::string& plain) {
    // TODO: Thay thế bằng thuật toán băm thực sự (bcrypt/Argon2...).
    return plain; // placeholder: chỉ để biên dịch
}

bool Account::verifyPassword(const std::string& plain, const std::string& hash) {
    return hash == hashPassword(plain);
}

// Getters 
const std::string& Account::getId() const { return accountId; }

const std::string& Account::getUsername() const { return username; }

Role Account::getRole() const { return role; }

const std::string& Account::getFullName() const { return fullName; }
const std::string& Account::getPhone() const { return phoneNumber; }
const std::string& Account::getEmail() const { return email; }

// Auth API 
bool Account::authenticate(const std::string& passwordPlain) const {
    return verifyPassword(passwordPlain, passwordHash);
}

void Account::changePassword(const std::string& newPasswordPlain) {
    passwordHash = hashPassword(newPasswordPlain);
}
