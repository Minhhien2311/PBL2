#pragma once

#include <string>

enum class BookingClass { Economy, Business };
enum BookingStatus { Issued, Cancelled, Changed };

class Booking {
private:
    // --- Định danh ---
    std::string bookingId;
    std::string flightId;
    std::string agentId;
    std::string passengerId;
    std::string seatID;

    // --- Thông tin vé ---
    std::string bookingDate;
    BookingClass bookingClass;
    int baseFare;       
    BookingStatus status;

public:
    // --- Constructor ---
    Booking() = default;
    explicit Booking(const std::string& flightId, const std::string& agentId,
                     const std::string& passengerId, const std::string& seatID,
                     const std::string& bookingDate, BookingClass bookingClass,
                     int baseFare, BookingStatus status = BookingStatus::Issued); 

    // --- Getters ---
    const std::string& getBookingId() const;
    const std::string& getFlightId() const;
    const std::string& getAgentId() const;
    const std::string& getPassengerId() const;
    const std::string& getSeatID() const;
    const std::string& getBookingDate() const;
    BookingClass getClass() const;
    int getBaseFare() const;
    BookingStatus getStatus() const;

    // --- Setters ---
    void setBaseFare(int value);
    void setStatus(BookingStatus newStatus);
    void setPassengerId(const std::string& newPassengerId);
    void setClass(BookingClass newClass);
    void setSeatId(const std::string& newSeatId);
    void setFlightId(const std::string& newFlightId);

    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static Booking fromRecordLine(const std::string& line);
    void overrideIdForLoad(const std::string& existingId);
};