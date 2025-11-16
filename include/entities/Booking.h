#ifndef BOOKING_H
#define BOOKING_H

#include <string>

// Hạng đặt chỗ
enum class BookingClass {
    Economy,
    Business
};

enum BookingStatus {
    Issued,
    Cancelled,
    Changed
};

class Booking {
private:
    // Định danh & Liên kết
    std::string bookingId;          // Khóa nội bộ duy nhất
    std::string flightId;           // Chuyến bay cụ thể
    std::string agentId;            // ID của Agent tạo booking
    std::string passengerId;        // ID duy nhất của hành khác
    std::string seatID;             // ID ghế được đặt

    // Thông tin
    std::string bookingDate;        // Ngày giờ tạo
    BookingClass bookingClass;

    // Chi tiết giá vé  
    int baseFare;       

    // Trạng thái của vé này
    BookingStatus status;

public:
    Booking() = default;

    // Constructor
    explicit Booking(const std::string& flightId,
            const std::string& agentId,
            const std::string& passengerId,
            const std::string& seatID,
            const std::string& bookingDate,
            BookingClass bookingClass,
            int baseFare, BookingStatus status = BookingStatus::Issued); 

    // --- Getters ---
    const std::string& getBookingId() const;
    const std::string& getFlightId() const;
    const std::string& getAgentId() const;
    const std::string& getPassengerId() const;
    const std::string& getSeatID() const;
    const std::string& getBookingDate() const;
    BookingClass getClass() const;
    int getBaseFare() const;
    BookingStatus getStatus() const;

    // --- Setters ---
    void setBaseFare(int value);
    void setStatus(BookingStatus newStatus);
    void setPassengerId(const std::string& newPassengerId);
    void setClass(BookingClass newClass);
    void setSeatId(const std::string& newSeatId);
    void setFlightId(const std::string& newFlightId);

    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static Booking fromRecordLine(const std::string& line);
    void overrideIdForLoad(const std::string& existingId);
};

#endif // BOOKING_H