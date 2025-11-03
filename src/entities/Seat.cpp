#include "entities/Seat.h"
#include <cctype>
#include <stdexcept>

// === Triển khai lớp Seat ===

Seat::Seat(const std::string& seatId, SeatType seatType, SeatStatus seatStatus)
    : id(seatId), type(seatType), status(seatStatus) {}

const std::string& Seat::getId() const {
    return id;
}

SeatType Seat::getType() const {
    return type;
}

SeatStatus Seat::getStatus() const {
    return status;
}

bool Seat::isBooked() const {
    return status == SeatStatus::Booked;
}

void Seat::bookSeat() {
    status = SeatStatus::Booked;
}

void Seat::releaseSeat() {
    status = SeatStatus::Available;
}

void Seat::setStatus(SeatStatus newStatus) {
    status = newStatus;
}

std::pair<int, int> Seat::getCoordinates() const {
    if (id.empty() || !std::isalpha(id[0])) {
        return {-1, -1}; // ID không hợp lệ
    }

    int row = std::toupper(id[0]) - 'A';
    
    try {
        int col = std::stoi(id.substr(1)) - 1;
        if (col < 0) return {-1, -1};
        return {row, col};
    } catch (const std::exception&) {
        return {-1, -1}; // Lỗi chuyển đổi
    }
}

std::string Seat::coordinatesToId(const std::pair<int, int>& coords) {
    char rowChar = 'A' + coords.first;
    return rowChar + std::to_string(coords.second + 1);
}