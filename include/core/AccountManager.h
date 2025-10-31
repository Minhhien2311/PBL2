#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include "DSA/DynamicArray.h"      // CTDL để lưu trữ danh sách các tài khoản
#include "entities/AccountAdmin.h" // Đối tượng Admin
#include "entities/AccountAgent.h" // Đối tượng Agent
#include <string>

/*
    AccountManager quản lý toàn bộ logic liên quan đến tài khoản người dùng (Admin và Agent).
    Lớp này chịu trách nhiệm:
    1. Tải danh sách tài khoản (Admin, Agent) từ file lúc khởi động.
    2. Xác thực thông tin đăng nhập (login).
    3. Quản lý phiên đăng nhập (ai đang đăng nhập, đã đăng nhập chưa).
    4. Cung cấp các hàm để tìm kiếm, tạo mới, và thay đổi trạng thái tài khoản.
    5. Lưu lại toàn bộ danh sách tài khoản vào file khi cần.
 */
class AccountManager {
private:
    // Mảng động chứa các con trỏ trỏ tới đối tượng AccountAdmin
    DynamicArray<AccountAdmin*> allAdmins;
    // Mảng động chứa các con trỏ trỏ tới đối tượng AccountAgent
    DynamicArray<AccountAgent*> allAgents;
    
    // Con trỏ lưu trữ thông tin của người dùng *hiện tại* đang đăng nhập.
    // Sẽ là nullptr nếu chưa ai đăng nhập.
    Account* currentUser;

    // --- Các hàm trợ giúp nội bộ ---

    
    // Tải danh sách tài khoản Admin từ file văn bản.
    void loadAdminsFromFile(const std::string& filePath);
    
    // Tải danh sách tài khoản Agent từ file văn bản.
    void loadAgentsFromFile(const std::string& filePath);

public:
    AccountManager() {}
    /*
    Hàm khởi tạo cho AccountManager.
    Tự động gọi các hàm load...FromFile để nạp dữ liệu.
     */
    AccountManager(const std::string& adminsFilePath, const std::string& agentsFilePath);
    
    /*
    Hàm hủy mặc định.
    (Lưu ý: DynamicArray<T*> đã được code để tự động 'delete' các con trỏ
    khi nó bị hủy, nên ở đây không cần làm gì thêm).
     */
    ~AccountManager() = default;

    /*
    Xác thực thông tin đăng nhập của người dùng:
    1. Sẽ tìm trong cả danh sách Admin và Agent.
    2. Nếu thành công, gán con trỏ 'currentUser' về tài khoản đó.
    3. Nhập username và password
    4. Trả về true nếu đăng nhập thành công, false nếu thất bại.
     */
    bool login(const std::string& username, const std::string& password);
    
    // Đăng xuất người dùng hiện tại, gán 'currentUser' về nullptr.
    void logout();

    /*
    Lấy thông tin của người dùng đang đăng nhập.
    Con trỏ trỏ tới đối tượng Account (có thể là Admin hoặc Agent).
    Trả về nullptr nếu chưa đăng nhập.
     */
    Account* getCurrentUser() const;

    /*
    Kiểm tra xem đã có ai đăng nhập hay chưa.
    true nếu 'currentUser' khác nullptr, ngược lại là false.
     */
    bool isLoggedIn() const;

    /*
    Tìm kiếm một Agent bằng ID (ví dụ: "AGT-001").
    Nhập agentId ID của agent cần tìm.
    Con trỏ trỏ tới AccountAgent nếu tìm thấy, ngược lại là nullptr.
     */
    AccountAgent* findAgentById(const std::string& agentId);
    
    /*
    Lấy toàn bộ danh sách Agent (dùng cho Admin).
    Tham chiếu (const) đến mảng động chứa các con trỏ AccountAgent.
     */
    const DynamicArray<AccountAgent*>& getAllAgents() const;

    /*
    Tạo một tài khoản Agent mới.
    Tự động kiểm tra trùng lặp username trước khi tạo.
    username Tên đăng nhập (phải là duy nhất).
    password Mật khẩu thô (sẽ được hash).
    true nếu tạo thành công, false nếu dữ liệu không hợp lệ hoặc username đã tồn tại.
     */
    bool createNewAgent(const std::string& username,
                        const std::string& password,
                        const std::string& fullName,
                        const std::string& phone,
                        const std::string& email);
    
    /*
    Kích hoạt hoặc vô hiệu hóa một tài khoản Agent.
    agentId ID của Agent cần thay đổi.
    isActive Trạng thái mới (true = kích hoạt, false = vô hiệu hóa).
    true nếu tìm thấy Agent và cập nhật thành công.
     */
    bool setAgentStatus(const std::string& agentId, bool isActive);

    /*
    Lưu tất cả thay đổi (danh sách Admin, Agent) về lại file.
    adminsFilePath Đường dẫn file admins.
    agentsFilePath Đường dẫn file agents.
    true nếu lưu cả 2 file thành công.
     */
    bool saveDataToFiles(const std::string& adminsFilePath, const std::string& agentsFilePath) const;

    // Hàm bổ sung nếu có thể
    void updateAgentProfile(const std::string& agentId, std::string newName, std::string newPhone, std::string newEmail);
    void changeAgentPassword(const std::string& agentId, std::string newPassword);
};

#endif