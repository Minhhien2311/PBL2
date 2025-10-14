#include "C:/PBL2/include/core/AccountManager.h"
#include <fstream>  // Cần cho việc đọc/ghi file (ifstream, ofstream)
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
                AccountAdmin admin = AccountAdmin::fromRecordLine(line);
                this->allAdmins.push_back(admin);
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
                AccountAgent agent = AccountAgent::fromRecordLine(line);
                this->allAgents.push_back(agent);
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
        if (allAdmins[i].getUsername() == username && allAdmins[i].authenticate(password)) {
            // Dùng con trỏ lớp cha để trỏ tới đối tượng lớp con (tính đa hình)
            this->currentUser = &allAdmins[i];
            return true;
        }
    }

    // 2. Nếu không tìm thấy trong Admin, tìm trong danh sách Agent
    for (size_t i = 0; i < allAgents.size(); ++i) {
        if (allAgents[i].getUsername() == username && allAgents[i].authenticate(password)) {
            this->currentUser = &allAgents[i];
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
        if (allAgents[i].getId() == agentId) {
            return &allAgents[i];
        }
    }
    return nullptr; // Không tìm thấy
}

const DynamicArray<AccountAgent>& AccountManager::getAllAgents() const {
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
        if (allAdmins[i].getUsername() == username) return false;
    }
    for (size_t i = 0; i < allAgents.size(); ++i) {
        if (allAgents[i].getUsername() == username) return false;
    }

    // Nếu tất cả kiểm tra đều qua, tạo đối tượng mới và thêm vào danh sách
    AccountAgent newAgent(username, password, fullName, phone, email);
    this->allAgents.push_back(newAgent);
    
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
        adminFile << allAdmins[i].toRecordLine() << "\n";
    }
    adminFile.close();

    // Lưu danh sách Agent
    std::ofstream agentFile(agentsFilePath);
    if (!agentFile.is_open()) return false;
    for (size_t i = 0; i < allAgents.size(); ++i) {
        agentFile << allAgents[i].toRecordLine() << "\n";
    }
    agentFile.close();

    return true;
}