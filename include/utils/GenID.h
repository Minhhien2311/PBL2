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

<<<<<<< HEAD
    // Hàm tạo ID cho FlightInstance, bắt đầu bằng "FI-"
    std::string generateInstanceId();

=======
>>>>>>> 1bf0690d5400cbc135025e79f5a69f9bb5b91863
    // Hàm tạo ID cho Bookings, bắt đầu bằng "BKG-"
    std::string generateBookingId();
}

#endif // GEN_ID_H