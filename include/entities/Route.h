#pragma once

#include <string>

// Tuyến bay cố định (VD: HAN-SGN)
class Route {
private:
    std::string routeId;
    std::string departureAirport;   // Mã IATA đi
    std::string arrivalAirport;     // Mã IATA đến

public:
    // --- Constructor ---
    Route() = delete;
    explicit Route(const std::string& departureIATA, const std::string& arrivalIATA);

    // --- Getters ---
    const std::string& getRouteId() const;
    const std::string& getDepartureAirport() const;
    const std::string& getArrivalAirport() const;

    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static Route fromRecordLine(const std::string& line);
    void overrideIdForLoad(const std::string& existingId);
};