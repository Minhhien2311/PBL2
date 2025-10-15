// #include "C:/PBL2/include/core/AccountManager.h"
// #include <fstream>  // Cần cho việc đọc/ghi file (ifstream, ofstream)
// #include <string>

// // --- Constructor & Destructor ---

// // Constructor: Khởi tạo currentUser là nullptr và nạp dữ liệu từ file.
// AccountManager::AccountManager(const std::string& adminsFilePath, const std::string& agentsFilePath) {
//     this->currentUser = nullptr;
//     this->loadAdminsFromFile(adminsFilePath);
//     this->loadAgentsFromFile(agentsFilePath);
// }

// // --- Hàm trợ giúp nội bộ ---

// void AccountManager::loadAdminsFromFile(const std::string& filePath) {
//     std::ifstream file(filePath);
//     std::string line;

//     if (file.is_open()) {
//         while (std::getline(file, line)) {
//             // Chỉ xử lý các dòng không rỗng
//             if (!line.empty()) {
//                 AccountAdmin admin = AccountAdmin::fromRecordLine(line);
//                 this->allAdmins.push_back(admin);
//             }
//         }
//         file.close();
//     }
//     // Nếu không mở được file, chương trình sẽ tiếp tục với danh sách admin rỗng.
// }

// void AccountManager::loadAgentsFromFile(const std::string& filePath) {
//     std::ifstream file(filePath);
//     std::string line;

//     if (file.is_open()) {
//         while (std::getline(file, line)) {
//             if (!line.empty()) {
//                 AccountAgent agent = AccountAgent::fromRecordLine(line);
//                 this->allAgents.push_back(agent);
//             }
//         }
//         file.close();
//     }
//     // Tương tự, tiếp tục với danh sách agent rỗng nếu không đọc được file.
// }

// // --- Nghiệp vụ Authentication & Session ---

// bool AccountManager::login(const std::string& username, const std::string& password) {
//     // Nếu đã có người đăng nhập, không cho phép đăng nhập lại
//     if (isLoggedIn()) {
//         return false;
//     }

//     // 1. Tìm trong danh sách Admin
//     for (size_t i = 0; i < allAdmins.size(); ++i) {
//         if (allAdmins[i].getUsername() == username && allAdmins[i].authenticate(password)) {
//             // Dùng con trỏ lớp cha để trỏ tới đối tượng lớp con (tính đa hình)
//             this->currentUser = &allAdmins[i];
//             return true;
//         }
//     }

//     // 2. Nếu không tìm thấy trong Admin, tìm trong danh sách Agent
//     for (size_t i = 0; i < allAgents.size(); ++i) {
//         if (allAgents[i].getUsername() == username && allAgents[i].authenticate(password)) {
//             this->currentUser = &allAgents[i];
//             return true;
//         }
//     }

//     // Nếu không tìm thấy ở cả hai danh sách
//     return false;
// }

// void AccountManager::logout() {
//     this->currentUser = nullptr;
// }

// Account* AccountManager::getCurrentUser() const {
//     return this->currentUser;
// }

// bool AccountManager::isLoggedIn() const {
//     return this->currentUser != nullptr;
// }

// // --- Nghiệp vụ Quản lý Agent ---

// AccountAgent* AccountManager::findAgentById(const std::string& agentId) {
//     for (size_t i = 0; i < allAgents.size(); ++i) {
//         if (allAgents[i].getId() == agentId) {
//             return &allAgents[i];
//         }
//     }
//     return nullptr; // Không tìm thấy
// }

// const DynamicArray<AccountAgent>& AccountManager::getAllAgents() const {
//     return this->allAgents;
// }

// // "Người Gác Cổng" kiểm tra dữ liệu trước khi tạo Agent mới.
// bool AccountManager::createNewAgent(const std::string& username,
//                                     const std::string& password,
//                                     const std::string& fullName,
//                                     const std::string& phone,
//                                     const std::string& email) {
//     // Kiểm tra đầu vào cơ bản
//     if (username.empty() || password.empty() || fullName.empty()) {
//         return false; // Dữ liệu bắt buộc không được rỗng
//     }

//     // Kiểm tra xem username đã tồn tại chưa
//     for (size_t i = 0; i < allAdmins.size(); ++i) {
//         if (allAdmins[i].getUsername() == username) return false;
//     }
//     for (size_t i = 0; i < allAgents.size(); ++i) {
//         if (allAgents[i].getUsername() == username) return false;
//     }

