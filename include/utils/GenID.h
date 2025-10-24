#ifndef GEN_ID_H
#define GEN_ID_H

#include <string>

// Namespace để gom nhóm các hàm, tránh bị trùng tên
namespace IdGenerator {

    // Nạp giá trị các biến đếm từ file khi khởi động chương trình
    void loadCountersFromFile(const std::string& filePath);
    // Lưu giá trị các biến đếm hiện tại ra file trước khi thoát
    void saveCountersToFile(const std::string& filePath);

    // Hàm tạo ID cho Admin, bắt đầu bằng "ADM-"
    std::string generateAdminId();

    // Hàm tạo ID cho Agent, bắt đầu bằng "AGT-"
    std::string generateAgentId();

    // Hàm tạo ID cho Flight, bắt đầu bằng "FL-"
    std::string generateFlightId();

    // Hàm tạo ID cho FlightInstance, bắt đầu bằng "FI-"
    std::string generateInstanceId();
    
    // Hàm tạo ID cho Passenger, bắt đầu bằng "PAX-"
    std::string generatePassengerId();

    // Hàm tạo ID cho Bookings, bắt đầu bằng "BKG-"
    std::string generateBookingId();
}

#endif // GEN_ID_H