#include "C:/PBL2/include/entities/Ticket.h"
#include "C:/PBL2/include/utils/GenID.h"
#include <string>
#include <iostream> 

Ticket::Ticket(const std::string& bookingId,
           const std::string& passengerId,
           const std::string& flightInstanceId,
           const std::string& bookingClass,
           const std::string& issueDateTime,
           double baseFare,
           TicketStatus status) 
        : ticketNumber(IdGenerator::generateTicketNumber()), bookingId(bookingId),
          passengerId(passengerId), flightInstanceId(flightInstanceId), 
          bookingClass(bookingClass), issueDateTime(issueDateTime),
          baseFare(baseFare), status(status) {}

// --- Getters (đã có) ---
const std::string& Ticket::getTicketNumber() const { return ticketNumber; }
const std::string& Ticket::getBookingId() const { return bookingId; }
const std::string& Ticket::getPassengerId() const { return passengerId; }
const std::string& Ticket::getFlightInstanceId() const { return flightInstanceId; }
const std::string& Ticket::getBookingClass() const { return bookingClass; }
const std::string& Ticket::getIssueDateTime() const { return issueDateTime; }
double Ticket::getBaseFare() const { return baseFare; }
TicketStatus Ticket::getStatus() const { return status; }

void Ticket::setStatus(TicketStatus newStatus) { status = newStatus; }

// --- Đọc/Ghi file ---

std::string Ticket::toRecordLine() const {
    std::string statusStr = std::to_string(static_cast<int>(this->status));

    return this->ticketNumber + "|" +
           this->bookingId + "|" +
           this->passengerId + "|" +
           this->flightInstanceId + "|" +
           this->bookingClass + "|" +
           this->issueDateTime + "|" +
           std::to_string(this->baseFare) + "|" +
           statusStr;
}

Ticket Ticket::fromRecordLine(const std::string& line) {
    size_t start = 0;
    size_t end = line.find('|');
    
    std::string ticketNum = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string bookingId = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string passengerId = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string flightInstanceId = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string bookingClass = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string issueDateTime = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    double baseFare = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    int statusInt = std::stoi(line.substr(start, end - start));
    TicketStatus status = static_cast<TicketStatus>(statusInt);

    // Tạo đối tượng bằng constructor
    Ticket t(bookingId, passengerId, flightInstanceId, bookingClass, issueDateTime, baseFare, status);
    
    // Ghi đè ID
    t.overrideTicketNumberForLoad(ticketNum);
    
    return t;
}

void Ticket::overrideTicketNumberForLoad(const std::string& existingNumber) {
    this->ticketNumber = existingNumber;
}

void Ticket::printTicket() { 
    return;
}