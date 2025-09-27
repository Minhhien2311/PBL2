#include "C:/PBL2/include/entities/AccountAdmin.h"
#include <iostream>

// Constructor: gọi base Account với role = Admin
AccountAdmin::AccountAdmin(const std::string& id,
                           const std::string& username,
                           const std::string& passwordPlain,
                           const std::string& fullName,
                           const std::string& phone,
                           const std::string& email)
    : accountId(id),
      username(username),
      passwordHash(hashPassword(passwordPlain)),
      role(role),
      fullName(name),
      phoneNumber(phone),
      email(email)
    // Lưu ý: nếu Account không lưu fullName/phone/email thì các tham số này chỉ để tương thích chữ ký.
{
}

// Thêm agent (không cho sửa agent theo yêu cầu)
void AccountAdmin::addAgent(const AccountAgent& agent) { }

// Xóa agent theo agentId
void AccountAdmin::removeAgent(const std::string& agentId) { }

// Xem danh sách agent
const std::vector<AccountAgent>& AccountAdmin::getAgents() const { }

// Hiển thị thông tin admin + số lượng agent đang quản lý
void AccountAdmin::displayInfo() const { }
