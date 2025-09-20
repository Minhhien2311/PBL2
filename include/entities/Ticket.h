// Thông tin vé đã xuất
#ifndef TICKET_H
#define TICKET_H

#include <string>
#include "Booking.h"

class Ticket {
private:
    std::string ticketId;
    std::string bookingId;
    std::string ticketNumber;
    std::string issueDate;
    std::string status;
    std::string originalTicketId;

public:
    Ticket(const std::string& id, const std::string& bookingId, const std::string& number,
           const std::string& issueDate, const std::string& status, 
           const std::string& originalTicketId = "");
    
    // Getter methods
    std::string getTicketId() const;
    std::string getBookingId() const;
    std::string getTicketNumber() const;
    std::string getIssueDate() const;
    std::string getStatus() const;
    std::string getOriginalTicketId() const;
    
    // Setter methods
    void setStatus(const std::string& newStatus);
    
    // Business methods
    bool isActive() const;
    bool isChanged() const;
    bool isCancelled() const;
    bool canBeRefunded() const;
    
    static std::vector<Ticket> loadAllTickets();
    static Ticket findTicketByNumber(const std::string& ticketNumber);
    static std::vector<Ticket> findTicketsByBooking(const std::string& bookingId);
    static bool issueTicketsForBooking(const std::string& bookingId);
    static bool cancelTicket(const std::string& ticketId);
};

#endif // TICKET_H