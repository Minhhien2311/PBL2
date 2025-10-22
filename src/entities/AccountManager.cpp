#include "C:/PBL2/include/core/AccountManager.h"
#include <fstream>
#include <string>

// --- Constructor & Destructor ---

// Constructor: Khởi tạo currentUser là nullptr và nạp dữ liệu từ file.
AccountManager::AccountManager(const std::string& adminsFilePath, const std::string& agentsFilePath) {
    this->currentUser = nullptr;
    this->loadAdminsFromFile(adminsFilePath);
    this->loadAgentsFromFile(agentsFilePath);
}

// --- Hàm trợ giúp nội bộ ---

void AccountManager::loadAdminsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Chỉ xử lý các dòng không rỗng
            if (!line.empty()) {
                // <<< THAY ĐỔI: Tạo đối tượng trên heap bằng 'new' và đẩy con trỏ vào mảng
                AccountAdmin adminOnStack = AccountAdmin::fromRecordLine(line);
                this->allAdmins.push_back(new AccountAdmin(adminOnStack)); 
            }
        }
        file.close();
    }
    // Nếu không mở được file, chương trình sẽ tiếp tục với danh sách admin rỗng.
}

void AccountManager::loadAgentsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // <<< THAY ĐỔI: Tạo đối tượng trên heap bằng 'new' và đẩy con trỏ vào mảng
                AccountAgent agentOnStack = AccountAgent::fromRecordLine(line);
                this->allAgents.push_back(new AccountAgent(agentOnStack));
            }
        }
        file.close();
    }
    // Tương tự, tiếp tục với danh sách agent rỗng nếu không đọc được file.
}

// --- Nghiệp vụ Authentication & Session ---

bool AccountManager::login(const std::string& username, const std::string& password) {
    // Nếu đã có người đăng nhập, không cho phép đăng nhập lại
    if (isLoggedIn()) {
        return false;
    }

    // 1. Tìm trong danh sách Admin
    for (size_t i = 0; i < allAdmins.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ thay vì .
        if (allAdmins[i]->getUsername() == username && allAdmins[i]->authenticate(password)) {
            // <<< THAY ĐỔI: allAdmins[i] đã là một con trỏ, gán trực tiếp
            this->currentUser = allAdmins[i];
            return true;
        }
    }

    // 2. Nếu không tìm thấy trong Admin, tìm trong danh sách Agent
    for (size_t i = 0; i < allAgents.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ thay vì .
        if (allAgents[i]->getUsername() == username && allAgents[i]->authenticate(password)) {
            // <<< THAY ĐỔI: allAgents[i] đã là một con trỏ, gán trực tiếp
            this->currentUser = allAgents[i];
            return true;
        }
    }

    // Nếu không tìm thấy ở cả hai danh sách
    return false;
}

void AccountManager::logout() {
    this->currentUser = nullptr;
}

Account* AccountManager::getCurrentUser() const {
    return this->currentUser;
}

bool AccountManager::isLoggedIn() const {
    return this->currentUser != nullptr;
}

// --- Nghiệp vụ Quản lý Agent ---

AccountAgent* AccountManager::findAgentById(const std::string& agentId) {
    for (size_t i = 0; i < allAgents.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ thay vì .
        if (allAgents[i]->getId() == agentId) {
            // <<< THAY ĐỔI: allAgents[i] đã là một con trỏ, trả về nó
            return allAgents[i];
        }
    }
    return nullptr; // Không tìm thấy
}

// LƯU Ý: Kiểu trả về của hàm này trong file .h cũng phải được sửa thành DynamicArray<AccountAgent*>
const DynamicArray<AccountAgent*>& AccountManager::getAllAgents() const {
    return this->allAgents;
}

// "Người Gác Cổng" kiểm tra dữ liệu trước khi tạo Agent mới.
bool AccountManager::createNewAgent(const std::string& username,
                                     const std::string& password,
                                     const std::string& fullName,
                                     const std::string& phone,
                                     const std::string& email) {
    // Kiểm tra đầu vào cơ bản
    if (username.empty() || password.empty() || fullName.empty()) {
        return false; // Dữ liệu bắt buộc không được rỗng
    }

    // Kiểm tra xem username đã tồn tại chưa
    for (size_t i = 0; i < allAdmins.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ thay vì .
        if (allAdmins[i]->getUsername() == username) return false;
    }
    for (size_t i = 0; i < allAgents.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ thay vì .
        if (allAgents[i]->getUsername() == username) return false;
    }

    // <<< THAY ĐỔI: Tạo đối tượng mới trên heap bằng 'new' và thêm con trỏ vào danh sách
    this->allAgents.push_back(new AccountAgent(username, password, fullName, phone, email));
    
    return true;
}
    
bool AccountManager::setAgentStatus(const std::string& agentId, bool isActive) {
    AccountAgent* agent = findAgentById(agentId);
    if (agent != nullptr) {
        agent->setActive(isActive);
        return true;
    }
    return false; // Không tìm thấy agent
}

// --- Chức năng Lưu trữ ---

bool AccountManager::saveDataToFiles(const std::string& adminsFilePath, const std::string& agentsFilePath) const {
    // Lưu danh sách Admin
    std::ofstream adminFile(adminsFilePath);
    if (!adminFile.is_open()) return false; // Không mở được file để ghi
    for (size_t i = 0; i < allAdmins.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ thay vì .
        adminFile << allAdmins[i]->toRecordLine() << "\n";
    }
    adminFile.close();

    // Lưu danh sách Agent
    std::ofstream agentFile(agentsFilePath);
    if (!agentFile.is_open()) return false;
    for (size_t i = 0; i < allAgents.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ thay vì .
        agentFile << allAgents[i]->toRecordLine() << "\n";
    }
    agentFile.close();

    return true;
}
