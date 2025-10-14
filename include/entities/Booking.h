#ifndef BOOKING_H
#define BOOKING_H

#include <string>

// Trạng thái đặt chỗ
enum class BookingStatus {
    Confirmed,  // Đã chốt, sẵn sàng xuất vé
    Issued,     // Đã xuất vé
    Cancelled,  // Đã huỷ
};

// Hạng đặt chỗ
enum class BookingClass {
    Economy,
    Business,
};

class Booking {
private:
    // Định danh & Liên kết
    std::string bookingId;         // Khóa nội bộ duy nhất
    std::string pnr;               // Mã locator hiển thị/tra cứu (thường 6 ký tự)
    std::string agentId;           // Đại lý/admin tạo booking
    std::string flightInstanceId;  // Chuyến bay cụ thể
    std::string passengerId;       // ID duy nhất của hành khách
    std::string seatId;            // Ghế đã gán; rỗng nếu chưa gán

    // Trạng thái & Hạng vé
    std::string bookingDate;       // Định dạng YYYYMMDDhhmm
    BookingStatus status;
    BookingClass bookingClass;

    // Chi tiết giá vé  
    double baseFare;               // Giá vé nền (đã bao gồm phụ phí hạng vé)
    double discount;               // Khoản giảm giá
    double totalAmount;            // Tổng tiền cuối cùng

    // Thông tin huỷ vé  
    std::string cancelReason;      // Lý do huỷ (nếu có)

public:
    // Constructors  
    Booking() = default;

    // Constructor chính để tạo một booking mới
    explicit Booking(const std::string& pnr,
            const std::string& agentId,
            const std::string& flightInstanceId,
            const std::string& passengerId,
            const std::string& bookingDate,
            BookingClass bookingClass,
            double baseFare); // baseFare nên được tính bên ngoài và truyền vào

    // Getters (Truy xuất thông tin)  
    const std::string& getBookingId() const;
    const std::string& getPNR() const;
    const std::string& getAgentId() const;
    const std::string& getFlightInstanceId() const;
    const std::string& getPassengerId() const;
    const std::string& getSeatId() const;

    const std::string& getBookingDate() const;
    BookingStatus getStatus() const;
    BookingClass getClass() const;

    double getBaseFare() const;
    double getTotalAmount() const;

    const std::string& getCancelReason() const;

    // Thay đổi thông tin
    void setStatus(BookingStatus newStatus);
    void setCancelReason(const std::string& reason);

    // Các hàm setter cho giá trị nên được dùng cẩn thận,
    // vì chúng có thể ảnh hưởng đến các quy tắc nghiệp vụ.
    void setBaseFare(double value);

    // Quy tắc nghiệp vụ dựa theo rule
    bool canBeCancelled() const; // Logic sẽ ở file .cpp
    bool canBeChanged() const;   // Logic sẽ ở file .cpp

    // Thao tác Ghế  
    // Gán ghế. Trả về false nếu thất bại.
    bool assignSeat(const std::string& seatId);
    // Bỏ gán ghế hiện tại.
    void unassignSeat();

    // --- Đọc/Ghi file ---
    // Chuyển đổi đối tượng thành 1 dòng string để lưu vào file.
    std::string toRecordLine() const;
    
    // Tạo đối tượng Booking từ 1 dòng string đọc từ file.
    static Booking fromRecordLine(const std::string& line);

    // Helper cho việc nạp dữ liệu từ file.
    void overrideIdForLoad(const std::string& existingId);
};

#endif // BOOKING_H