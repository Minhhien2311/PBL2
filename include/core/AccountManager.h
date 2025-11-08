#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <QObject>                  // Qt base class for signal/slot support
#include <vector>                   // STL để lưu trữ danh sách các tài khoản
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
class AccountManager : public QObject {
    Q_OBJECT
private:
    // Vector chứa các con trỏ trỏ tới đối tượng AccountAdmin
    std::vector<AccountAdmin*> allAdmins;
    // Vector chứa các con trỏ trỏ tới đối tượng AccountAgent
    std::vector<AccountAgent*> allAgents;
    
    // Con trỏ lưu trữ thông tin của người dùng *hiện tại* đang đăng nhập. Sẽ là nullptr nếu chưa ai đăng nhập.
    Account* currentUser;
    
    // Lưu đường dẫn file để dùng trong các hàm update
    std::string adminFilePath_;
    std::string agentFilePath_;

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
  
    // std::vector<T*> requires manual cleanup in destructor if needed
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

    // Lấy thông tin của người dùng đang đăng nhập. Con trỏ trỏ tới đối tượng Account (có thể là Admin hoặc Agent).
    // Trả về nullptr nếu chưa đăng nhập.
    Account* getCurrentUser() const;

    // Kiểm tra xem đã có ai đăng nhập hay chưa.
    // true nếu 'currentUser' khác nullptr, ngược lại là false.
    bool isLoggedIn() const;

    // Tìm kiếm một Agent bằng ID (ví dụ: "AGT-001").
    // Nhập agentId ID của agent cần tìm.
    // Con trỏ trỏ tới AccountAgent nếu tìm thấy, ngược lại là nullptr.
    AccountAgent* findAgentById(const std::string& agentId);
    
    // Lấy toàn bộ danh sách Agent (dùng cho Admin).
    // Tham chiếu (const) đến vector chứa các con trỏ AccountAgent.
    const std::vector<AccountAgent*>& getAllAgents() const;

    // Tạo một tài khoản Agent mới. Tự động kiểm tra trùng lặp username trước khi tạo.
    // username phải là duy nhất, password sẽ được hash, true nếu tạo thành công, false nếu dữ liệu không hợp lệ hoặc username đã tồn tại.
    bool createNewAgent(const std::string& username,
                        const std::string& password,
                        const std::string& fullName,
                        const std::string& phone,
                        const std::string& email);

    // Kích hoạt hoặc vô hiệu hóa một tài khoản Agent, true nếu tìm thấy Agent và cập nhật thành công.
    bool setAgentStatus(const std::string& agentId, bool isActive);

    // Lưu tất cả thay đổi (danh sách Admin, Agent) về lại file.
    bool saveDataToFiles(const std::string& adminsFilePath, const std::string& agentsFilePath) const;

    // Hàm bổ sung nếu có thể
    void updateAgentProfile(const std::string& agentId, std::string newName, std::string newPhone, std::string newEmail);
    void changeAgentPassword(const std::string& agentId, std::string newPassword);

    // Đổi mật khẩu cho bất kỳ user (Admin hoặc Agent).
    // Trả về true nếu mật khẩu cũ hợp lệ và mật khẩu được cập nhật + ghi file thành công.
    bool changePassword(const std::string& userId,
                        const std::string& oldPasswordPlain,
                        const std::string& newPasswordPlain);
    
    std::string getCurrentAgentId() const;

signals:
    // Phát tín hiệu khi người dùng đăng nhập thành công
    void userLoggedIn(Account* user);
    
    // Phát tín hiệu khi người dùng đăng xuất
    void userLoggedOut();
};

#endif