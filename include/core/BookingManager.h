#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

// Khai báo tiền định (forward declaration)
class FlightManager; 

#include "C:/PBL2/include/DSA/DynamicArray.h"
#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/entities/Ticket.h"
#include <string>

/*
BookingManager (Đã tái cấu trúc - Siêu đơn giản)
Chịu trách nhiệm cho 2 nghiệp vụ cốt lõi:
1. Bán vé: Trừ ghế, tạo Booking (lịch sử) + Ticket (vé active).
2. Hủy vé: Hoàn ghế, đổi trạng thái Ticket (sang cancelled).
*/
class BookingManager {
private:
    // Mảng động lưu trữ con trỏ tới các đối tượng Booking (lịch sử)
    DynamicArray<Booking*> allBookings;
    // Mảng động lưu trữ con trỏ tới các đối tượng Ticket (vé có trạng thái)
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

    /*
    (NGHIỆP VỤ CỐT LÕI) Bán vé: Tạo Booking VÀ Xuất Vé ngay lập tức.
     *
    1. Kiểm tra và trừ ghế từ FlightManager.
    2. Tạo đối tượng Booking (làm "hóa đơn" lịch sử).
    3. Tạo đối tượng Ticket (status = Active).
     *
    flightManager Tham chiếu đến FlightManager (để trừ ghế).
    flightInstanceId ID của chuyến bay cụ thể.
    passengerId ID của hành khách.
    bookingClass Hạng vé (Economy / Business).
    baseFare Giá vé tại thời điểm bán.
    Con trỏ trỏ tới Ticket mới nếu thành công, ngược lại là nullptr (nếu hết ghế).
    */
    Ticket* createBookingAndIssueTicket( FlightManager& flightManager,
                                       const std::string& flightInstanceId,
                                       const std::string& passengerId,
                                       BookingClass bookingClass,
                                       double baseFare);

    /*
    (NGHIỆP VỤ CỐT LÕI) Hủy một Vé đã được xuất.
    1. Tìm Ticket, kiểm tra xem có 'Active' không.
    2. Tìm Booking gốc để lấy thông tin chuyến bay.
    3. Hoàn trả ghế về cho FlightManager.
    4. Đổi trạng thái Ticket -> Cancelled.
     *
    flightManager Tham chiếu đến FlightManager (để hoàn trả ghế).
    ticketId ID của vé (TKT-XXXX) cần hủy.
    true nếu hủy thành công.
    */
    bool cancelTicket(FlightManager& flightManager, const std::string& ticketId);

    // --- Tìm kiếm ---
    Booking* findBookingById(const std::string& bookingId);
    Ticket* findTicketById(const std::string& ticketId);
    
    // --- Lưu trữ ---
    bool saveDataToFiles(const std::string& bookingsFilePath, const std::string& ticketsFilePath) const;
};

#endif