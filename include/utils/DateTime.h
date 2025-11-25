#pragma once

#include <chrono>
#include <string>
#include <ctime>

namespace utils {

class DateTime {
public:
    // --- Lấy thời gian hiện tại ---
    static std::chrono::system_clock::time_point nowUtc();

    // --- Chuyển đổi sang Struct tm ---
    static std::tm toLocalTm(std::chrono::system_clock::time_point tp);
    static std::tm toUtcTm  (std::chrono::system_clock::time_point tp);

    // --- Format sang chuỗi hiển thị ---
    static std::string formatLocal(std::chrono::system_clock::time_point tp,
                                   const char* fmt = "%Y-%m-%d %H:%M:%S");
    static std::string formatUtc  (std::chrono::system_clock::time_point tp,
                                   const char* fmt = "%Y-%m-%dT%H:%M:%SZ");

    // --- Chuyển đổi Unix Timestamp ---
    static std::time_t toUnix(std::chrono::system_clock::time_point tp);
    static std::chrono::system_clock::time_point fromUnix(std::time_t sec);

    // --- Chuyển đổi & Format chuyên biệt (App Logic) ---
    // Parse chuỗi ngày + giờ thành time_point
    static std::chrono::system_clock::time_point 
    fromDmYHm(const std::string& date, const std::string& time);

    // Format time_point thành "DD/MM/YYYY"
    static std::string formatDmY(std::chrono::system_clock::time_point tp);

    // Format time_point thành "HH:MM"
    static std::string formatHm(std::chrono::system_clock::time_point tp);
};

}