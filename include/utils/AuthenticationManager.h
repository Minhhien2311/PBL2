//Quản lý xác thực người dùng
#ifndef AUTHENTICATIONMANAGER_H
#define AUTHENTICATIONMANAGER_H

#include <string>
#include "../entities/Account.h"

class AuthenticationManager {
private:
    static Account* currentUser;

public:
    static bool login(const std::string& username, const std::string& password);
    static bool logout();
    static bool isLoggedIn();
    static Account* getCurrentUser();
    static bool hasPermission(const std::string& permission);
    static bool changePassword(const std::string& oldPassword, const std::string& newPassword);
    static bool resetPassword(const std::string& username, const std::string& newPassword);
    static bool registerNewUser(const Account& newUser);
};

#endif // AUTHENTICATIONMANAGER_H