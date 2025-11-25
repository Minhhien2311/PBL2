#pragma once

#include <string>
#include <vector>
#include <utility>

enum class SeatType { Economy, Business };
enum class SeatStatus { Available, Booked };

class Seat {
private:
    std::string id;
    SeatType type;
    SeatStatus status;

public:
    // --- Constructor ---
    Seat(const std::string& seatId, SeatType seatType = SeatType::Economy, 
         SeatStatus seatStatus = SeatStatus::Available);

    // --- Getters ---
    const std::string& getId() const;
    SeatType getType() const;
    SeatStatus getStatus() const;
    bool isBooked() const;

    // --- Setters ---
    void bookSeat();
    void releaseSeat();
    void setStatus(SeatStatus newStatus);

    // --- Helpers (Tọa độ) ---
    std::pair<int, int> getCoordinates() const; // ID -> (row, col)
    static std::string coordinatesToId(const std::pair<int, int>& coords); // (row, col) -> ID
};