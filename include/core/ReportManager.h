#pragma once

#include <vector>
#include "entities/Booking.h"
#include <string>

class AccountManager;
class BookingManager;

// Struct kết quả báo cáo cho từng Agent
struct AgentReport {
    std::string agentId;
    std::string agentName;
    int totalBookings = 0;
    int issuedTickets = 0;
    int cancelledTickets = 0;
    double totalRevenue = 0.0;
};

// Lớp thống kê dữ liệu từ các Manager
class ReportManager {
private:
    AccountManager& accountManager_;
    BookingManager& bookingManager_;

public:
    ReportManager(AccountManager& am, BookingManager& bm);
    ~ReportManager() = default;

    // --- Tổng quan (Admin) ---
    double getTotalRevenueAllAgents() const;
    int getTotalTicketsSold() const;
    int countBookingsByStatus(BookingStatus status) const;
    std::vector<AgentReport*>* generateFullAgentReport() const;

    // --- Thống kê theo ngày cho Agent ---
    double getDailyRevenue(const std::string& agentId) const;
    int getDailyTicketsSold(const std::string& agentId) const;
    int getDailyCancellations(const std::string& agentId) const;

    // --- Thống kê theo khoảng ngày ---
    int getTicketsSoldInRange(const std::string& startDate, const std::string& endDate) const;
    double getRevenueInRange(const std::string& startDate, const std::string& endDate) const;

    // --- Thống kê theo agent và khoảng ngày ---
    int getTicketsSoldInRange(const std::string& agentId,
                              const std::string& startDate,
                              const std::string& endDate) const;

    double getRevenueInRange(const std::string& agentId,
                             const std::string& startDate,
                             const std::string& endDate) const;

    std::vector<AgentReport*>* generateAgentReportInRange(const std::string& startDate,
                                                           const std::string& endDate) const;

    // --- Báo cáo theo tháng (Agent view) ---
    std::vector<int>* getMonthlyTicketsByAgent(const std::string& agentId, int year) const;
};
