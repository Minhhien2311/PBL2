#pragma once

#include <string>

// Chuyến bay cụ thể (Flight Instance)
class Flight {
private:
    std::string flightId;       // ID duy nhất
    std::string routeId;        // Tham chiếu Route
    std::string airline;        // Hãng bay
    std::string flightNumber;   // Số hiệu (VN123)
    
    // Thời gian
    std::string departureDate;  // DD/MM/YYYY
    std::string departureTime;  // HH:MM
    std::string arrivalDate;
    std::string arrivalTime;

    // Sức chứa & Giá
    int availableSeats;
    int totalCapacity;
    int fareEconomy;
    int fareBusiness;

public:
    // --- Constructor ---
    Flight() = delete;
    explicit Flight(const std::string& routeId, const std::string& airline,
                    const std::string& flightNumber, const std::string& departureDate,
                    const std::string& departureTime, const std::string& arrivalDate,
                    const std::string& arrivalTime, int totalCapacity,
                    int availableSeats, int fareEconomy, int fareBusiness);

    // --- Getters ---
    const std::string& getFlightId() const;
    const std::string& getRouteId() const;
    const std::string& getAirline() const;
    const std::string& getFlightNumber() const;
    const std::string& getDepartureDate() const;
    const std::string& getDepartureTime() const;
    const std::string& getArrivalDate() const;
    const std::string& getArrivalTime() const;
    
    int getAvailableSeats() const;
    int getTotalCapacity() const;
    double getFareEconomy() const;
    double getFareBusiness() const;

    // --- Setters ---
    void setFareEconomy(double fare);
    void setFareBusiness(double fare);
    void setAirline(const std::string& airline);
    void setAvailableSeats(int seats);

    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static Flight fromRecordLine(const std::string& line);
    void overrideIdForLoad(const std::string& existingId);
};