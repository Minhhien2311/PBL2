#ifndef SEAT_H
#define SEAT_H

#include <string>

// Trạng thái ghế
enum class SeatStatus {
    Available,  // Ghế trống
    Booked,     // Ghế đã đặt
    Locked      // Ghế bị khóa (bảo trì, v.v.)
};

// Loại ghế
enum class SeatType {
    Economy,    // Hạng phổ thông
    Business    // Hạng thương gia
};

/**
 * @brief Đại diện cho một ghế ngồi trên chuyến bay
 * 
 * ID ghế có dạng: "1A", "12B", "5C", v.v.
 * - Số: hàng ghế (row)
 * - Chữ: cột ghế (A, B, C, D, E, F)
 */
class Seat {
private:
    std::string id;          // ID ghế (ví dụ: "1A", "12F")
    SeatStatus status;       // Trạng thái hiện tại
    SeatType type;           // Loại ghế (Economy/Business)

public:
    // Constructor mặc định
    Seat();
    
    // Constructor với tham số
    Seat(const std::string& id, SeatStatus status, SeatType type);

    // --- Getters ---
    const std::string& getId() const;
    SeatStatus getStatus() const;
    SeatType getType() const;

    // --- Setters ---
    void setStatus(SeatStatus newStatus);
    void setType(SeatType newType);

    // --- Chuyển đổi tọa độ ---
    // Chuyển ID ghế thành tọa độ (row, col)
    // Ví dụ: "1A" -> (1, 0), "12F" -> (12, 5)
    static void idToCoordinates(const std::string& seatId, int& row, int& col);
    
    // Chuyển tọa độ thành ID ghế
    // Ví dụ: (1, 0) -> "1A", (12, 5) -> "12F"
    static std::string coordinatesToId(int row, int col);

    // --- Đọc/Ghi file ---
    // Chuyển đổi đối tượng thành dòng string để lưu vào file
    // Format: id|status|type
    std::string toRecordLine() const;
    
    // Tạo đối tượng Seat từ dòng string
    static Seat fromRecordLine(const std::string& line);
};

#endif // SEAT_H
