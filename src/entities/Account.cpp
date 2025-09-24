#include "C:/PBL2/include/entities/Account.h"
#include "C:/PBL2/include/utils/Helpers.h"
#include <iostream>

User::User(const std::string& id, const std::string& name, const std::string& dob, 
           const std::string& email, const std::string& phone, const std::string& addr,
           const std::string& user, const std::string& pwdHash, 
           const std::string& regDate, const std::string& role)
    : Person(id, name, dob, email, phone, addr), 
      username(user), passwordHash(pwdHash), 
      registrationDate(regDate), role(role) {
    lastLogin = "";
}

// Getter methods
std::string User::getUsername() const { return username; }
std::string User::getPasswordHash() const { return passwordHash; }
std::string User::getRegistrationDate() const { return registrationDate; }
std::string User::getLastLogin() const { return lastLogin; }
std::string User::getRole() const { return role; }

// Setter methods
void User::setPasswordHash(const std::string& pwdHash) { passwordHash = pwdHash; }
void User::setLastLogin(const std::string& loginTime) { lastLogin = loginTime; }

// Authentication methods
bool User::authenticate(const std::string& password) const {
    return Helpers::hashPassword(password) == passwordHash;
}

void User::changePassword(const std::string& newPassword) {
    passwordHash = Helpers::hashPassword(newPassword);
}

void User::displayInfo() const {
    Person::displayInfo();
    std::cout << "Username: " << username << std::endl;
    std::cout << "Role: " << role << std::endl;
    std::cout << "Registration Date: " << registrationDate << std::endl;
    std::cout << "Last Login: " << lastLogin << std::endl;
}