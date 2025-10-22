#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

// Khai báo tiền định (forward declaration)
class FlightManager; 

#include "C:/PBL2/include/DSA/DynamicArray.h"
#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/entities/Ticket.h"
#include <string>

/*
 BookingManager quản lý logic nghiệp vụ liên quan đến Đặt chỗ (Booking) và Vé (Ticket).
Lớp này chịu trách nhiệm:
    1. Tải danh sách Booking và Ticket từ file.
    2. Cung cấp hàm tạo Booking mới (nghiệp vụ cốt lõi).
    3. Cung cấp hàm xuất vé (issue) hoặc hủy (cancel) booking.
    4. Tìm kiếm Booking/Ticket theo ID.
    5. Lưu lại thay đổi ra file.
 */
class BookingManager {
private:
    // Mảng động lưu trữ con trỏ tới các đối tượng Booking
    DynamicArray<Booking*> allBookings;
    // Mảng động lưu trữ con trỏ tới các đối tượng Ticket
    DynamicArray<Ticket*> allTickets;

    // --- Hàm trợ giúp nội bộ ---
    void loadBookingsFromFile(const std::string& filePath);
    void loadTicketsFromFile(const std::string& filePath);

public:
   /*
    Hàm khởi tạo, tự động tải dữ liệu bookings và tickets.
     */
    BookingManager(const std::string& bookingsFilePath, const std::string& ticketsFilePath);
    
    ~BookingManager() = default;

    // (NGHIỆP VỤ CỐT LÕI) Tạo một đặt chỗ (Booking) mới.
    Booking* createNewBooking( FlightManager& flightManager,
                               const std::string& flightInstanceId,
                               const std::string& passengerId,
                               BookingClass bookingClass,
                               double baseFare);

   /*
    Xuất vé từ một Booking đã 'Confirmed'.
    bookingId ID của Booking cần xuất vé.
    Con trỏ trỏ tới Ticket mới nếu thành công.
     */
    Ticket* issueTicketForBooking(const std::string& bookingId);

   /*
    Hủy một Booking.
    *** ĐÃ CẬP NHẬT: Thêm FlightManager để hoàn trả ghế ***
    flightManager Tham chiếu đến FlightManager (để hoàn trả ghế).
    bookingId ID của Booking cần hủy.
    reason Lý do hủy.
    true nếu hủy thành công.
     */
    bool cancelBooking(FlightManager& flightManager, const std::string& bookingId, const std::string& reason);

    // --- Tìm kiếm ---
    Booking* findBookingById(const std::string& bookingId);
    Ticket* findTicketById(const std::string& ticketId);
    
    // --- Lưu trữ ---
    bool saveDataToFiles(const std::string& bookingsFilePath, const std::string& ticketsFilePath) const;
};

#endif