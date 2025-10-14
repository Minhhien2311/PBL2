#include "C:/PBL2/include/entities/AccountAdmin.h"
#include "C:/PBL2/include/utils/GenID.h"
#include "C:/PBL2/include/DSA/DynamicArray.h"
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

            // CHÚ Ý: Gọi hàm erase(index) mới
            managedAgents.erase(i);
            
            // Thoát khỏi hàm ngay sau khi xóa
            return true;
        }
    }
    // Nếu duyệt hết vòng lặp mà không tìm thấy, trả về false
    return false;
}

// --- Getter cho danh sách Agent ---
const DynamicArray<AccountAgent>& AccountAdmin::getManagedAgents() const {
    return managedAgents;
}

// Chuyển đổi đối tượng AccountAdmin thành một dòng string để lưu vào file.
std::string AccountAdmin::toRecordLine() const {
    std::string roleStr = std::to_string(static_cast<int>(this->role)); // Truy cập trực tiếp 'role'

    // SỬA LỖI: Truy cập trực tiếp vào thuộc tính 'passwordHash' của lớp cha
    return this->accountId + "|" +
           this->username + "|" +
           this->passwordHash + "|" + 
           roleStr + "|" +
           this->fullName + "|" +
           this->phoneNumber + "|" +
           this->email;
}

// Tạo đối tượng AccountAdmin từ một dòng string đọc từ file.
AccountAdmin AccountAdmin::fromRecordLine(const std::string& line) {
    // Dùng phương pháp phân tích chuỗi thủ công đã thống nhất
    size_t start = 0;
    size_t end = line.find('|');
    
    // Tách các trường dữ liệu từ chuỗi
    std::string id = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string username = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string passwordHash = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    // Bỏ qua role vì chúng ta biết đây chắc chắn là Admin
    start = end + 1;
    end = line.find('|', start);

    std::string fullName = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string phone = line.substr(start, end - start);
    start = end + 1;
    end = line.length();

    std::string email = line.substr(start, end - start);

    // ---- QUAN TRỌNG ----
    // Vì constructor của AccountAdmin tự động sinh ID mới và hash lại mật khẩu, chúng ta cần một cách để tạo đối tượng với dữ liệu đã có sẵn từ file.
    // Giải pháp đơn giản nhất là tạo đối tượng rồi ghi đè lại các giá trị.

    // 1. Tạo một đối tượng Admin tạm thời. Mật khẩu "temp" sẽ bị ghi đè.
    AccountAdmin admin(username, "temp_password", fullName, phone, email);

    // 2. Ghi đè lại ID và passwordHash bằng dữ liệu đã đọc từ file.
    admin.overrideIdForLoad(id);
    admin.overridePasswordHashForLoad(passwordHash);

    return admin;
}

// --- Triển khai hàm ảo ---
// Hiển thị thông tin của Admin và số lượng agent đang quản lý
void AccountAdmin::displayInfo() const {
    return;
}
