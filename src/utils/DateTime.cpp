// Time.cpp — định nghĩa hàm cho utils::Time
#include "utils/DateTime.h"
#include <sstream>   // std::ostringstream
#include <iomanip>   // std::put_time

namespace utils {
namespace {
// ---- helper thread-safe cho chuyển đổi std::time_t -> std::tm ----
// Windows: *_s ; POSIX: *_r ; fallback: không thread-safe (ít khuyến nghị).
inline std::tm safe_localtime(std::time_t t) {
    std::tm out{};
#if defined(_WIN32)
    ::localtime_s(&out, &t);
#elif defined(__unix__) || defined(__APPLE__)
    ::localtime_r(&t, &out);
#else
    out = *std::localtime(&t);
#endif
    return out;
}

inline std::tm safe_gmtime(std::time_t t) {
    std::tm out{};
#if defined(_WIN32)
    ::gmtime_s(&out, &t);
#elif defined(__unix__) || defined(__APPLE__)
    ::gmtime_r(&t, &out);
#else
    out = *std::gmtime(&t);
#endif
    return out;
}
} // namespace

// ----- nowUtc -----
std::chrono::system_clock::time_point DateTime::nowUtc() {
    return std::chrono::system_clock::now();
}

// ----- toLocalTm / toUtcTm -----
std::tm DateTime::toLocalTm(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    return safe_localtime(t);
}
std::tm DateTime::toUtcTm(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    return safe_gmtime(t);
}

// ----- formatLocal / formatUtc -----
std::string DateTime::formatLocal(std::chrono::system_clock::time_point tp,
                              const char* fmt) {
    std::tm tm = toLocalTm(tp);
    std::ostringstream oss;
    oss << std::put_time(&tm, fmt);  // ví dụ: "%Y-%m-%d %H:%M:%S"
    return oss.str();
}

std::string DateTime::formatUtc(std::chrono::system_clock::time_point tp,
                            const char* fmt) {
    std::tm tm = toUtcTm(tp);
    std::ostringstream oss;
    oss << std::put_time(&tm, fmt);  // ví dụ: "%FT%TZ"
    return oss.str();
}

// ----- epoch <-> time_point -----
std::time_t DateTime::toUnix(std::chrono::system_clock::time_point tp) {
    return std::chrono::system_clock::to_time_t(tp);
}
std::chrono::system_clock::time_point DateTime::fromUnix(std::time_t sec) {
    return std::chrono::system_clock::from_time_t(sec);
}

// ----- HÀM MỚI: fromDmYHm -----
std::chrono::system_clock::time_point 
DateTime::fromDmYHm(const std::string& date, const std::string& time) {
    std::tm tm = {};
    std::string dateTimeStr = date + " " + time;
    std::stringstream ss(dateTimeStr);
    
    // Parse chuỗi "DD/MM/YYYY HH:MM"
    ss >> std::get_time(&tm, "%d/%m/%Y %H:%M");
    
    // Chuyển đổi struct tm (giờ địa phương) thành time_t
    std::time_t tt = std::mktime(&tm);
    
    // Chuyển đổi time_t thành time_point
    return std::chrono::system_clock::from_time_t(tt);
}

// ----- HÀM MỚI: formatDmY -----
std::string DateTime::formatDmY(std::chrono::system_clock::time_point tp) {
    // Dùng lại hàm formatLocal đã có với định dạng mới
    return formatLocal(tp, "%d/%m/%Y");
}

// ----- HÀM MỚI: formatHm -----
std::string DateTime::formatHm(std::chrono::system_clock::time_point tp) {
    // Dùng lại hàm formatLocal đã có với định dạng mới
    return formatLocal(tp, "%H:%M");
}

// ----- HÀM MỚI: formatDateForId -----
std::string DateTime::formatDateForId(const std::string& ddmmyyyy) {
    // Chuyển đổi từ "DD/MM/YYYY" sang "YYYYMMDD"
    // Định dạng đầu vào: DD/MM/YYYY (10 ký tự)
    if (ddmmyyyy.length() != 10) {
        return ""; // Định dạng không hợp lệ
    }
    
    // Validate the format has slashes in correct positions
    if (ddmmyyyy[2] != '/' || ddmmyyyy[5] != '/') {
        return ""; // Định dạng không hợp lệ
    }
    
    std::string dd = ddmmyyyy.substr(0, 2);
    std::string mm = ddmmyyyy.substr(3, 2);
    std::string yyyy = ddmmyyyy.substr(6, 4);
    
    return yyyy + mm + dd;
}

} // namespace utils
