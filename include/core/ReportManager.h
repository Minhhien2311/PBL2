#pragma once

// --- Khai báo xuôi (Forward Declarations) ---
// Chúng ta chỉ dùng tham chiếu tới các Manager, nên không cần include file .h đầy đủ
class AccountManager;
class BookingManager;

// --- Include các file cần thiết ---
#include "DSA/DynamicArray.h" // Cần cho DynamicArray
#include "entities/Booking.h" // Cần cho BookingStatus
#include <string>

/*
 * Struct đơn giản để chứa kết quả báo cáo cho mỗi Agent.
 * Dữ liệu này được tạo ra bởi ReportManager.
 */
struct AgentReport {
    std::string agentId;
    std::string agentName;
    int    totalBookings = 0; // Tổng số booking đã tạo
    int    issuedTickets = 0; // Số booking đã phát hành (thành công)
    int    cancelledTickets = 0; // Số booking đã hủy
    double totalRevenue = 0.0;   // Tổng doanh thu từ các vé đã phát hành
};

/*
 * ReportManager: Lớp chịu trách nhiệm tính toán và thống kê
 * dữ liệu từ các manager khác.
 * Lớp này KHÔNG SỞ HỮU dữ liệu, nó chỉ ĐỌC dữ liệu.
 */
class ReportManager {
private:
    // Giữ tham chiếu (references) đến các manager chính
    AccountManager& accountManager_;
    BookingManager& bookingManager_;

public:
    // Constructor: Nhận các manager qua tham chiếu (Dependency Injection)
    ReportManager(AccountManager& am, BookingManager& bm);

    // Destructor mặc định là đủ
    ~ReportManager() = default;


    // --- Thống kê Tổng quan (cho Admin) ---

    // Tính tổng doanh thu từ tất cả các booking đã 'Issued'
    double getTotalRevenueAllAgents() const;
    
    // Tính tổng số booking đã bán (trạng thái Issued)
    int getTotalTicketsSold() const;

    // Đếm số lượng booking theo một trạng thái cụ thể
    int countBookingsByStatus(BookingStatus status) const;


    // --- Thống kê theo Agent (cho Admin) ---
    
    // Tạo một báo cáo doanh thu chi tiết cho từng agent.
    // Trả về một con trỏ tới DynamicArray mới. 
    // QUAN TRỌNG: Người gọi có trách nhiệm 'delete' đối tượng mảng này
    // và tất cả các con trỏ AgentReport* bên trong nó sau khi sử dụng xong.
    DynamicArray<AgentReport*>* generateFullAgentReport() const;

    // --- Thống kê theo ngày cho Agent cụ thể ---
    
    // Lấy tổng doanh thu trong ngày của một agent cụ thể
    double getDailyRevenue(const std::string& agentId) const;
    
    // Lấy số vé đã bán trong ngày của một agent cụ thể
    int getDailyTicketsSold(const std::string& agentId) const;
    
    // Lấy số vé đã hủy trong ngày của một agent cụ thể
    int getDailyCancellations(const std::string& agentId) const;
    
    // Lấy số vé đã đổi trong ngày của một agent cụ thể
    // Hiện tại trả về 0 vì chưa có tính năng đổi vé
    int getDailyTicketChanges(const std::string& agentId) const;
};