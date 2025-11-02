#ifndef SEAT_H
#define SEAT_H

#include <string>
#include <utility>

// Enum for seat status
enum class SeatStatus {
    AVAILABLE,
    BOOKED
};

// Enum for seat type
enum class SeatType {
    ECONOMY,
    BUSINESS
};

class Seat {
private:
    std::string id;
    SeatStatus status;
    SeatType type;

public:
    // Constructors
    Seat(); // Default constructor
    Seat(const std::string& seatId, SeatType seatType);

    // Booking operations
    void book();
    void release();
    bool isAvailable() const;

    // Getters
    SeatType getType() const;
    std::string getId() const;
    SeatStatus getStatus() const;

    // Static utility methods for coordinate conversion
    // Example: "B5" -> {1, 4} (row B is index 1, column 5 is index 4)
    static std::pair<int, int> idToCoordinates(const std::string& seatId);
    
    // Example: {1, 4} -> "B5"
    static std::string coordinatesToId(int row, int col);
};

#endif // SEAT_H
