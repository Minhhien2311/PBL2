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
        ss << "ADM-" 
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
        ss << "AGT-" 
           << std::setw(3) << std::setfill('0') << agentCounter;
           
        return ss.str();
    }

    // Biến đếm tĩnh, chỉ tồn tại trong file này và dành riêng cho Flight
    static int flightCounter = 0;
    std::string generateFlightId() {
        ++flightCounter;
        
        // Dùng stringstream hoặc std::format để tạo định dạng "FL-001"
        // (Hoặc cách thủ công bằng std::to_string nếu bạn muốn)
        std::stringstream ss;
        ss << "FL-" << std::setw(3) << std::setfill('0') << flightCounter;
        return ss.str();
    }

    static int instanceCounter = 0;
    std::string generateInstanceId() {
        ++instanceCounter;
        
        // Dùng stringstream để tạo định dạng "FI-00001"
        // Dùng 5 chữ số để có thể lưu được nhiều chuyến bay
        std::stringstream ss;
        ss << "FI-" << std::setw(5) << std::setfill('0') << instanceCounter;
        return ss.str();
    }

    static int passengerCounter = 0;
    std::string generatePassengerId() {
        ++passengerCounter;
        
        // Dùng stringstream để tạo định dạng "FI-00001"
        // Dùng 5 chữ số để có thể lưu được nhiều chuyến bay
        std::stringstream ss;
        ss << "PAX-" << std::setw(5) << std::setfill('0') << instanceCounter;
        return ss.str();
    }

    static int ticketCounter = 0;
    std::string generateTicketId() {
        ++ticketCounter;
        
        // Dùng stringstream để tạo định dạng "TKT-0000001"
        // Dùng 5 chữ số để có thể lưu được nhiều chuyến bay
        std::stringstream ss;
        ss << "TKT-" << std::setw(7) << std::setfill('0') << instanceCounter;
        return ss.str();
    }
}