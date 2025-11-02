#include "entities/Seat.h"
#include <cctype>

Seat::Seat() : id(""), status(SeatStatus::AVAILABLE), type(SeatType::ECONOMY) {
}

Seat::Seat(const std::string& seatId, SeatType seatType) 
    : id(seatId), status(SeatStatus::AVAILABLE), type(seatType) {
}

void Seat::book() {
    status = SeatStatus::BOOKED;
}

void Seat::release() {
    status = SeatStatus::AVAILABLE;
}

bool Seat::isAvailable() const {
    return status == SeatStatus::AVAILABLE;
}

SeatType Seat::getType() const {
    return type;
}

std::string Seat::getId() const {
    return id;
}

SeatStatus Seat::getStatus() const {
    return status;
}

// Convert seat ID like "B5" to coordinates {row, col}
// Row: A=0, B=1, C=2, etc.
// Col: 1-based to 0-based (5 -> 4)
std::pair<int, int> Seat::idToCoordinates(const std::string& seatId) {
    if (seatId.empty()) {
        return {-1, -1};
    }

    // Extract row letter (e.g., 'B' from "B5")
    char rowChar = std::toupper(seatId[0]);
    int row = rowChar - 'A';

    // Extract column number (e.g., "5" from "B5")
    int col = 0;
    for (size_t i = 1; i < seatId.length(); ++i) {
        if (std::isdigit(seatId[i])) {
            col = col * 10 + (seatId[i] - '0');
        }
    }
    // Convert to 0-based index
    col = col - 1;

    return {row, col};
}

// Convert coordinates {row, col} to seat ID like "B5"
// Row: 0=A, 1=B, 2=C, etc.
// Col: 0-based to 1-based (4 -> 5)
std::string Seat::coordinatesToId(int row, int col) {
    if (row < 0 || col < 0) {
        return "";
    }

    // Convert row to letter
    char rowChar = 'A' + row;
    
    // Convert col to 1-based number
    int colNum = col + 1;

    return std::string(1, rowChar) + std::to_string(colNum);
}