//     // Nếu tất cả kiểm tra đều qua, tạo đối tượng mới và thêm vào danh sách
//     AccountAgent newAgent(username, password, fullName, phone, email);
//     this->allAgents.push_back(newAgent);
    
//     return true;
// }
    
// bool AccountManager::setAgentStatus(const std::string& agentId, bool isActive) {
//     AccountAgent* agent = findAgentById(agentId);
//     if (agent != nullptr) {
//         agent->setActive(isActive);
//         return true;
//     }
//     return false; // Không tìm thấy agent
// }

// // --- Chức năng Lưu trữ ---

// bool AccountManager::saveDataToFiles(const std::string& adminsFilePath, const std::string& agentsFilePath) const {
//     // Lưu danh sách Admin
//     std::ofstream adminFile(adminsFilePath);
//     if (!adminFile.is_open()) return false; // Không mở được file để ghi
//     for (size_t i = 0; i < allAdmins.size(); ++i) {
//         adminFile << allAdmins[i].toRecordLine() << "\n";
//     }
//     adminFile.close();

//     // Lưu danh sách Agent
//     std::ofstream agentFile(agentsFilePath);
//     if (!agentFile.is_open()) return false;
//     for (size_t i = 0; i < allAgents.size(); ++i) {
//         agentFile << allAgents[i].toRecordLine() << "\n";
//     }
//     agentFile.close();

//     return true;
// }

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

//   ==== DEBUG ===== 
// #include "core/AccountManager.h"
// #include <fstream>
// #include <string>
// #include <iostream> // <<< THÊM VÀO ĐỂ DEBUG

// // --- Constructor & Destructor ---
// AccountManager::AccountManager(const std::string& adminsFilePath, const std::string& agentsFilePath) {
//     this->currentUser = nullptr;
//     this->loadAdminsFromFile(adminsFilePath);
//     this->loadAgentsFromFile(agentsFilePath);
// }

// // --- Hàm trợ giúp nội bộ ---
// void AccountManager::loadAdminsFromFile(const std::string& filePath) {
//     std::ifstream file(filePath);
//     std::string line;

//     // --- CÔNG CỤ THÁM TỬ #1: KIỂM TRA VIỆC ĐỌC FILE ---
//     std::cout << "[DEBUG] Dang doc file admin tai: " << filePath << std::endl;
//     if (!file.is_open()) {
//         std::cout << "[DEBUG] LOI: Khong mo duoc file admin!" << std::endl;
//         return;
//     }
//     std::cout << "[DEBUG] Mo file admin thanh cong." << std::endl;


//     while (std::getline(file, line)) {
//         // --- CÔNG CỤ THÁM TỬ #2: XEM DỮ LIỆU THÔ ĐỌC ĐƯỢC ---
//         std::cout << "[DEBUG] Doc duoc dong du lieu tho: \"" << line << "\"" << std::endl;
        
//         if (!line.empty()) {
//             // Xóa ký tự '\r' (carriage return) nếu có - KẺ TÌNH NGHI SỐ MỘT
//             if (line.back() == '\r') {
//                 line.pop_back();
//                 std::cout << "[DEBUG] Da xoa ky tu '\\r'. Dong du lieu sach: \"" << line << "\"" << std::endl;
//             }
            
//             AccountAdmin adminOnStack = AccountAdmin::fromRecordLine(line);
//             this->allAdmins.push_back(new AccountAdmin(adminOnStack));
//         }
//     }
//     file.close();
// }

// void AccountManager::loadAgentsFromFile(const std::string& filePath) {
//     // (Làm tương tự cho file agent nếu cần)
//     std::ifstream file(filePath);
//     std::string line;
//     if (file.is_open()) {
//         while (std::getline(file, line)) {
//             if (!line.empty()) {
//                 if (line.back() == '\r') {
//                     line.pop_back();
//                 }
//                 AccountAgent agentOnStack = AccountAgent::fromRecordLine(line);
//                 this->allAgents.push_back(new AccountAgent(agentOnStack));
//             }
//         }
//         file.close();
//     }
// }

