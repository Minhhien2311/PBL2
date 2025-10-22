#ifndef BOOKING_H
#define BOOKING_H

#include <string>

// Hạng đặt chỗ
enum class BookingClass {
    Economy,
    Business,
};

/*
 * Booking (Đã tái cấu trúc - Siêu đơn giản)
 * Chỉ là một "bản ghi lịch sử" (hóa đơn).
 * Không có trạng thái (status), không có ID ghế (seatId).
 */
class Booking {
private:
    // Định danh & Liên kết
    std::string bookingId;         // Khóa nội bộ duy nhất
    std::string flightInstanceId;  // Chuyến bay cụ thể
    std::string passengerId;       // ID duy nhất của hành khách
    // (Đã xóa seatId)

    // Thông tin
    std::string bookingDate;       // Ngày giờ tạo
    BookingClass bookingClass;

    // Chi tiết giá vé  
    double baseFare;       

public:
    Booking() = default;

    // Constructor chính
    explicit Booking(const std::string& flightInstanceId,
            const std::string& passengerId,
            const std::string& bookingDate,
            BookingClass bookingClass,
            double baseFare); 

    // --- Getters ---
    const std::string& getBookingId() const;
    const std::string& getFlightInstanceId() const;
    const std::string& getPassengerId() const;
    // (Đã xóa getSeatId)
    const std::string& getBookingDate() const;
    BookingClass getClass() const;
    double getBaseFare() const;
    double getTotalAmount() const;

    // --- Setters ---
    void setBaseFare(double value);
    
    // (Đã xóa các hàm Thao tác Ghế)

    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static Booking fromRecordLine(const std::string& line);
    void overrideIdForLoad(const std::string& existingId);
};

#endif // BOOKING_H