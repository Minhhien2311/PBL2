#include "entities/Seat.h"
#include <sstream>

// --- Constructors ---

Seat::Seat() 
    : id(""), status(SeatStatus::Available), type(SeatType::Economy) {
}

Seat::Seat(const std::string& id, SeatStatus status, SeatType type)
    : id(id), status(status), type(type) {
}

// --- Getters ---

const std::string& Seat::getId() const {
    return id;
}

SeatStatus Seat::getStatus() const {
    return status;
}

SeatType Seat::getType() const {
    return type;
}

// --- Setters ---

void Seat::setStatus(SeatStatus newStatus) {
    status = newStatus;
}

void Seat::setType(SeatType newType) {
    type = newType;
}

// --- Chuyển đổi tọa độ ---

void Seat::idToCoordinates(const std::string& seatId, int& row, int& col) {
    if (seatId.empty()) {
        row = -1;
        col = -1;
        return;
    }

    // Tách phần số (row) và phần chữ (col)
    size_t i = 0;
    while (i < seatId.length() && std::isdigit(seatId[i])) {
        i++;
    }

    if (i == 0 || i >= seatId.length()) {
        row = -1;
        col = -1;
        return;
    }

    // Parse row number
    row = std::stoi(seatId.substr(0, i));

    // Parse column letter (A=0, B=1, C=2, ...)
    char colChar = seatId[i];
    col = colChar - 'A';
}

std::string Seat::coordinatesToId(int row, int col) {
    if (row < 0 || col < 0 || col > 25) {
        return "";
    }

    // Convert column index to letter
    char colChar = 'A' + col;
    
    return std::to_string(row) + colChar;
}

// --- Đọc/Ghi file ---

std::string Seat::toRecordLine() const {
    std::string statusStr;
    switch (status) {
        case SeatStatus::Available: statusStr = "Available"; break;
        case SeatStatus::Booked:    statusStr = "Booked"; break;
        case SeatStatus::Locked:    statusStr = "Locked"; break;
    }

    std::string typeStr;
    switch (type) {
        case SeatType::Economy:  typeStr = "Economy"; break;
        case SeatType::Business: typeStr = "Business"; break;
    }

    return id + "|" + statusStr + "|" + typeStr;
}

Seat Seat::fromRecordLine(const std::string& line) {
    std::stringstream ss(line);
    std::string idStr, statusStr, typeStr;

    std::getline(ss, idStr, '|');
    std::getline(ss, statusStr, '|');
    std::getline(ss, typeStr, '|');

    // Parse status
    SeatStatus status = SeatStatus::Available;
    if (statusStr == "Booked") {
        status = SeatStatus::Booked;
    } else if (statusStr == "Locked") {
        status = SeatStatus::Locked;
    }

    // Parse type
    SeatType type = SeatType::Economy;
    if (typeStr == "Business") {
        type = SeatType::Business;
    }

    return Seat(idStr, status, type);
}
