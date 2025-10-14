#include "C:/PBL2/include/utils/GenID.h" // Đường dẫn tương đối đến file .h
#include <string>
#include <sstream>   // Để xây dựng chuỗi có định dạng
#include <iomanip>   // Để định dạng (setw, setfill)
#include <fstream> // Cần cho ifstream và ofstream

// Tạo biến đếm tĩnh cho mỗi hàm tạo ID
// Tăng biến đếm trước, rồi mới ghép chuỗi

namespace IdGenerator {

    // 2 hàm này cần thiết để tránh bug sinh ID khi tạo đối tượng từ file
    void loadCountersFromFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return; // Nếu file chưa tồn tại, các biến đếm sẽ giữ giá trị mặc định là 0
        }

        std::string line;
        while (std::getline(file, line)) {
            // Tìm vị trí của dấu hai chấm ':'
            size_t delimiterPos = line.find(':');
            if (delimiterPos == std::string::npos) {
                continue; // Bỏ qua nếu dòng không đúng định dạng
            }

            // Tách key và value ra khỏi dòng
            std::string key = line.substr(0, delimiterPos);
            int value = std::stoi(line.substr(delimiterPos + 1));

            // Dùng chuỗi if-else if để gán giá trị, thay thế cho map
            if (key == "ADMIN_COUNTER") {
                adminCounter = value;
            } else if (key == "AGENT_COUNTER") {
                agentCounter = value;
            } else if (key == "FLIGHT_COUNTER") {
                flightCounter = value;
            } else if (key == "INSTANCE_COUNTER") {
                instanceCounter = value;
            } else if (key == "PASSENGER_COUNTER") {
                passengerCounter = value;
            } else if (key == "TICKET_COUNTER") {
                ticketCounter = value;
            } else if (key == "BOOKING_COUNTER") {
                bookingCounter = value;
            }
        }
        file.close();
    }
    void saveCountersToFile(const std::string& filePath) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            // Có thể thêm xử lý lỗi ở đây nếu muốn, ví dụ in ra console
            return;
        }

        file << "ADMIN_COUNTER:" << adminCounter << "\n";
        file << "AGENT_COUNTER:" << agentCounter << "\n";
        file << "FLIGHT_COUNTER:" << flightCounter << "\n";
        file << "INSTANCE_COUNTER:" << instanceCounter << "\n";
        file << "PASSENGER_COUNTER:" << passengerCounter << "\n";
        file << "TICKET_COUNTER:" << ticketCounter << "\n";
        file << "BOOKING_COUNTER:" << bookingCounter << "\n";

        file.close();
    }

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
    std::string generateTicketNumber() {
        ++ticketCounter;
        
        // Dùng stringstream để tạo định dạng "TKT-0000001"
        // Dùng 5 chữ số để có thể lưu được nhiều chuyến bay
        std::stringstream ss;
        ss << "TKT-" << std::setw(7) << std::setfill('0') << instanceCounter;
        return ss.str();
    }

    static int bookingCounter = 0;
    std::string generateBookingId() {
        ++bookingCounter;
        
        // Dùng stringstream để tạo định dạng "BKG-0000001"
        // Dùng 5 chữ số để có thể lưu được nhiều chuyến bay
        std::stringstream ss;
        ss << "BKG-" << std::setw(7) << std::setfill('0') << instanceCounter;
        return ss.str();
    }
}