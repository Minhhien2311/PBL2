#include "C:/PBL2/include/utils/GenID.h" // Đường dẫn tương đối đến file .h
#include <string>
#include <sstream>   // Để xây dựng chuỗi có định dạng
#include <iomanip>   // Để định dạng (setw, setfill)

// Tạo biến đếm tĩnh cho mỗi hàm tạo ID
// Tăng biến đếm trước, rồi mới ghép chuỗi

namespace IdGenerator {

    // Biến đếm tĩnh, chỉ tồn tại trong file này và dành riêng cho Admin
    static int adminCounter = 0;
    std::string generateAdminId() {
        // Tăng biến đếm
        ++adminCounter;

        // Sử dụng stringstream để định dạng
        std::stringstream ss;
        ss << "ADM" 
           << std::setw(3) << std::setfill('0') << adminCounter;
        
        return ss.str();
    }

    // Biến đếm tĩnh, chỉ tồn tại trong file này và dành riêng cho Agent
    static int agentCounter = 0;
    std::string generateAgentId() {
        // Tăng biến đếm
        ++agentCounter;

        // Sử dụng stringstream để định dạng
        std::stringstream ss;
        ss << "AGT" 
           << std::setw(3) << std::setfill('0') << agentCounter;
           
        return ss.str();
    }
}