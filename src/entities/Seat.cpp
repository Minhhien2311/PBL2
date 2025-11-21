#include "entities/Seat.h"
#include <cctype>
#include <stdexcept>

#include <iostream>

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
        return {-1, -1}; // Invalid ID
    }

    // Seat ID format: {COL Letter}{ROW Number} e.g., A1, B10, H25
    // Parse: First character = Column (A=0, B=1...), Number = Row (1-based)
    int col = std::toupper(id[0]) - 'A';  // Column from letter (0-based)
    
    try {
        std::string numPart = id.substr(1);
        
        int row = std::stoi(numPart) - 1;  // Row from number (convert to 0-based)
        
        if (row < 0 || col < 0) return {-1, -1};
        return {row, col};
    } catch (const std::exception& e) {
        // Nếu lỗi parse số, trả về invalid coordinate mà không in lỗi ra console
        return {-1, -1}; 
    }
}
std::string Seat::coordinatesToId(const std::pair<int, int>& coords) {
    char colChar = 'A' + coords.first;
    return colChar + std::to_string(coords.second + 1);
}