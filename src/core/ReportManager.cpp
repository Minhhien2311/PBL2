#include "core/ReportManager.h"

// Include các file header đầy đủ của các manager và entities cần thiết
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/AccountAgent.h"
#include "entities/Booking.h"


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
        //     if (!booking) continue; // Bỏ qua nếu con trỏ booking không hợp lệ

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