#pragma once

#include "DSA/SimpleMap.h"
#include <vector>
#include <string>

class FlightManager;

// Quản lý ánh xạ: Display Name (Hà Nội (HAN)) <-> IATA Code (HAN)
class AirportManager {
public:
    // --- Constructor & Destructor ---
    explicit AirportManager(const std::string& airportsFilePath);
    ~AirportManager() = default;

    // --- Management (Quản lý) ---
    void addAirport(const std::string& displayName, const std::string& iataCode);
    void loadAirportsFromRoutes(FlightManager* flightManager); // Auto-add từ chuyến bay

    // --- Lookup & Conversion (Tra cứu) ---
    std::string getIATACode(const std::string& displayName) const;
    std::string getDisplayName(const std::string& iataCode) const;
    std::vector<std::string> getAllDisplayNames() const; // Sorted alphabetically

    // --- Utils ---
    bool hasAirport(const std::string& iataCode) const;
    int getAirportCount() const;

private:
    // --- Data Storage ---
    SimpleMap<std::string, std::string> displayToIATA_;
    SimpleMap<std::string, std::string> iataToDisplay_;

    // --- Internal Helpers ---
    void loadAirportsFromFile(const std::string& filePath);
};