#pragma once

#include <string>
#include <vector>
#include "entities/Booking.h"

class AccountManager;
class BookingManager;
class AccountAgent;

// Cấu trúc báo cáo
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
public:
    // --- Constructor ---
    ReportManager(AccountManager& am, BookingManager& bm);

    // ==========================================
    //  GLOBAL STATISTICS (ADMIN / TỔNG QUÁT)
    // ==========================================
    
    // Tổng quan doanh thu & số lượng
    double getTotalRevenueAllAgents() const;
    int getTotalTicketsSold() const;
    int countBookingsByStatus(BookingStatus status) const;

    // Doanh thu theo thời gian thực (Toàn hệ thống)
    double getTodayRevenue() const;
    double getThisWeekRevenue() const;
    double getThisMonthRevenue() const;
    std::vector<double> getMonthlyRevenue(int year) const; // Biểu đồ doanh thu năm

    // Thống kê theo khoảng thời gian (Toàn hệ thống)
    int getTicketsSoldInRange(const std::string& startDate, const std::string& endDate) const;
    double getRevenueInRange(const std::string& startDate, const std::string& endDate) const;
    
    // Thống kê chi tiết loại vé (Toàn hệ thống)
    int getBusinessTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    int getEconomyTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    int getCancelledTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    int getChangedTicketsInRange(const std::string& startDate, const std::string& endDate) const;

    // Báo cáo chi tiết từng Agent (Admin View)
    std::vector<AgentReport*>* generateFullAgentReport() const;
    std::vector<AgentReport*>* generateAgentReportInRange(const std::string& startDate, const std::string& endDate) const;

    // ==========================================
    //  AGENT STATISTICS (CÁ NHÂN HÓA)
    // ==========================================

    // Doanh thu & Vé (Cho 1 Agent cụ thể)
    double getDailyRevenue(const std::string& agentId) const;
    int getDailyTicketsSold(const std::string& agentId) const;
    int getDailyCancellations(const std::string& agentId) const;
    
    // Doanh thu theo thời gian (Cho 1 Agent cụ thể)
    double getAgentTodayRevenue(const std::string& agentId) const;
    double getAgentThisWeekRevenue(const std::string& agentId) const;
    double getAgentThisMonthRevenue(const std::string& agentId) const;
    std::vector<double> getAgentMonthlyRevenue(const std::string& agentId, int year) const;

    // Thống kê theo khoảng ngày (Cho 1 Agent cụ thể)
    int getTicketsSoldInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;
    double getRevenueInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;

    // Thống kê loại vé chi tiết (Cho 1 Agent cụ thể)
    int getAgentBusinessTicketsInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;
    int getAgentEconomyTicketsInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;
    int getAgentCancelledTicketsInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;
    int getAgentChangedTicketsInRange(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;
    
    // Biểu đồ/Báo cáo tháng (Cho 1 Agent cụ thể)
    std::vector<int> getMonthlyTicketsByAgent(const std::string& agentId, int year) const;
    std::vector<MonthlyTicketReport*>* generateMonthlyTicketReportForAgent(const std::string& agentId, const std::string& startDate, const std::string& endDate) const;

    // ==========================================
    //  CURRENT SESSION HELPERS (AGENT UI)
    //  (Tự động dùng ID của user đang login)
    // ==========================================
    double getAgentTodayRevenue() const;
    double getAgentThisWeekRevenue() const;
    double getAgentThisMonthRevenue() const;
    
    int getAgentTicketsSoldInRange(const std::string& startDate, const std::string& endDate) const;
    int getAgentBusinessTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    int getAgentEconomyTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    int getAgentCancelledTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    int getAgentChangedTicketsInRange(const std::string& startDate, const std::string& endDate) const;
    
    std::vector<double> getAgentMonthlyRevenue(int year) const;
    std::vector<int> getAgentMonthlyTickets(int year) const;

private:
    // --- Dependencies ---
    AccountManager& accountManager_;
    BookingManager& bookingManager_;

    // --- Internal Helpers ---
    int safeStoi(const std::string& str, int defaultValue = 0) const;
    std::string convertDateFormat(const std::string& date) const; // dd/mm/yyyy -> yyyy-mm-dd
    std::string extractDatePart(const std::string& datetime) const;
    bool isDateInRange(const std::string& bookingDate, const std::string& startDate, const std::string& endDate) const;
};