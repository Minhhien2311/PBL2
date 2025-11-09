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
#include <map> 

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
    // Kiểm tra xem định dạng là yyyy-MM-dd hay dd/MM/yyyy
    if (datetime[4] == '-') {
        // Là yyyy-MM-dd... (hoặc yyyy-MM-dd hh:mm:ss)
        // Cần chuyển về dd/MM/yyyy
        std::string year = datetime.substr(0, 4);
        std::string month = datetime.substr(5, 2);
        std::string day = datetime.substr(8, 2);
        return day + "/" + month + "/" + year;
    }
    // Mặc định là dd/MM/yyyy
    return datetime.substr(0, 10); // Lấy dd/mm/yyyy
    }
    return datetime;
}

// Hàm so sánh ngày với định dạng dd/mm/yyyy
bool ReportManager::isDateInRange(const std::string& bookingDateFull,
                    const std::string& startDate,
                    const std::string& endDate) const {

    std::string bookingDay = extractDatePart(bookingDateFull);
    std::string startDay = extractDatePart(startDate);
    std::string endDay = extractDatePart(endDate);

    // Kiểm tra tính hợp lệ cơ bản
    if (bookingDay.length() != 10 || startDay.length() != 10 || endDay.length() != 10) {
    return false; // Không thể so sánh nếu định dạng sai
    }

    std::tm tmBooking{}, tmStart{}, tmEnd{};
    std::istringstream ssBooking(bookingDay);
    std::istringstream ssStart(startDay);
    std::istringstream ssEnd(endDay);

    ssBooking >> std::get_time(&tmBooking, "%d/%m/%Y");
    ssStart >> std::get_time(&tmStart, "%d/%m/%Y");
    ssEnd >> std::get_time(&tmEnd, "%d/%m/%Y");

    if (ssBooking.fail() || ssStart.fail() || ssEnd.fail()) {
    return false; // Phân tích ngày thất bại
    }

    std::time_t tBooking = std::mktime(&tmBooking);
    std::time_t tStart = std::mktime(&tmStart);
    std::time_t tEnd = std::mktime(&tmEnd);

    return difftime(tBooking, tStart) >= 0 && difftime(tEnd, tBooking) >= 0;
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

    // SỬA: Dùng class DateTime của bạn
    std::string today = utils::DateTime::formatDmY(utils::DateTime::nowUtc());

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

    // SỬA: Dùng class DateTime của bạn
    std::string today = utils::DateTime::formatDmY(utils::DateTime::nowUtc());

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

    // SỬA: Dùng class DateTime của bạn
    std::string today = utils::DateTime::formatDmY(utils::DateTime::nowUtc());

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
    // Tối ưu: Chỉ lấy booking của agent đó
    const auto& bookings = bookingManager_.getBookingsByAgentId(agentId);
    for (int i = 0; i < bookings.size(); ++i) {
    Booking* b = bookings[i];
    if (!b) continue;
    // Không cần kiểm tra agentId nữa
    if (b->getStatus() == BookingStatus::Issued &&
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
    // Tối ưu: Chỉ lấy booking của agent đó
    const auto& bookings = bookingManager_.getBookingsByAgentId(agentId);
    for (int i = 0; i < bookings.size(); ++i) {
    Booking* b = bookings[i];
    if (!b) continue;
    // Không cần kiểm tra agentId nữa
    if (b->getStatus() == BookingStatus::Issued &&
        isDateInRange(b->getBookingDate(), startDate, endDate)) {
        total += b->getBaseFare();
    }
    }
    return total;
}

// --- Báo cáo theo agent và khoảng ngày (Admin view) ---
std::vector<AgentReport*>* ReportManager::generateAgentReportInRange(const std::string& startDate,
                                    const std::string& endDate) const {
// ... (Giữ nguyên logic cũ)
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
std::vector<int> ReportManager::getMonthlyTicketsByAgent(const std::string& agentId, int year) const {
    std::vector<int> monthly(12, 0); // Trả về by value

    const auto& bookings = bookingManager_.getBookingsByAgentId(agentId);

    for (int i = 0; i < bookings.size(); ++i) {
    Booking* b = bookings[i];
    if (!b) continue;

    if (b->getStatus() == BookingStatus::Issued) {
        std::string bookingDate = extractDatePart(b->getBookingDate());
        if (bookingDate.length() < 10) continue;

        int bookingYear = safeStoi(bookingDate.substr(6, 4), 0);
        int bookingMonth = safeStoi(bookingDate.substr(3, 2), 0);

        if (bookingYear == year && bookingMonth >= 1 && bookingMonth <= 12) {
        monthly[bookingMonth - 1]++;
        }
    }
    }

    return monthly; // Trả về by value
}

// --- Các hàm mới cho thống kê doanh thu ---
double ReportManager::getTodayRevenue() const {
    // SỬA: Dùng class DateTime của bạn
    std::string today = utils::DateTime::formatDmY(utils::DateTime::nowUtc());
    return getRevenueInRange(today, today);
}

double ReportManager::getThisWeekRevenue() const {
    // SỬA: Dùng class DateTime của bạn để tính toán
    auto now_tp = utils::DateTime::nowUtc();
    std::tm now_tm = utils::DateTime::toLocalTm(now_tp);

    // tm_wday là [0=Chủ Nhật, 1=Thứ 2, ..., 6=Thứ 7]
    int days_since_monday = now_tm.tm_wday - 1;
    if (days_since_monday < 0) days_since_monday = 6; // Chủ nhật (0) -> -1 -> 6

    auto week_start_tp = now_tp - std::chrono::hours(24 * days_since_monday);

    std::string start_date = utils::DateTime::formatDmY(week_start_tp);
    std::string end_date = utils::DateTime::formatDmY(now_tp);

    return getRevenueInRange(start_date, end_date);
}

double ReportManager::getThisMonthRevenue() const {
    // SỬA: Dùng class DateTime của bạn
    auto now_tp = utils::DateTime::nowUtc();
    std::tm now_tm = utils::DateTime::toLocalTm(now_tp);

    // Dùng std::strftime để tạo chuỗi "01/mm/yyyy"
    char start_date_cstr[11];
    std::strftime(start_date_cstr, sizeof(start_date_cstr), "01/%m/%Y", &now_tm);
    std::string start_date(start_date_cstr);

    std::string end_date = utils::DateTime::formatDmY(now_tp);

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

// --- Các hàm mới cho Agent ---

double ReportManager::getAgentTodayRevenue(const std::string& agentId) const {
    // SỬA: Dùng class DateTime của bạn
    std::string today = utils::DateTime::formatDmY(utils::DateTime::nowUtc());
    return getRevenueInRange(agentId, today, today);
}

double ReportManager::getAgentThisWeekRevenue(const std::string& agentId) const {
    // SỬA: Dùng class DateTime của bạn
    auto now_tp = utils::DateTime::nowUtc();
    std::tm now_tm = utils::DateTime::toLocalTm(now_tp);

    int days_since_monday = now_tm.tm_wday - 1;
    if (days_since_monday < 0) days_since_monday = 6; 

    auto week_start_tp = now_tp - std::chrono::hours(24 * days_since_monday);

    std::string start_date = utils::DateTime::formatDmY(week_start_tp);
    std::string end_date = utils::DateTime::formatDmY(now_tp);

    return getRevenueInRange(agentId, start_date, end_date);
}

double ReportManager::getAgentThisMonthRevenue(const std::string& agentId) const {
    // SỬA: Dùng class DateTime của bạn
    auto now_tp = utils::DateTime::nowUtc();
    std::tm now_tm = utils::DateTime::toLocalTm(now_tp);

    char start_date_cstr[11];
    std::strftime(start_date_cstr, sizeof(start_date_cstr), "01/%m/%Y", &now_tm);
    std::string start_date(start_date_cstr);

    std::string end_date = utils::DateTime::formatDmY(now_tp);

    return getRevenueInRange(agentId, start_date, end_date);
}

std::vector<double> ReportManager::getAgentMonthlyRevenue(const std::string& agentId, int year) const {
    std::vector<double> monthlyRevenue(12, 0.0);
    // Tối ưu: Chỉ lấy booking của agent này
    const auto& bookings = bookingManager_.getBookingsByAgentId(agentId);

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

int ReportManager::getAgentBusinessTicketsInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const {
    int count = 0;
    const auto& bookings = bookingManager_.getBookingsByAgentId(agentId);
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

int ReportManager::getAgentEconomyTicketsInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const {
    int count = 0;
    const auto& bookings = bookingManager_.getBookingsByAgentId(agentId);
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

std::vector<MonthlyTicketReport*>* ReportManager::generateMonthlyTicketReportForAgent(const std::string& agentId, const std::string& startDate, const std::string& endDate) const {
    auto* reports = new std::vector<MonthlyTicketReport*>();
    const auto& bookings = bookingManager_.getBookingsByAgentId(agentId);

    // Tạo map để nhóm theo tháng
    std::map<std::string, int> monthlyTickets;

    for (int i = 0; i < bookings.size(); ++i) {
    Booking* b = bookings[i];
    if (!b) continue;

    if (b->getStatus() == BookingStatus::Issued &&
        isDateInRange(b->getBookingDate(), startDate, endDate)) {
        
        std::string bookingDate = extractDatePart(b->getBookingDate());
        if (bookingDate.length() < 10) continue;
        
        // Trích xuất tháng và năm (MM/YYYY)
        std::string month = bookingDate.substr(3, 2) + "/" + bookingDate.substr(6, 4);
        monthlyTickets[month]++;
    }
    }

    // Chuyển đổi map sang vector
    for (const auto& pair : monthlyTickets) {
    auto* report = new MonthlyTicketReport();
    report->month = pair.first;
    report->issuedTickets = pair.second;
    reports->push_back(report);
    }

    return reports;
}

// --- Các hàm overload cho agent hiện tại ---
int ReportManager::getAgentTicketsSoldInRange(const std::string& startDate, const std::string& endDate) const {
    std::string currentAgentId = accountManager_.getCurrentAgentId();
    return getTicketsSoldInRange(currentAgentId, startDate, endDate);
}

int ReportManager::getAgentBusinessTicketsInRange(const std::string& startDate, const std::string& endDate) const {
    std::string currentAgentId = accountManager_.getCurrentAgentId();
    return getAgentBusinessTicketsInRange(currentAgentId, startDate, endDate);
}

int ReportManager::getAgentEconomyTicketsInRange(const std::string& startDate, const std::string& endDate) const {
    std::string currentAgentId = accountManager_.getCurrentAgentId();
    return getAgentEconomyTicketsInRange(currentAgentId, startDate, endDate);
}

double ReportManager::getAgentTodayRevenue() const {
    std::string currentAgentId = accountManager_.getCurrentAgentId();
    return getAgentTodayRevenue(currentAgentId);
}

double ReportManager::getAgentThisWeekRevenue() const {
    std::string currentAgentId = accountManager_.getCurrentAgentId();
    return getAgentThisWeekRevenue(currentAgentId);
}

double ReportManager::getAgentThisMonthRevenue() const {
    std::string currentAgentId = accountManager_.getCurrentAgentId();
    return getAgentThisMonthRevenue(currentAgentId);
}

std::vector<double> ReportManager::getAgentMonthlyRevenue(int year) const {
    std::string currentAgentId = accountManager_.getCurrentAgentId();
    return getAgentMonthlyRevenue(currentAgentId, year);
}

std::vector<int> ReportManager::getAgentMonthlyTickets(int year) const {
    std::string currentAgentId = accountManager_.getCurrentAgentId();
    return getMonthlyTicketsByAgent(currentAgentId, year);
}