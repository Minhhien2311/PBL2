// #include "core/ReportManager.h"

// // Include các file header đầy đủ của các manager và entities cần thiết
// #include "core/AccountManager.h"
// #include "core/BookingManager.h"
// #include "entities/AccountAgent.h"
// #include "entities/Booking.h"
// #include "utils/DateTime.h"

// // Constructor: Gán các tham chiếu
// ReportManager::ReportManager(AccountManager& am, BookingManager& bm)
//     : accountManager_(am), bookingManager_(bm) {
// }

// // --- Tổng quan ---
// double ReportManager::getTotalRevenueAllAgents() const {
//     double total = 0.0;
//     const auto& bookings = bookingManager_.getAllBookings();
//     for (int i = 0; i < bookings.size(); ++i) {
//         if (bookings[i]->getStatus() == BookingStatus::Issued) {
//             total += bookings[i]->getBaseFare();
//         }
//     }
//     return total;
// }

// int ReportManager::countBookingsByStatus(BookingStatus status) const {
//     int count = 0;
//     const auto& bookings = bookingManager_.getAllBookings();
//     for (int i = 0; i < bookings.size(); ++i) {
//         if (bookings[i]->getStatus() == status) count++;
//     }
//     return count;
// }

// int ReportManager::getTotalTicketsSold() const {
//     return this->countBookingsByStatus(BookingStatus::Issued);
// }

// DynamicArray<AgentReport*>* ReportManager::generateFullAgentReport() const {
//     auto* reportList = new DynamicArray<AgentReport*>();
//     const auto& agents = accountManager_.getAllAgents();
//     const auto& bookings = bookingManager_.getAllBookings();

//     for (int i = 0; i < agents.size(); ++i) {
//         AccountAgent* agent = agents[i];
//         if (!agent) continue;

//         auto* agentReport = new AgentReport();
//         agentReport->agentId = agent->getId();
//         agentReport->agentName = agent->getFullName();

//         for (int j = 0; j < bookings.size(); ++j) {
//             Booking* booking = bookings[j];
//             if (!booking) continue;

//             if (booking->getAgentId() == agent->getId()) {
//                 agentReport->totalBookings++;
//                 if (booking->getStatus() == BookingStatus::Issued) {
//                     agentReport->issuedTickets++;
//                     agentReport->totalRevenue += booking->getBaseFare();
//                 } else if (booking->getStatus() == BookingStatus::Cancelled) {
//                     agentReport->cancelledTickets++;
//                 }
//             }
//         }

//         reportList->push_back(agentReport);
//     }
//     return reportList;
// }

// // --- Helper kiểm tra ngày hôm nay ---
// namespace {
//     bool isToday(const std::string& bookingDate) {
//         std::string today = utils::DateTime::formatLocal(
//             utils::DateTime::nowUtc(), "%Y-%m-%d");
//         if (bookingDate.find(today) == 0) return true;

//         std::string todayDMY = utils::DateTime::formatDmY(utils::DateTime::nowUtc());
//         if (bookingDate.find(todayDMY) == 0) return true;

//         return false;
//     }

//     // Kiểm tra booking trong khoảng ngày
//     bool isInRange(const std::string& bookingDate,
//                    const std::string& startDate,
//                    const std::string& endDate) {
//         return bookingDate >= startDate && bookingDate <= endDate;
//     }

//     int getMonthFromDate(const std::string& bookingDate) {
//         // YYYY-MM-DD
//         if (bookingDate.size() < 7) return 0;
//         return std::stoi(bookingDate.substr(5,2));
//     }
// }

// // --- Thống kê theo ngày ---
// double ReportManager::getDailyRevenue(const std::string& agentId) const {
//     double total = 0.0;
//     const auto& bookings = bookingManager_.getAllBookings();
//     for (int i = 0; i < bookings.size(); ++i) {
//         Booking* booking = bookings[i];
//         if (!booking) continue;
//         if (booking->getAgentId() == agentId &&
//             booking->getStatus() == BookingStatus::Issued &&
//             isToday(booking->getBookingDate())) {
//             total += booking->getBaseFare();
//         }
//     }
//     return total;
// }

// int ReportManager::getDailyTicketsSold(const std::string& agentId) const {
//     int count = 0;
//     const auto& bookings = bookingManager_.getAllBookings();
//     for (int i = 0; i < bookings.size(); ++i) {
//         Booking* booking = bookings[i];
//         if (!booking) continue;
//         if (booking->getAgentId() == agentId &&
//             booking->getStatus() == BookingStatus::Issued &&
//             isToday(booking->getBookingDate())) {
//             count++;
//         }
//     }
//     return count;
// }

// int ReportManager::getDailyCancellations(const std::string& agentId) const {
//     int count = 0;
//     const auto& bookings = bookingManager_.getAllBookings();
//     for (int i = 0; i < bookings.size(); ++i) {
//         Booking* booking = bookings[i];
//         if (!booking) continue;
//         if (booking->getAgentId() == agentId &&
//             booking->getStatus() == BookingStatus::Cancelled &&
//             isToday(booking->getBookingDate())) {
//             count++;
//         }
//     }
//     return count;
// }

// int ReportManager::getDailyTicketChanges(const std::string& agentId) const {
//     return 0;
// }

