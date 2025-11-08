#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include <string>
#include <vector>
#include "entities/Booking.h"

class AccountManager;
class BookingManager;
class AccountAgent;

struct AgentReport {
    std::string agentId;
    std::string agentName;
    int totalBookings = 0;
    int issuedTickets = 0;
    int cancelledTickets = 0;
    double totalRevenue = 0.0;
};

struct MonthlyTicketReport {
    std::string month;
    int issuedTickets = 0;
};

class ReportManager {
private:
    AccountManager& accountManager_;
    BookingManager& bookingManager_;

    // Hàm chuyển đổi an toàn
    int safeStoi(const std::string& str, int defaultValue = 0) const;
    // Hàm chuyển đổi định dạng ngày từ dd/mm/yyyy sang yyyy-mm-dd để so sánh
    std::string convertDateFormat(const std::string& date) const;
    // Hàm trích xuất phần ngày từ chuỗi datetime
    std::string extractDatePart(const std::string& datetime) const;
    // Hàm so sánh ngày
    bool isDateInRange(const std::string& bookingDate, const std::string& startDate, const std::string& endDate) const;

public:
    ReportManager(AccountManager& am, BookingManager& bm);

    // --- Tổng quan (Admin) ---
    double getTotalRevenueAllAgents() const;
    int countBookingsByStatus(BookingStatus status) const;
    int getTotalTicketsSold() const;
    std::vector<AgentReport*>* generateFullAgentReport() const;

    // --- Thống kê theo ngày cho Agent ---
    double getDailyRevenue(const std::string& agentId) const;
    int getDailyTicketsSold(const std::string& agentId) const;
    int getDailyCancellations(const std::string& agentId) const;

    // --- Thống kê theo khoảng ngày ---
    int getTicketsSoldInRange(const std::string& startDate, const std::string& endDate) const;
    double getRevenueInRange(const std::string& startDate, const std::string& endDate) const;

    // --- Thống kê theo agent và khoảng ngày ---
    int getTicketsSoldInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;
    double getRevenueInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;

    // --- Báo cáo theo agent và khoảng ngày (Admin view) ---
    std::vector<AgentReport*>* generateAgentReportInRange(const std::string& startDate, const std::string& endDate) const;

    // --- Báo cáo theo tháng (Admin view) ---
    // === SỬA: Trả về by value thay vì con trỏ ===
    std::vector<int> getMonthlyTicketsByAgent(const std::string& agentId, int year) const;

    // --- Các hàm mới cho thống kê doanh thu ---
    double getTodayRevenue() const;
    double getThisWeekRevenue() const;
    double getThisMonthRevenue() const;
    std::vector<double> getMonthlyRevenue(int year) const;

    // --- Thống kê theo loại vé ---
    int getBusinessTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    int getEconomyTicketsInRange(const std::string& startDate, const std::string& endDate) const;

    // --- Các hàm mới cho Agent ---
    // Doanh thu theo thời gian cho agent cụ thể
    double getAgentTodayRevenue(const std::string& agentId) const;
    double getAgentThisWeekRevenue(const std::string& agentId) const;
    double getAgentThisMonthRevenue(const std::string& agentId) const;
    std::vector<double> getAgentMonthlyRevenue(const std::string& agentId, int year) const;

    // Thống kê vé theo loại cho agent cụ thể
    int getAgentBusinessTicketsInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;
    int getAgentEconomyTicketsInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;

    // Báo cáo vé theo tháng cho agent
    std::vector<MonthlyTicketReport*>* generateMonthlyTicketReportForAgent(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;

    // --- Các hàm overload cho agent hiện tại (dùng trong UI) ---
    int getAgentTicketsSoldInRange(const std::string& startDate, const std::string& endDate) const;
    int getAgentBusinessTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    int getAgentEconomyTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    double getAgentTodayRevenue() const;
    double getAgentThisWeekRevenue() const;
    double getAgentThisMonthRevenue() const;
    std::vector<double> getAgentMonthlyRevenue(int year) const;

    // === THÊM MỚI: Hàm overload cho biểu đồ vé ===
    std::vector<int> getAgentMonthlyTickets(int year) const;
};

#endif // REPORT_MANAGER_H