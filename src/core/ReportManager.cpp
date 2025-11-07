#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/AccountAgent.h"
#include "entities/Booking.h"
#include "utils/DateTime.h"
#include <chrono>
#include <ctime>
#include <vector>
#include <cctype>
#include <sstream>
#include <iomanip>

// Constructor
ReportManager::ReportManager(AccountManager& am, BookingManager& bm)
    : accountManager_(am), bookingManager_(bm) {}

// Hàm chuyển đổi an toàn
int ReportManager::safeStoi(const std::string& str, int defaultValue) const {
    if (str.empty()) return defaultValue;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return defaultValue;
        }
    }
    
    return std::atoi(str.c_str());
}

// Hàm chuyển đổi định dạng ngày từ dd/mm/yyyy sang yyyy-mm-dd
std::string ReportManager::convertDateFormat(const std::string& date) const {
    if (date.length() < 10) return date;
    
    std::string day = date.substr(0, 2);
    std::string month = date.substr(3, 2);
    std::string year = date.substr(6, 4);
    
    return year + "-" + month + "-" + day;
}

// Hàm trích xuất phần ngày từ chuỗi datetime (dd/mm/yyyy HH:MM:SS)
std::string ReportManager::extractDatePart(const std::string& datetime) const {
    if (datetime.length() >= 10) {
        return datetime.substr(0, 10); // Lấy dd/mm/yyyy
    }
    return datetime;
}

// Hàm so sánh ngày với định dạng dd/mm/yyyy
bool ReportManager::isDateInRange(const std::string& bookingDate, const std::string& startDate, const std::string& endDate) const {
    std::string bookingDateStr = extractDatePart(bookingDate);
    std::string bookingYMD = convertDateFormat(bookingDateStr);
    std::string startYMD = convertDateFormat(startDate);
    std::string endYMD = convertDateFormat(endDate);
    
    return (bookingYMD >= startYMD && bookingYMD <= endYMD);
}

// --- Tổng quan (Admin) ---
double ReportManager::getTotalRevenueAllAgents() const {
    double total = 0.0;
    const auto& bookings = bookingManager_.getAllBookings();
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getStatus() == BookingStatus::Issued)
            total += b->getBaseFare();
    }
    return total;
}

int ReportManager::countBookingsByStatus(BookingStatus status) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getStatus() == status)
            count++;
    }
    return count;
}

int ReportManager::getTotalTicketsSold() const {
    return countBookingsByStatus(BookingStatus::Issued);
}

std::vector<AgentReport*>* ReportManager::generateFullAgentReport() const {
    auto* reports = new std::vector<AgentReport*>();
    const auto& agents = accountManager_.getAllAgents();
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < agents.size(); ++i) {
        AccountAgent* agent = agents[i];
        if (!agent) continue;

        auto* r = new AgentReport();
        r->agentId = agent->getId();
        r->agentName = agent->getFullName();

        for (int j = 0; j < bookings.size(); ++j) {
            Booking* b = bookings[j];
            if (!b) continue;
            if (b->getAgentId() == agent->getId()) {
                r->totalBookings++;
                if (b->getStatus() == BookingStatus::Issued) {
                    r->issuedTickets++;
                    r->totalRevenue += b->getBaseFare();
                } else if (b->getStatus() == BookingStatus::Cancelled) {
                    r->cancelledTickets++;
                }
            }
        }

        reports->push_back(r);
    }

    return reports;
}

// --- Thống kê theo ngày cho Agent ---
double ReportManager::getDailyRevenue(const std::string& agentId) const {
    double total = 0.0;
    const auto& bookings = bookingManager_.getAllBookings();
    
    // Lấy ngày hôm nay định dạng dd/mm/yyyy
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    
    char today[11];
    std::strftime(today, sizeof(today), "%d/%m/%Y", &now_tm);

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        
        std::string bookingDate = extractDatePart(b->getBookingDate());
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Issued &&
            bookingDate == today) {
            total += b->getBaseFare();
        }
    }
    return total;
}

int ReportManager::getDailyTicketsSold(const std::string& agentId) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    
    // Lấy ngày hôm nay định dạng dd/mm/yyyy
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    
    char today[11];
    std::strftime(today, sizeof(today), "%d/%m/%Y", &now_tm);

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        
        std::string bookingDate = extractDatePart(b->getBookingDate());
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Issued &&
            bookingDate == today) {
            count++;
        }
    }
    return count;
}

int ReportManager::getDailyCancellations(const std::string& agentId) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    
    // Lấy ngày hôm nay định dạng dd/mm/yyyy
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    
    char today[11];
    std::strftime(today, sizeof(today), "%d/%m/%Y", &now_tm);

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        
        std::string bookingDate = extractDatePart(b->getBookingDate());
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Cancelled &&
            bookingDate == today) {
            count++;
        }
    }
    return count;
}

// --- Thống kê theo khoảng ngày ---
int ReportManager::getTicketsSoldInRange(const std::string& startDate,
                                         const std::string& endDate) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getStatus() == BookingStatus::Issued &&
            isDateInRange(b->getBookingDate(), startDate, endDate)) {
            count++;
        }
    }
    return count;
}

double ReportManager::getRevenueInRange(const std::string& startDate,
                                        const std::string& endDate) const {
    double total = 0.0;
    const auto& bookings = bookingManager_.getAllBookings();
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getStatus() == BookingStatus::Issued &&
            isDateInRange(b->getBookingDate(), startDate, endDate)) {
            total += b->getBaseFare();
        }
    }
    return total;
}

