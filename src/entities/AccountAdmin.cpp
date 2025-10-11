#include "C:/PBL2/include/entities/AccountAdmin.h"
#include "C:/PBL2/include/utils/GenID.h"
#include <iostream>
#include <algorithm>

// Constructor: Không cần truyền ID, sẽ tự động sinh ra
AccountAdmin::AccountAdmin(const std::string& username,
                           const std::string& passwordPlain,
                           const std::string& fullName,
                           const std::string& phone, const std::string& email)
    : Account(IdGenerator::generateAdminId(), username, passwordPlain, Role::Admin, fullName, phone, email) {
    // Constructor của AccountAdmin, gọi constructor của lớp cơ sở Account
}

// --- Chức năng quản lý Agent ---

// Thêm một agent vào danh sách quản lý
void AccountAdmin::addAgent(const AccountAgent& agent) {
    // Để tránh trùng lặp, có thể thêm kiểm tra xem agentId đã tồn tại chưa
    // (Ở đây làm đơn giản là chỉ thêm vào)
    managedAgents.push_back(agent);
}


// Xóa đúng 1 agent có ID khớp (dừng ngay khi tìm thấy), dùng vòng lặp for với chỉ số (index)
bool AccountAdmin::removeAgent(const std::string& agentId) {
    // Duyệt qua vector bằng chỉ số i
    for (size_t i = 0; i < managedAgents.size(); ++i) {
        // Nếu tìm thấy agent có ID khớp
        if (managedAgents[i].getId() == agentId) {
            // Áp dụng quy tắc nghiệp vụ
            if (managedAgents[i].getIsActive()) {
                return false; 
            }

            // Xóa phần tử tại vị trí i
            managedAgents.erase(managedAgents.begin() + i);
            
            // Thoát khỏi hàm ngay sau khi xóa
            return true; 
        }
    }
    // Nếu duyệt hết vòng lặp mà không tìm thấy, trả về false
    return false;
}


// --- Getter cho danh sách Agent ---
const std::vector<AccountAgent>& AccountAdmin::getManagedAgents() const {
    return managedAgents;
}

// --- Triển khai hàm ảo ---
// Hiển thị thông tin của Admin và số lượng agent đang quản lý
void AccountAdmin::displayInfo() const {
    std::cout << "===== ADMIN ACCOUNT INFO =====" << std::endl;
    std::cout << "ID: \t\t" << this->getId() << std::endl;
    std::cout << "Username: \t" << this->getUsername() << std::endl;
    std::cout << "Full Name: \t" << this->getFullName() << std::endl;
    std::cout << "Role: \t\tAdmin" << std::endl;
    std::cout << "Managing: \t" << this->managedAgents.size() << " agent(s)" << std::endl;
    std::cout << "============================" << std::endl;
}
