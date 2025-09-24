// Time.cpp — định nghĩa hàm cho utils::Time
#include "utils/Time.h"
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
std::chrono::system_clock::time_point Time::nowUtc() {
    return std::chrono::system_clock::now();
}

// ----- toLocalTm / toUtcTm -----
std::tm Time::toLocalTm(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    return safe_localtime(t);
}
std::tm Time::toUtcTm(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    return safe_gmtime(t);
}

// ----- formatLocal / formatUtc -----
std::string Time::formatLocal(std::chrono::system_clock::time_point tp,
                              const char* fmt) {
    std::tm tm = toLocalTm(tp);
    std::ostringstream oss;
    oss << std::put_time(&tm, fmt);  // ví dụ: "%Y-%m-%d %H:%M:%S"
    return oss.str();
}

std::string Time::formatUtc(std::chrono::system_clock::time_point tp,
                            const char* fmt) {
    std::tm tm = toUtcTm(tp);
    std::ostringstream oss;
    oss << std::put_time(&tm, fmt);  // ví dụ: "%FT%TZ"
    return oss.str();
}

// ----- epoch <-> time_point -----
std::time_t Time::toUnix(std::chrono::system_clock::time_point tp) {
    return std::chrono::system_clock::to_time_t(tp);
}
std::chrono::system_clock::time_point Time::fromUnix(std::time_t sec) {
    return std::chrono::system_clock::from_time_t(sec);
}

} // namespace utils