// --- Thống kê theo agent và khoảng ngày ---
int ReportManager::getTicketsSoldInRange(const std::string& agentId,
                                         const std::string& startDate,
                                         const std::string& endDate) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Issued &&
            isDateInRange(b->getBookingDate(), startDate, endDate)) {
            count++;
        }
    }
    return count;
}

double ReportManager::getRevenueInRange(const std::string& agentId,
                                        const std::string& startDate,
                                        const std::string& endDate) const {
    double total = 0.0;
    const auto& bookings = bookingManager_.getAllBookings();
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Issued &&
            isDateInRange(b->getBookingDate(), startDate, endDate)) {
            total += b->getBaseFare();
        }
    }
    return total;
}

// --- Báo cáo theo agent và khoảng ngày (Admin view) ---
std::vector<AgentReport*>* ReportManager::generateAgentReportInRange(const std::string& startDate,
                                                                      const std::string& endDate) const {
    auto* reports = new std::vector<AgentReport*>();
    const auto& agents = accountManager_.getAllAgents();
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < agents.size(); ++i) {
        AccountAgent* agent = agents[i];
        if (!agent) continue;

        auto* r = new AgentReport();
        r->agentId = agent->getId();
        r->agentName = agent->getFullName();

        for (int j = 0; j < bookings.size(); ++j) {
            Booking* b = bookings[j];
            if (!b) continue;
            if (b->getAgentId() == agent->getId() &&
                isDateInRange(b->getBookingDate(), startDate, endDate)) {
                r->totalBookings++;
                if (b->getStatus() == BookingStatus::Issued) {
                    r->issuedTickets++;
                    r->totalRevenue += b->getBaseFare();
                } else if (b->getStatus() == BookingStatus::Cancelled) {
                    r->cancelledTickets++;
                }
            }
        }

        reports->push_back(r);
    }

    return reports;
}

// --- Báo cáo theo tháng (Agent view) ---
std::vector<int>* ReportManager::getMonthlyTicketsByAgent(const std::string& agentId, int year) const {
    auto* monthly = new std::vector<int>(12, 0);
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        
        std::string bookingDate = extractDatePart(b->getBookingDate());
        if (bookingDate.length() < 10) continue;
            
        // Trích xuất năm và tháng từ định dạng dd/mm/yyyy
        int bookingYear = safeStoi(bookingDate.substr(6, 4), 0);
        int bookingMonth = safeStoi(bookingDate.substr(3, 2), 0);
        
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Issued &&
            bookingYear == year && 
            bookingMonth >= 1 && bookingMonth <= 12) {
            (*monthly)[bookingMonth - 1]++;
        }
    }

    return monthly;
}

// --- Các hàm mới cho thống kê doanh thu ---
double ReportManager::getTodayRevenue() const {
    // Lấy ngày hôm nay định dạng dd/mm/yyyy
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    
    char today[11];
    std::strftime(today, sizeof(today), "%d/%m/%Y", &now_tm);
    
    return getRevenueInRange(today, today);
}

double ReportManager::getThisWeekRevenue() const {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    
    // Tìm ngày đầu tuần (thứ 2)
    int days_since_monday = now_tm.tm_wday - 1;
    if (days_since_monday < 0) days_since_monday = 6; // Chủ nhật
    
    auto week_start = now - std::chrono::hours(24 * days_since_monday);
    auto week_start_time_t = std::chrono::system_clock::to_time_t(week_start);
    std::tm week_start_tm = *std::localtime(&week_start_time_t);
    
    char start_date[11];
    std::strftime(start_date, sizeof(start_date), "%d/%m/%Y", &week_start_tm);
    char end_date[11];
    std::strftime(end_date, sizeof(end_date), "%d/%m/%Y", &now_tm);
    
    return getRevenueInRange(start_date, end_date);
}

double ReportManager::getThisMonthRevenue() const {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);
    
    char start_date[11];
    std::strftime(start_date, sizeof(start_date), "01/%m/%Y", &now_tm);
    char end_date[11];
    std::strftime(end_date, sizeof(end_date), "%d/%m/%Y", &now_tm);
    
    return getRevenueInRange(start_date, end_date);
}

std::vector<double> ReportManager::getMonthlyRevenue(int year) const {
    std::vector<double> monthlyRevenue(12, 0.0);
    const auto& bookings = bookingManager_.getAllBookings();
    
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        
        if (b->getStatus() == BookingStatus::Issued) {
            std::string bookingDate = extractDatePart(b->getBookingDate());
            if (bookingDate.length() < 10) continue;
            
            int bookingYear = safeStoi(bookingDate.substr(6, 4), 0);
            int bookingMonth = safeStoi(bookingDate.substr(3, 2), 0);
            
            if (bookingYear == year && bookingMonth >= 1 && bookingMonth <= 12) {
                monthlyRevenue[bookingMonth - 1] += b->getBaseFare();
            }
        }
    }
    
    return monthlyRevenue;
}

// --- Thống kê theo loại vé ---
int ReportManager::getBusinessTicketsInRange(const std::string& startDate, const std::string& endDate) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getStatus() == BookingStatus::Issued &&
            b->getClass() == BookingClass::Business &&
            isDateInRange(b->getBookingDate(), startDate, endDate)) {
            count++;
        }
    }
    return count;
}

int ReportManager::getEconomyTicketsInRange(const std::string& startDate, const std::string& endDate) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getStatus() == BookingStatus::Issued &&
            b->getClass() == BookingClass::Economy &&
            isDateInRange(b->getBookingDate(), startDate, endDate)) {
            count++;
        }
    }
    return count;
}