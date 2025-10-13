#ifndef GEN_ID_H
#define GEN_ID_H

#include <string>

// Namespace để gom nhóm các hàm, tránh bị trùng tên
namespace IdGenerator {

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

    // Hàm tạo ID cho Ticket, bắt đầu bằng "TKT-"
    std::string generateTicketNumber();
}

#endif // GEN_ID_H