#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

// Khai báo tiền định (forward declaration)
class FlightManager; 

#include "C:/PBL2/include/DSA/DynamicArray.h"
#include "C:/PBL2/include/entities/Booking.h" // Chỉ include Booking
#include <string>

/*
BookingManager (Đã tái cấu trúc - Gộp Ticket)
Chịu trách nhiệm cho 2 nghiệp vụ cốt lõi:
1. Bán vé: Trừ ghế, tạo Booking (status = CONFIRMED).
2. Hủy vé: Hoàn ghế, đổi trạng thái Booking (status = CANCELLED).
*/
class BookingManager {
private:
    // Mảng động lưu trữ con trỏ tới các đối tượng Booking
    DynamicArray<Booking*> allBookings;

    // --- Hàm trợ giúp nội bộ ---
    void loadBookingsFromFile(const std::string& filePath);

public:
    /*
    Hàm khởi tạo, tự động tải dữ liệu bookings.
    */
    BookingManager(const std::string& bookingsFilePath); // Chỉ còn 1 file path
    
    /*
    Hàm hủy, sẽ dọn dẹp các con trỏ Booking trong DynamicArray
    */
    ~BookingManager();

    /*
    (NGHIỆP VỤ CỐT LÕI) Bán vé (Tạo Booking).
    1. Kiểm tra và trừ ghế từ FlightManager.
    2. Tạo đối tượng Booking (status = CONFIRMED).
    *
    Con trỏ trỏ tới Booking mới nếu thành công, ngược lại là nullptr (nếu hết ghế).
    */
    Booking* createNewBooking( FlightManager& flightManager,
                               const std::string& flightInstanceId,
                               const std::string& passengerId,
                               BookingClass bookingClass,
                               double baseFare);

    /*
    (NGHIỆP VỤ CỐT LÕI) Hủy một Booking.
    1. Tìm Booking, kiểm tra xem có 'CONFIRMED' không.
    2. Hoàn trả ghế về cho FlightManager.
    3. Đổi trạng thái Booking -> CANCELLED.
    *
    bookingId ID của booking (BKG-XXXX) cần hủy.
    true nếu hủy thành công.
    */
    bool cancelBooking(FlightManager& flightManager, const std::string& bookingId);

    // --- Tìm kiếm (Tạm thời dùng Vòng lặp For) ---
    Booking* findBookingById(const std::string& bookingId);
    
    // --- Lấy dữ liệu ---
    const DynamicArray<Booking*>& getAllBookings() const;

    // --- Lưu trữ ---
    bool saveDataToFiles(const std::string& bookingsFilePath) const; // Chỉ còn 1 file path
};

#endif