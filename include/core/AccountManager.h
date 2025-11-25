#pragma once

#include <QObject>
#include <vector>
#include <string>
#include "entities/AccountAdmin.h"
#include "entities/AccountAgent.h"

class AccountManager : public QObject {
    Q_OBJECT

public:
    // --- Constructor & Destructor ---
    AccountManager();
    AccountManager(const std::string& adminsFilePath, const std::string& agentsFilePath);
    ~AccountManager() = default;

    // --- Authentication (Đăng nhập / Đăng xuất) ---
    bool login(const std::string& username, const std::string& password);
    void logout();
    bool isLoggedIn() const;

    // --- Current User Info (Thông tin người dùng hiện tại) ---
    Account* getCurrentUser() const;
    std::string getCurrentUserName() const;
    const std::string getCurrentAgentId() const;

    // --- Account Lookup (Tìm kiếm tài khoản) ---
    AccountAgent* findAgentById(const std::string& agentId);
    AccountAdmin* findAdminById(const std::string& adminId);
    const std::vector<AccountAgent*>& getAllAgents() const;

    // --- Account Management (Quản lý Agent - Dành cho Admin) ---
    bool createNewAgent(const std::string& username, const std::string& password, 
                        const std::string& fullName, const std::string& phone, 
                        const std::string& email);
    bool setAgentStatus(const std::string& agentId, bool isActive);

    // --- Profile & Security (Thông tin cá nhân & Bảo mật) ---
    void updateProfile(const std::string& accountId, std::string newName, 
                       std::string newPhone, std::string newEmail);
    bool changePassword(const std::string& userId, const std::string& oldPasswordPlain, 
                        const std::string& newPasswordPlain);
    bool resetPassword(const std::string& email, const std::string& newPassword);

    // --- Data Persistence (Lưu trữ dữ liệu) ---
    bool saveDataToFiles(const std::string& adminsFilePath, const std::string& agentsFilePath) const;
    bool saveAdminsToFile(const std::string& filePath);
    bool saveAgentsToFile(const std::string& filePath);

signals:
    void userLoggedIn(Account* user);
    void userLoggedOut();

private:
    // --- Data Members ---
    std::vector<AccountAdmin*> allAdmins;
    std::vector<AccountAgent*> allAgents;
    Account* currentUser; // nullptr nếu chưa login

    std::string adminFilePath_;
    std::string agentFilePath_;

    // --- Internal Helpers ---
    void loadAdminsFromFile(const std::string& filePath);
    void loadAgentsFromFile(const std::string& filePath);
};