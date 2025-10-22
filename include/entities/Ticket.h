#ifndef TICKET_H
#define TICKET_H

#include <string>

// Chỉ còn 2 trạng thái: Active (có thể bay) hoặc Cancelled (đã hủy)
enum class TicketStatus {
    Active,
    Cancelled
};

class Ticket {
private:
    std::string ticketNumber;     // số vé
    std::string bookingId;        // tham chiếu về booking gốc
    std::string passengerId;
    std::string flightInstanceId;
    std::string bookingClass;     // "Economy" / "Business"
    std::string issueDateTime;    // "YYYY-MM-DDTHH:MM:SS"

    double baseFare;
    double discount;
    double totalAmount;

    TicketStatus status;

public:
    Ticket(const std::string& bookingId,
           const std::string& passengerId,
           const std::string& flightInstanceId,
           const std::string& bookingClass,
           const std::string& issueDateTime,
           double baseFare,
           double discount,
           double totalAmount,
           TicketStatus status = TicketStatus::Active);

    // Getters
    const std::string& getTicketNumber() const;
    const std::string& getBookingId() const;
    const std::string& getPassengerId() const;
    const std::string& getFlightInstanceId() const;
    const std::string& getBookingClass() const;
    const std::string& getIssueDateTime() const;
    double getBaseFare() const;
    double getDiscount() const;
    double getTotalAmount() const;
    TicketStatus getStatus() const;

    void setStatus(TicketStatus s);  // dùng khi cancel

    void overrideTicketNumberForLoad(const std::string& existingNumber);

    std::string toRecordLine() const;
    static Ticket fromRecordLine(const std::string& line);
    void printTicket();
};

#endif // TICKET_H