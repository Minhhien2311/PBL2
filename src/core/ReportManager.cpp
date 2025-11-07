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