// // --- Thống kê theo khoảng thời gian ---
// int ReportManager::getTicketsSoldInRange(const std::string& startDate,
//                                         const std::string& endDate) const {
//     int count = 0;
//     const auto& bookings = bookingManager_.getAllBookings();
//     for (int i=0; i<bookings.size(); ++i) {
//         Booking* b = bookings[i];
//         if (!b) continue;
//         if (b->getStatus() == BookingStatus::Issued &&
//             isInRange(b->getBookingDate(), startDate, endDate)) {
//             count++;
//         }
//     }
//     return count;
// }

// int ReportManager::getTicketsSoldByClassInRange(const std::string& startDate,
//                                                 const std::string& endDate,
//                                                 BookingClass bookingClass) const {
//     int count = 0;
//     const auto& bookings = bookingManager_.getAllBookings();
//     for (int i=0; i<bookings.size(); ++i) {
//         Booking* b = bookings[i];
//         if (!b) continue;
//         if (b->getStatus() == BookingStatus::Issued &&
//             isInRange(b->getBookingDate(), startDate, endDate) &&
//             b->getClass() == bookingClass) {
//             count++;
//         }
//     }
//     return count;
// }

// DynamicArray<AgentReport*>* ReportManager::generateAgentReportInRange(
//     const std::string& startDate, const std::string& endDate) const {

//     auto* reportList = new DynamicArray<AgentReport*>();
//     const auto& agents = accountManager_.getAllAgents();
//     const auto& bookings = bookingManager_.getAllBookings();

//     for (int i = 0; i < agents.size(); ++i) {
//         AccountAgent* agent = agents[i];
//         if (!agent) continue;

//         auto* agentReport = new AgentReport();
//         agentReport->agentId = agent->getId();
//         agentReport->agentName = agent->getFullName();

//         for (int j = 0; j < bookings.size(); ++j) {
//             Booking* booking = bookings[j];
//             if (!booking) continue;

//             if (booking->getAgentId() == agent->getId() &&
//                 isInRange(booking->getBookingDate(), startDate, endDate)) {
//                 agentReport->totalBookings++;
//                 if (booking->getStatus() == BookingStatus::Issued) {
//                     agentReport->issuedTickets++;
//                     agentReport->totalRevenue += booking->getBaseFare();
//                 } else if (booking->getStatus() == BookingStatus::Cancelled) {
//                     agentReport->cancelledTickets++;
//                 }
//             }
//         }

//         reportList->push_back(agentReport);
//     }

//     return reportList;
// }

// DynamicArray<int>* ReportManager::getMonthlyTicketsByAgent(const std::string& agentId, int year) const {
//     auto* monthly = new DynamicArray<int>(12);
//     for(int i=0; i<12; ++i) (*monthly)[i] = 0;

//     const auto& bookings = bookingManager_.getAllBookings();
//     std::string yearStr = std::to_string(year);

//     for(int i=0; i<bookings.size(); ++i) {
//         Booking* b = bookings[i];
//         if(!b) continue;
//         if(b->getAgentId() == agentId &&
//            b->getStatus() == BookingStatus::Issued &&
//            b->getBookingDate().find(yearStr) == 0) {
//             int month = getMonthFromDate(b->getBookingDate());
//             if(month >= 1 && month <= 12) (*monthly)[month-1]++;
//         }
//     }
//     return monthly;
// }

#include "core/ReportManager.h"
#include "core/AccountManager.h"
#include "core/BookingManager.h"
#include "entities/AccountAgent.h"
#include "entities/Booking.h"
#include "utils/DateTime.h"

// Constructor
ReportManager::ReportManager(AccountManager& am, BookingManager& bm)
    : accountManager_(am), bookingManager_(bm) {}

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
    std::string today = utils::DateTime::formatLocal(utils::DateTime::nowUtc(), "%Y-%m-%d");

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Issued &&
            b->getBookingDate().find(today) == 0) {
            total += b->getBaseFare();
        }
    }
    return total;
}

int ReportManager::getDailyTicketsSold(const std::string& agentId) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    std::string today = utils::DateTime::formatLocal(utils::DateTime::nowUtc(), "%Y-%m-%d");

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Issued &&
            b->getBookingDate().find(today) == 0) {
            count++;
        }
    }
    return count;
}

int ReportManager::getDailyCancellations(const std::string& agentId) const {
    int count = 0;
    const auto& bookings = bookingManager_.getAllBookings();
    std::string today = utils::DateTime::formatLocal(utils::DateTime::nowUtc(), "%Y-%m-%d");

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Cancelled &&
            b->getBookingDate().find(today) == 0) {
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
            b->getBookingDate() >= startDate &&
            b->getBookingDate() <= endDate) {
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
            b->getBookingDate() >= startDate &&
            b->getBookingDate() <= endDate) {
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
            b->getBookingDate() >= startDate &&
            b->getBookingDate() <= endDate) {
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
            b->getBookingDate() >= startDate &&
            b->getBookingDate() <= endDate) {
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
                b->getBookingDate() >= startDate &&
                b->getBookingDate() <= endDate) {
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
    std::string yearStr = std::to_string(year);

    for (int i = 0; i < bookings.size(); ++i) {
        Booking* b = bookings[i];
        if (!b) continue;
        if (b->getAgentId() == agentId &&
            b->getStatus() == BookingStatus::Issued &&
            b->getBookingDate().find(yearStr) == 0) {
            int month = std::stoi(b->getBookingDate().substr(5,2));
            if (month >= 1 && month <= 12) (*monthly)[month-1]++;
        }
    }

    return monthly;
}
