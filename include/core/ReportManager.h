// #pragma once

// class AccountManager;
// class BookingManager;

// #include "DSA/DynamicArray.h"   // Cần cho DynamicArray
// #include "entities/Booking.h"   // Cần cho BookingStatus
// #include <string>

// //  * Struct đơn giản để chứa kết quả báo cáo cho mỗi Agent. Dữ liệu này được tạo ra bởi ReportManager.
// struct AgentReport {
//     std::string agentId;
//     std::string agentName;
//     int    totalBookings = 0;       // Tổng số booking đã tạo
//     int    issuedTickets = 0;       // Số booking đã phát hành (thành công)
//     int    cancelledTickets = 0;    // Số booking đã hủy
//     double totalRevenue = 0.0;      // Tổng doanh thu từ các vé đã phát hành
// };

// //  Lớp chịu trách nhiệm tính toán và thống kê dữ liệu từ các manager khác.
// //  Lớp này KHÔNG SỞ HỮU dữ liệu, nó chỉ ĐỌC dữ liệu.
// class ReportManager {
// private:
//     AccountManager& accountManager_;
//     BookingManager& bookingManager_;

// public:
//     ReportManager(AccountManager& am, BookingManager& bm);
//     ~ReportManager() = default;

//     // --- Thống kê Tổng quan (cho Admin) ---
//     double getTotalRevenueAllAgents() const;
//     int getTotalTicketsSold() const;
//     int countBookingsByStatus(BookingStatus status) const;
//     DynamicArray<AgentReport*>* generateFullAgentReport() const;

//     // --- Thống kê theo ngày cho Agent cụ thể ---
//     double getDailyRevenue(const std::string& agentId) const;
//     int getDailyTicketsSold(const std::string& agentId) const;
//     int getDailyCancellations(const std::string& agentId) const;
//     int getDailyTicketChanges(const std::string& agentId) const;

//     // --- Thống kê theo khoảng thời gian ---
//     // Trả về tổng số vé đã bán trong khoảng từ ngàyStart -> ngàyEnd (YYYY-MM-DD)
//     int getTicketsSoldInRange(const std::string& startDate, const std::string& endDate) const;

//     // Trả về số vé theo loại hạng (Thương gia / Phổ thông) trong khoảng thời gian
//     int getTicketsSoldByClassInRange(const std::string& startDate, const std::string& endDate, BookingClass bookingClass) const;

//     // Báo cáo doanh thu theo từng agent trong khoảng thời gian
//     DynamicArray<AgentReport*>* generateAgentReportInRange(const std::string& startDate, const std::string& endDate) const;

//     // Báo cáo doanh thu của 1 agent theo từng tháng trong năm (Agent view)
//     DynamicArray<int>* getMonthlyTicketsByAgent(const std::string& agentId, int year) const;
// };

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
