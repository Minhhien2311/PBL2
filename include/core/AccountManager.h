#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include "C:/PBL2/include/DSA/DynamicArray.h"      // CTDL để lưu trữ danh sách các tài khoản
#include "C:/PBL2/include/entities/AccountAdmin.h" // Đối tượng Admin
#include "C:/PBL2/include/entities/AccountAgent.h" // Đối tượng Agent
#include <string>

class AccountManager {
private:
    DynamicArray<AccountAdmin*> allAdmins;
    DynamicArray<AccountAgent*> allAgents;
    Account* currentUser;

    void loadAdminsFromFile(const std::string& filePath);
    void loadAgentsFromFile(const std::string& filePath);

public:
    AccountManager(const std::string& adminsFilePath, const std::string& agentsFilePath);
    ~AccountManager() = default;

    bool login(const std::string& username, const std::string& password);
    void logout();
    Account* getCurrentUser() const;
    bool isLoggedIn() const;

    AccountAgent* findAgentById(const std::string& agentId);
    
    // <<< THAY ĐỔI: Kiểu trả về bây giờ là một mảng các con trỏ
    const DynamicArray<AccountAgent*>& getAllAgents() const;

    bool createNewAgent(const std::string& username,
                        const std::string& password,
                        const std::string& fullName,
                        const std::string& phone,
                        const std::string& email);
    
    bool setAgentStatus(const std::string& agentId, bool isActive);

    bool saveDataToFiles(const std::string& adminsFilePath, const std::string& agentsFilePath) const;
};

#endif
