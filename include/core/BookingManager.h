// #ifndef BOOKING_MANAGER_H
// #define BOOKING_MANAGER_H

// class FlightManager; //fix
// #include "C:/PBL2/include/DSA/DynamicArray.h"
// #include "C:/PBL2/include/entities/Booking.h"
// #include "C:/PBL2/include/entities/Ticket.h"
// #include <string>

// /*
//     BookingManager: Lớp quản lý trung tâm cho các nghiệp vụ đặt chỗ và xuất vé.
//     - Chịu trách nhiệm vòng đời của Booking: tạo, tìm kiếm, hủy, xuất vé.
//     - Tương tác với các Manager khác (ví dụ: FlightManager để kiểm tra ghế trống).
// */
// class BookingManager {
// private:
//     DynamicArray<Booking> allBookings; // "Cơ sở dữ liệu" trong bộ nhớ cho Bookings
//     DynamicArray<Ticket> allTickets;   // "Cơ sở dữ liệu" trong bộ nhớ cho Tickets

//     // --- Hàm trợ giúp nội bộ ---
//     void loadBookingsFromFile(const std::string& filePath);
//     void loadTicketsFromFile(const std::string& filePath);

// public:
//     // --- Constructor & Destructor ---
//     BookingManager(const std::string& bookingsFilePath, const std::string& ticketsFilePath);
//     ~BookingManager() = default;

//     // --- Nghiệp vụ chính ---

//     // Tạo một booking mới (giữ chỗ).
//     // Cần kiểm tra xem chuyến bay có còn đủ chỗ không trước khi tạo.
//     // Trả về con trỏ tới booking vừa tạo, hoặc nullptr nếu thất bại.
//     Booking* createNewBooking( FlightManager& flightManager,
//                                const std::string& pnr,
//                                const std::string& agentId,
//                                const std::string& flightInstanceId,
//                                const std::string& passengerId,
//                                BookingClass bookingClass,
//                                double baseFare);

//     // Chuyển trạng thái Booking thành Issued và tạo ra một Ticket tương ứng.
//     // Trả về con trỏ tới vé vừa tạo, hoặc nullptr nếu thất bại.
//     Ticket* issueTicketForBooking(const std::string& bookingId);

//     // Hủy một booking (nếu hợp lệ).
//     bool cancelBooking(const std::string& bookingId, const std::string& reason);

//     // --- Chức năng Tìm kiếm (Read) ---
//     Booking* findBookingById(const std::string& bookingId);
//     Booking* findBookingByPNR(const std::string& pnr); // PNR là mã tra cứu của khách
//     Ticket* findTicketById(const std::string& ticketId);

//     // --- Chức năng Lưu trữ (Persistence) ---
//     bool saveDataToFiles(const std::string& bookingsFilePath, const std::string& ticketsFilePath) const;
// };

// #endif

#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

class FlightManager; 
#include "C:/PBL2/include/DSA/DynamicArray.h"
#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/entities/Ticket.h"
#include <string>

class BookingManager {
private:
    // <<< THAY ĐỔI: Lưu trữ một mảng các con trỏ thay vì đối tượng
    DynamicArray<Booking*> allBookings; 
    DynamicArray<Ticket*> allTickets;   

    void loadBookingsFromFile(const std::string& filePath);
    void loadTicketsFromFile(const std::string& filePath);

public:
    BookingManager(const std::string& bookingsFilePath, const std::string& ticketsFilePath);
    ~BookingManager() = default;

    Booking* createNewBooking( FlightManager& flightManager,
                               const std::string& pnr,
                               const std::string& agentId,
                               const std::string& flightInstanceId,
                               const std::string& passengerId,
                               BookingClass bookingClass,
                               double baseFare);

    Ticket* issueTicketForBooking(const std::string& bookingId);
    bool cancelBooking(const std::string& bookingId, const std::string& reason);

    Booking* findBookingById(const std::string& bookingId);
    Booking* findBookingByPNR(const std::string& pnr);
    Ticket* findTicketById(const std::string& ticketId);
    
    bool saveDataToFiles(const std::string& bookingsFilePath, const std::string& ticketsFilePath) const;
};

#endif
