#pragma once

#include <string>

namespace IdGenerator {

    // --- Quản lý trạng thái (Counter) ---
    void loadCountersFromFile(const std::string& filePath);
    void saveCountersToFile(const std::string& filePath);

    // --- Sinh ID cụ thể ---
    // Admin ID: "ADM-..."
    std::string generateAdminId();

    // Agent ID: "AGT-..."
    std::string generateAgentId();

    // Flight ID: "FI-..."
    std::string generateFlightId();

    // Booking ID: "BKG-..."
    std::string generateBookingId();
}