#ifndef SEAT_H
#define SEAT_H

#include <string>
#include <vector>
#include <utility>

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

class Seat {
private:
    std::string id;
    SeatType type;
    SeatStatus status;

public:
    // Constructor
    Seat(const std::string& seatId, SeatType seatType = SeatType::Economy, SeatStatus seatStatus = SeatStatus::Available);

    // --- Getters ---
    const std::string& getId() const;
    SeatType getType() const;
    SeatStatus getStatus() const;
    bool isBooked() const;

    // --- Setters ---
    void bookSeat();
    void releaseSeat();
    void setStatus(SeatStatus newStatus);

    // Chuyển đổi ID ghế sang tọa độ mảng (hàng, cột). Trả về std::pair<int, int> Tọa độ, {-1, -1} nếu ID không hợp lệ.
    std::pair<int, int> getCoordinates() const;

    // Hàm static để chuyển đổi tọa độ thành ID.
    static std::string coordinatesToId(const std::pair<int, int>& coords);
};

#endif