#ifndef TIME_H
#define TIME_H

#include <chrono>   // std::chrono::system_clock::time_point
#include <string>   // std::string
#include <ctime>    // std::tm

namespace utils {

class Time {
public:
    // Lấy "bây giờ" theo đồng hồ hệ điều hành (UTC time_point).
    // Dùng cho logic tính hạn, so sánh thời điểm, v.v.
    static std::chrono::system_clock::time_point nowUtc();

    // Chuyển time_point -> struct tm theo LOCAL timezone/UTC (để tùy ý format)
    static std::tm toLocalTm(std::chrono::system_clock::time_point tp);
    static std::tm toUtcTm  (std::chrono::system_clock::time_point tp);

    // Format sang chuỗi (hiển thị). Token giống strftime:
    // %Y-%m-%d %H:%M:%S, %d/%m/%Y, %FT%TZ, ...
    static std::string formatLocal(std::chrono::system_clock::time_point tp,
                                   const char* fmt = "%Y-%m-%d %H:%M:%S");
    static std::string formatUtc  (std::chrono::system_clock::time_point tp,
                                   const char* fmt = "%Y-%m-%dT%H:%M:%SZ");

    // Chuyển đổi tiện ích epoch seconds <-> time_point (nếu cần lưu file .txt)
    static std::time_t toUnix(std::chrono::system_clock::time_point tp);
    static std::chrono::system_clock::time_point fromUnix(std::time_t sec);

    // --- HÀM MỚI ĐỂ HỖ TRỢ ĐỊNH DẠNG MỚI ---

    // Chuyển đổi chuỗi ngày (DD/MM/YYYY) và giờ (HH:MM) thành time_point.
    // Giả định đây là giờ địa phương.
    static std::chrono::system_clock::time_point 
    fromDmYHm(const std::string& date, const std::string& time);

    // Định dạng time_point thành chuỗi "DD/MM/YYYY" (Giờ địa phương).
    static std::string formatDmY(std::chrono::system_clock::time_point tp);

    // Định dạng time_point thành chuỗi "HH:MM" (Giờ địa phương).
    static std::string formatHm(std::chrono::system_clock::time_point tp);
};

}

#endif