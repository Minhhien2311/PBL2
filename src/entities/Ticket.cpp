#include "C:/PBL2/include/entities/Ticket.h"
#include "C:/PBL2/include/utils/GenID.h"

Ticket::Ticket(const std::string& bookingId,
           const std::string& pnr,
           const std::string& passengerId,
           const std::string& flightInstanceId,
           const std::string& seatId,
           const std::string& bookingClass,
           const std::string& issueDateTime,
           double baseFare,
           double discount,
           double totalAmount,
           TicketStatus status = TicketStatus::Active)
        : ticketNumber(IdGenerator::generateTicketNumber()), bookingId(bookingId), pnr(pnr),
          passengerId(passengerId), flightInstanceId(flightInstanceId), seatId(seatId),
          bookingClass(bookingClass), issueDateTime(issueDateTime),
          baseFare(baseFare), discount(discount), totalAmount(totalAmount), status(status) {}

    // Getters
    const std::string& Ticket::getTicketNumber() const { return ticketNumber; }
    const std::string& Ticket::getBookingId() const { return bookingId; }
    const std::string& Ticket::getPNR() const { return pnr; }
    const std::string& Ticket::getPassengerId() const { return passengerId; }
    const std::string& Ticket::getFlightInstanceId() const { return flightInstanceId; }
    const std::string& Ticket::getSeatId() const { return seatId; }
    const std::string& Ticket::getBookingClass() const { return bookingClass; }
    const std::string& Ticket::getIssueDateTime() const { return issueDateTime; }
    double Ticket::getBaseFare() const { return baseFare; }
    double Ticket::getDiscount() const { return discount; }
    double Ticket::getTotalAmount() const { return totalAmount; }
    TicketStatus Ticket::getStatus() const { return status; }

    void Ticket::setStatus(TicketStatus newStatus) { status = newStatus; } // dùng khi exchange/cancel

    // I/O 1 dòng record
    std::string Ticket::toRecordLine() const { }                       // ghi object vào file
    Ticket Ticket::fromRecordLine(const std::string& line) {  }  // đọc từ file
    void Ticket::printTicket() { /* TODO: Implement */ }