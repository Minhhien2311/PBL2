#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include "C:/PBL2/include/DSA/DynamicArray.h"      // CTDL để lưu trữ danh sách các tài khoản
#include "C:/PBL2/include/entities/AccountAdmin.h" // Đối tượng Admin
#include "C:/PBL2/include/entities/AccountAgent.h" // Đối tượng Agent
#include <string>

/*
    AccountManager: Lớp quản lý trung tâm cho các nghiệp vụ tài khoản.
    - Chịu trách nhiệm nạp/lưu dữ liệu tài khoản từ file.
    - Xử lý logic đăng nhập, đăng xuất, và quản lý phiên làm việc.
    - Cung cấp các chức năng cho Admin để quản lý Agent.
 */
class AccountManager {
private:
    DynamicArray<AccountAdmin> allAdmins; // "Cơ sở dữ liệu" trong bộ nhớ cho Admin
    DynamicArray<AccountAgent> allAgents; // "Cơ sở dữ liệu" trong bộ nhớ cho Agent

    // Con trỏ tới tài khoản đang đăng nhập.
    // Dùng con trỏ lớp cha `Account*` để có thể trỏ tới Admin hoặc Agent (tính đa hình).
    // Sẽ là `nullptr` nếu không có ai đăng nhập.
    Account* currentUser;

    // --- Hàm trợ giúp nội bộ ---
    // Được gọi bởi constructor để nạp dữ liệu khi chương trình khởi động.
    void loadAdminsFromFile(const std::string& filePath);
    void loadAgentsFromFile(const std::string& filePath);

public:
    // --- Constructor & Destructor ---
    // Constructor sẽ tự động nạp dữ liệu từ file.
    AccountManager(const std::string& adminsFilePath, const std::string& agentsFilePath);
    ~AccountManager() = default; // Destructor mặc định là đủ.

    // --- Nghiệp vụ Authentication & Session ---
    // Thử đăng nhập với username và password. Trả về true nếu thành công.
    // Nếu thành công, `currentUser` sẽ được gán.
    bool login(const std::string& username, const std::string& password);

    // Đăng xuất, reset `currentUser` về nullptr.
    void logout();

    // Lấy thông tin người dùng đang đăng nhập.
    Account* getCurrentUser() const;

    // Kiểm tra xem có người dùng nào đang đăng nhập không.
    bool isLoggedIn() const;

    // --- Nghiệp vụ Quản lý Agent (dành cho Admin) ---
    // Tìm một Agent theo ID.
    AccountAgent* findAgentById(const std::string& agentId);

    // Lấy danh sách tất cả các Agent.
    const DynamicArray<AccountAgent>& getAllAgents() const;

    // Tạo một tài khoản Agent mới.
    // Là "Người Gác Cổng", sẽ kiểm tra dữ liệu trước khi tạo.
    bool createNewAgent(const std::string& username,
                        const std::string& password,
                        const std::string& fullName,
                        const std::string& phone,
                        const std::string& email);
    
    // Kích hoạt hoặc vô hiệu hóa một Agent.
    bool setAgentStatus(const std::string& agentId, bool isActive);

    // --- Chức năng Lưu trữ (Persistence) ---
    // Lưu lại toàn bộ dữ liệu tài khoản hiện tại vào file.
    bool saveDataToFiles(const std::string& adminsFilePath, const std::string& agentsFilePath) const;
};

#endif