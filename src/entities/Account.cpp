#include "C:/PBL2/include/entities/Account.h"
#include <string>

// === Constructor ===
Account::Account(const std::string& id,
                 const std::string& user,
                 const std::string& passwordPlain,
                 Role r)
    : accountId(id),
      username(user),
      // Lưu ý: hiện tại gọi hàm hashPassword (placeholder).
      // Thay bằng hàm băm an toàn khi triển khai thuật toán.
      passwordHash(hashPassword(passwordPlain)),
      role(r)
{}

// === Internal helpers (placeholder) ===
/*
  GHI CHÚ (Thực hiện thuật toán ở đây):
  - Nên dùng bcrypt / scrypt / Argon2 trong thực tế.
  - Sử dụng salt (riêng cho mỗi account) và pepper (bí mật ứng dụng).
  - Lưu salt (nếu dùng) kèm trong DB hoặc mở rộng struct Account để chứa salt.
  - Khi verify: hash(passwordPlain + salt + pepper) và so sánh với stored hash.
*/
std::string Account::hashPassword(const std::string& plain) {
    // PLACEHOLDER: minh họa, KHÔNG an toàn trong thực tế.
    // TODO: Thay thế bằng bcrypt/Argon2 + salt + pepper.
}

bool Account::verifyPassword(const std::string& plain, const std::string& hash) {
    // PLACEHOLDER: so sánh trực tiếp với hàm hash hiện có.
    // TODO: Thay bằng verify phù hợp với thuật toán bạn chọn.
    return hash == hashPassword(plain);
}

// Các hàm getter
const std::string& Account::getId() const { return accountId; }

const std::string& Account::getUsername() const { return username; }

Role Account::getRole() const { return role; }

// === Auth API ===
bool Account::authenticate(const std::string& passwordPlain) const {
    // Không cập nhật lastLogin ở đây (nếu có field đó, chỉ cập nhật khi login thành công)
    return verifyPassword(passwordPlain, passwordHash);
}

void Account::changePassword(const std::string& newPasswordPlain) {
    // Gợi ý: Validate password strength trước khi đổi (nếu cần)
    passwordHash = hashPassword(newPasswordPlain);
}