// // --- Nghiệp vụ Authentication & Session ---
// bool AccountManager::login(const std::string& username, const std::string& password) {
//     // --- CÔNG CỤ THÁM TỬ #3: KIỂM TRA DỮ LIỆU ĐẦU VÀO KHI LOGIN ---
//     std::cout << "\n[DEBUG] === BAT DAU LOGIN ===\n";
//     std::cout << "[DEBUG] Input: user=|" << username << "|, pass=|" << password << "|\n";
//     std::cout << "[DEBUG] Co tong so " << allAdmins.size() << " admin trong he thong.\n";

//     for (int i = 0; i < allAdmins.size(); ++i) {
//         // --- CÔNG CỤ THÁM TỬ #4: XEM DỮ LIỆU ĐANG ĐƯỢC SO SÁNH ---
//         std::cout << "[DEBUG] Kiem tra admin #" << i << ": user=|" << allAdmins[i]->getUsername() << "|\n";
        
//         // So sánh username
//         bool username_match = (allAdmins[i]->getUsername() == username);
//         std::cout << "[DEBUG]   -> So sanh username: " << (username_match ? "DUNG" : "SAI") << "\n";

//         // So sánh password
//         bool password_match = allAdmins[i]->authenticate(password);
//         std::cout << "[DEBUG]   -> So sanh password: " << (password_match ? "DUNG" : "SAI") << "\n";

// std::cout << "[DEBUG]   -> Hash tu file: |" << allAdmins[i]->getPasswordHash() << "|\n"; // Giả sử bạn có hàm getPasswordHash()

// std::cout << "[DEBUG]   -> So sanh password: " << (password_match ? "DUNG" : "SAI") << "\n";
        
//         if (username_match && password_match) {
//             std::cout << "[DEBUG] === LOGIN THANH CONG! ===\n\n";
//             this->currentUser = allAdmins[i];
//             return true;
//         }
//     }

//     // (Làm tương tự cho agent nếu cần)
    
//     std::cout << "[DEBUG] === LOGIN THAT BAI! ===\n\n";
//     return false;
// }

// // ... các hàm còn lại giữ nguyên ...
// void AccountManager::logout() {
//     this->currentUser = nullptr;
// }

// Account* AccountManager::getCurrentUser() const {
//     return this->currentUser;
// }

// bool AccountManager::isLoggedIn() const {
//     return this->currentUser != nullptr;
// }

// AccountAgent* AccountManager::findAgentById(const std::string& agentId) {
//     for (int i = 0; i < allAgents.size(); ++i) {
//         if (allAgents[i]->getId() == agentId) {
//             return allAgents[i];
//         }
//     }
//     return nullptr;
// }

// const DynamicArray<AccountAgent*>& AccountManager::getAllAgents() const {
//     return this->allAgents;
// }

// bool AccountManager::createNewAgent(const std::string& username,
//                                      const std::string& password,
//                                      const std::string& fullName,
//                                      const std::string& phone,
//                                      const std::string& email) {
//     if (username.empty() || password.empty() || fullName.empty()) {
//         return false;
//     }
//     for (int i = 0; i < allAdmins.size(); ++i) {
//         if (allAdmins[i]->getUsername() == username) return false;
//     }
//     for (int i = 0; i < allAgents.size(); ++i) {
//         if (allAgents[i]->getUsername() == username) return false;
//     }
//     this->allAgents.push_back(new AccountAgent(username, password, fullName, phone, email));
//     return true;
// }
    
// bool AccountManager::setAgentStatus(const std::string& agentId, bool isActive) {
//     AccountAgent* agent = findAgentById(agentId);
//     if (agent != nullptr) {
//         agent->setActive(isActive);
//         return true;
//     }
//     return false;
// }

// bool AccountManager::saveDataToFiles(const std::string& adminsFilePath, const std::string& agentsFilePath) const {
//     std::ofstream adminFile(adminsFilePath);
//     if (!adminFile.is_open()) return false;
//     for (int i = 0; i < allAdmins.size(); ++i) {
//         adminFile << allAdmins[i]->toRecordLine() << "\n";
//     }
//     adminFile.close();
//     std::ofstream agentFile(agentsFilePath);
//     if (!agentFile.is_open()) return false;
//     for (int i = 0; i < allAgents.size(); ++i) {
//         agentFile << allAgents[i]->toRecordLine() << "\n";
//     }
//     agentFile.close();
//     return true;
// }

