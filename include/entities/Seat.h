#ifndef SEAT_H
#define SEAT_H

#include <string>
#include <vector>
#include <utility> // Cho std::pair

// Loại ghế
enum class SeatType {
    Economy,
    Business
};

// Trạng thái của ghế
enum class SeatStatus {
    Available,
    Booked,
    Locked
};

/**
 * @class Seat
 * @brief Đại diện cho một ghế đơn lẻ trong một chuyến bay. (Đã đơn giản hóa)
 * 
 * Lớp này chỉ chứa thông tin cơ bản về một ghế: ID và trạng thái (đã đặt/còn trống).
 */
class Seat {
private:
    std::string id;
    SeatType type;
    SeatStatus status;

public:
    // Constructor with type support
    Seat(const std::string& seatId, SeatType seatType = SeatType::Economy, SeatStatus seatStatus = SeatStatus::Available);

    // Getters
    const std::string& getId() const;
    SeatType getType() const;
    SeatStatus getStatus() const;
    bool isBooked() const;

    // Setters - Các thao tác cơ bản trên ghế
    void bookSeat();
    void releaseSeat();
    void setStatus(SeatStatus newStatus);

    /**
     * @brief Chuyển đổi ID ghế sang tọa độ mảng (hàng, cột).
     * @return std::pair<int, int> Tọa độ. Trả về {-1, -1} nếu ID không hợp lệ.
     */
    std::pair<int, int> getCoordinates() const;

    /**
     * @brief Hàm static để chuyển đổi tọa độ thành ID.
     */
    static std::string coordinatesToId(const std::pair<int, int>& coords);
};

#endif // SEAT_H