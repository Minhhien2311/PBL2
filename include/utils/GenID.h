#ifndef GEN_ID_H
#define GEN_ID_H

#include <string>

// Namespace để gom nhóm các hàm, tránh bị trùng tên
namespace IdGenerator {

    // Hàm tạo ID cho Admin, bắt đầu bằng "ADM-"
    std::string generateAdminId();

    // Hàm tạo ID cho Agent, bắt đầu bằng "AGT-"
    std::string generateAgentId();

}

#endif // GEN_ID_H