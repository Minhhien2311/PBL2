#ifndef DATETIME_H
#define DATETIME_H

#include <chrono>   // Dùng để lấy time_point
#include <string>
#include <ctime>    // Dùng để chuyển đổi struct tm

namespace utils {

class DateTime {
public:
    // Lấy "bây giờ" theo đồng hồ hệ điều hành (UTC time_point).
    static std::chrono::system_clock::time_point nowUtc();

    // Chuyển time_point -> struct tm theo LOCAL timezone/UTC (để tùy ý format)
    static std::tm toLocalTm(std::chrono::system_clock::time_point tp);
    static std::tm toUtcTm  (std::chrono::system_clock::time_point tp);

    // Format sang chuỗi (hiển thị).
    static std::string formatLocal(std::chrono::system_clock::time_point tp,
                                   const char* fmt = "%Y-%m-%d %H:%M:%S");
    static std::string formatUtc  (std::chrono::system_clock::time_point tp,
                                   const char* fmt = "%Y-%m-%dT%H:%M:%SZ");

    // Chuyển đổi giữa time_point và Unix timestamp (số giây từ 01/01/1970).
    static std::time_t toUnix(std::chrono::system_clock::time_point tp);
    static std::chrono::system_clock::time_point fromUnix(std::time_t sec);

    // Phân tách chuỗi ngày "DD/MM/YYYY" và giờ "HH:MM" thành time_point.
    // Giả định đây là giờ địa phương.
    static std::chrono::system_clock::time_point 
    fromDmYHm(const std::string& date, const std::string& time);

    // Định dạng time_point thành chuỗi "DD/MM/YYYY" (Ngày địa phương).
    static std::string formatDmY(std::chrono::system_clock::time_point tp);

    // Định dạng time_point thành chuỗi "HH:MM" (Giờ địa phương).
    static std::string formatHm(std::chrono::system_clock::time_point tp);
};

}

#endif