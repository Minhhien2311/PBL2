#include "core/ReportManager.h"

// Include các file header đầy đủ của các manager và entities cần thiết
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/AccountAgent.h"
#include "entities/Booking.h"
#include "utils/DateTime.h"


// Constructor: Gán các tham chiếu
ReportManager::ReportManager(AccountManager& am, BookingManager& bm)
    : accountManager_(am), bookingManager_(bm) {
    // Để trống
}

// Tính tổng doanh thu từ tất cả các booking đã 'Issued'
double ReportManager::getTotalRevenueAllAgents() const {
    double total = 0.0;
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < bookings.size(); ++i) {
        // Chỉ tính doanh thu cho các vé đã phát hành (Issued)
        if (bookings[i]->getStatus() == BookingStatus::Issued) {
            total += bookings[i]->getBaseFare();
        }
    }
    return total;
}

// Đếm số lượng booking theo trạng thái
int ReportManager::countBookingsByStatus(BookingStatus status) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < bookings.size(); ++i) {
        if (bookings[i]->getStatus() == status) {
            count++;
        }
    }
    return count;
}

// Tính tổng số vé đã 'Issued' (bằng cách gọi lại hàm đếm)
int ReportManager::getTotalTicketsSold() const {
    return this->countBookingsByStatus(BookingStatus::Issued);
}

// Tạo một báo cáo doanh thu chi tiết cho từng agent
DynamicArray<AgentReport*>* ReportManager::generateFullAgentReport() const {
    auto* reportList = new DynamicArray<AgentReport*>();
    const auto& agents = accountManager_.getAllAgents();
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < agents.size(); ++i) {
        AccountAgent* agent = agents[i];
        if (!agent) continue;
        
        auto* agentReport = new AgentReport();
        agentReport->agentId = agent->getId();
        agentReport->agentName = agent->getFullName();

        // for (int j = 0; j < bookings.size(); ++j) {
        //     Booking* booking = bookings[j];
        //     if (!booking) continue;

        //     // Kiểm tra xem booking có thuộc agent này không
        //     if (booking->getAgentId() == agent->getId()) {
        //         agentReport->totalBookings++; // Đếm tất cả booking (cả hủy)
                
        //         if (booking->getStatus() == BookingStatus::Issued) {
        //             // Chỉ tính vé đã phát hành
        //             agentReport->issuedTickets++;
        //             agentReport->totalRevenue += booking->getBaseFare();
        //         } else if (booking->getStatus() == BookingStatus::Cancelled) {
        //             // Đếm vé đã hủy
        //             agentReport->cancelledTickets++;
        //         }
        //     }
        // }

        reportList->push_back(agentReport);
    }

    return reportList;
}

// Helper function to check if booking date matches today
namespace {
    bool isToday(const std::string& bookingDate) {
        // bookingDate format: "YYYY-MM-DD HH:MM:SS" or "DD/MM/YYYY HH:MM:SS"
        // Get today's date
        std::string today = utils::DateTime::formatLocal(
            utils::DateTime::nowUtc(), 
            "%Y-%m-%d"
        );
        
        // Check if bookingDate starts with today's date
        // Handle both formats
        if (bookingDate.find(today) == 0) {
            return true;
        }
        
        // Also check DD/MM/YYYY format
        std::string todayDMY = utils::DateTime::formatDmY(utils::DateTime::nowUtc());
        if (bookingDate.find(todayDMY) == 0) {
            return true;
        }
        
        return false;
    }
}

// Lấy tổng doanh thu trong ngày của một agent cụ thể
double ReportManager::getDailyRevenue(const std::string& agentId) const {
    double total = 0.0;
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* booking = bookings[i];
        if (!booking) continue;
        
        // Chỉ tính booking của agent này, đã phát hành, và trong ngày hôm nay
        if (booking->getAgentId() == agentId && 
            booking->getStatus() == BookingStatus::Issued &&
            isToday(booking->getBookingDate())) {
            total += booking->getBaseFare();
        }
    }
    return total;
}

// Lấy số vé đã bán trong ngày của một agent cụ thể
int ReportManager::getDailyTicketsSold(const std::string& agentId) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* booking = bookings[i];
        if (!booking) continue;
        
        // Chỉ đếm booking của agent này, đã phát hành, và trong ngày hôm nay
        if (booking->getAgentId() == agentId && 
            booking->getStatus() == BookingStatus::Issued &&
            isToday(booking->getBookingDate())) {
            count++;
        }
    }
    return count;
}

// Lấy số vé đã hủy trong ngày của một agent cụ thể
int ReportManager::getDailyCancellations(const std::string& agentId) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* booking = bookings[i];
        if (!booking) continue;
        
        // Chỉ đếm booking của agent này, đã hủy, và trong ngày hôm nay
        if (booking->getAgentId() == agentId && 
            booking->getStatus() == BookingStatus::Cancelled &&
            isToday(booking->getBookingDate())) {
            count++;
        }
    }
    return count;
}

// Lấy số vé đã đổi trong ngày của một agent cụ thể
// Hiện tại trả về 0 vì chưa có tính năng đổi vé
int ReportManager::getDailyTicketChanges(const std::string& agentId) const {
    // Chưa có tính năng đổi vé, luôn trả về 0
    return 0;
}