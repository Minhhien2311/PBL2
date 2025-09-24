#include "C:/PBL2/include/entities/Account.h"
#include "C:/PBL2/include/utils/Helpers.h"
#include <iostream>

Account::Account(const std::string& id, const std::string& name, const std::string& dob, 
           const std::string& email, const std::string& phone, const std::string& addr,
           const std::string& Account, const std::string& pwdHash, 
           const std::string& regDate, const std::string& role)
    : Person(id, name, dob, email, phone, addr), 
      username(Account), passwordHash(pwdHash), 
      registrationDate(regDate), role(role) {
    lastLogin = "";
}

// Getter methods
std::string Account::getUsername() const { return username; }
std::string Account::getPasswordHash() const { return passwordHash; }
std::string Account::getRegistrationDate() const { return registrationDate; }
std::string Account::getLastLogin() const { return lastLogin; }
std::string Account::getRole() const { return role; }

// Setter methods
void Account::setPasswordHash(const std::string& pwdHash) { passwordHash = pwdHash; }
void Account::setLastLogin(const std::string& loginTime) { lastLogin = loginTime; }

// Authentication methods
bool Account::authenticate(const std::string& password) const {
    return Helpers::hashPassword(password) == passwordHash;
}

void Account::changePassword(const std::string& newPassword) {
    passwordHash = Helpers::hashPassword(newPassword);
}

void Account::displayInfo() const {
    Person::displayInfo();
    std::cout << "username: " << username << std::endl;
    std::cout << "Role: " << role << std::endl;
    std::cout << "Registration Date: " << registrationDate << std::endl;
    std::cout << "Last Login: " << lastLogin << std::endl;
}