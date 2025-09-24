#include <iostream>
#include "C:/PBL2/include/entities/Ticket.h"

Ticket::Ticket(const std::string& id, const std::string& bookingId, const std::string& number,
           const std::string& issueDate, const std::string& status,
           const std::string& originalTicketId)
    : ticketId(id), bookingId(bookingId), ticketNumber(number),
      issueDate(issueDate), status(status), originalTicketId(originalTicketId) {}

// Getter methods
std::string Ticket::getTicketId() const { return ticketId; }
std::string Ticket::getBookingId() const { return bookingId; }
std::string Ticket::getTicketNumber() const { return ticketNumber; }
std::string Ticket::getIssueDate() const { return issueDate; }
std::string Ticket::getStatus() const { return status; }
std::string Ticket::getOriginalTicketId() const { return originalTicketId; }

// Setter methods
void Ticket::setStatus(const std::string& newStatus) { status = newStatus; }

// Business methods
bool Ticket::isActive() const { return status == "Active"; }
bool Ticket::isChanged() const { return !originalTicketId.empty(); }
bool Ticket::isCancelled() const { return status == "Cancelled"; }
bool Ticket::canBeRefunded() const { return status == "Active" || status == "Changed"; } // Example logic

std::vector<Ticket> Ticket::loadAllTickets() { return {}; } // Placeholder
Ticket Ticket::findTicketByNumber(const std::string& ticketNumber) { return Ticket("", "", "", "", "", ""); } // Placeholder
std::vector<Ticket> Ticket::findTicketsByBooking(const std::string& bookingId) { return {}; } // Placeholder
bool Ticket::issueTicketsForBooking(const std::string& bookingId) { return false; } // Placeholder
bool Ticket::cancelTicket(const std::string& ticketId) { return false; } // Placeholder