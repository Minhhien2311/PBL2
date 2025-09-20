//Quản lý xác thực người dùng
#ifndef AUTHENTICATIONMANAGER_H
#define AUTHENTICATIONMANAGER_H

#include <string>
#include "../entities/Account.h"

class AuthenticationManager {
private:
    static User* currentUser;

public:
    static bool login(const std::string& username, const std::string& password);
    static bool logout();
    static bool isLoggedIn();
    static User* getCurrentUser();
    static bool hasPermission(const std::string& permission);
    static bool changePassword(const std::string& oldPassword, const std::string& newPassword);
    static bool resetPassword(const std::string& username, const std::string& newPassword);
    static bool registerNewUser(const User& newUser);
};

#endif // AUTHENTICATIONMANAGER_H