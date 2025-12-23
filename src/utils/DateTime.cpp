#include "utils/DateTime.h"
#include <sstream>
#include <iomanip>

namespace utils {
namespace {

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
}

std::chrono::system_clock::time_point DateTime::nowUtc() {
    return std::chrono::system_clock::now();
}

std::tm DateTime::toLocalTm(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    return safe_localtime(t);
}
std::tm DateTime::toUtcTm(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    return safe_gmtime(t);
}

std::string DateTime::formatLocal(std::chrono::system_clock::time_point tp,
                              const char* fmt) {
    std::tm tm = toLocalTm(tp);
    std::ostringstream oss;
    oss << std::put_time(&tm, fmt);
    return oss.str();
}

std::string DateTime::formatUtc(std::chrono::system_clock::time_point tp,
                            const char* fmt) {
    std::tm tm = toUtcTm(tp);
    std::ostringstream oss;
    oss << std::put_time(&tm, fmt);
    return oss.str();
}

std::time_t DateTime::toUnix(std::chrono::system_clock::time_point tp) {
    return std::chrono::system_clock::to_time_t(tp);
}
std::chrono::system_clock::time_point DateTime::fromUnix(std::time_t sec) {
    return std::chrono::system_clock::from_time_t(sec);
}

std::chrono::system_clock::time_point 
DateTime::fromDmYHm(const std::string& date, const std::string& time) {
    std::tm tm = {};
    std::string dateTimeStr = date + " " + time;
    std::stringstream ss(dateTimeStr);
    
    ss >> std::get_time(&tm, "%d/%m/%Y %H:%M");
    
    std::time_t tt = std::mktime(&tm);
    
    return std::chrono::system_clock::from_time_t(tt);
}

std::string DateTime::formatDmY(std::chrono::system_clock::time_point tp) {
    return formatLocal(tp, "%d/%m/%Y");
}

std::string DateTime::formatHm(std::chrono::system_clock::time_point tp) {
    return formatLocal(tp, "%H:%M");
}
}
