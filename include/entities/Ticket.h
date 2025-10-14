#ifndef TICKET_H
#define TICKET_H

#include <string>

// Vé phát sinh khi Booking chuyển sang Issued.
// Là "snapshot" bất biến của Booking tại thời điểm xuất.
enum class TicketStatus {
    Active,     // vé đang có hiệu lực
    Exchanged,  // đã đổi sang vé mới
    Cancelled
};

class Ticket {
private:
    std::string ticketNumber;     // số vé (khác PNR); ví dụ: 123-4567890123
    std::string bookingId;        // tham chiếu về booking gốc
    std::string pnr;              // lưu lại PNR lúc xuất

    // Ảnh chụp hành trình/khách/ghế/giá tại thời điểm xuất
    std::string passengerId;
    std::string flightInstanceId;
    std::string seatId;
    std::string bookingClass;     // "Economy" / "Business" (hoặc enum -> serialize ra chữ)
    std::string issueDateTime;    // "YYYY-MM-DDTHH:MM:SS" (quy ước timezone)

    double baseFare;
    double discount;
    double totalAmount;

    TicketStatus status;

public:
    Ticket(const std::string& bookingId,
           const std::string& pnr,
           const std::string& passengerId,
           const std::string& flightInstanceId,
           const std::string& seatId,
           const std::string& bookingClass,
           const std::string& issueDateTime,
           double baseFare,
           double discount,
           double totalAmount,
           TicketStatus status = TicketStatus::Active);

    // Getters (rút gọn)
    const std::string& getTicketNumber() const;
    const std::string& getBookingId() const;
    const std::string& getPNR() const;
    const std::string& getPassengerId() const;
    const std::string& getFlightInstanceId() const;
    const std::string& getSeatId() const;
    const std::string& getBookingClass() const;
    const std::string& getIssueDateTime() const;
    double getBaseFare() const;
    double getDiscount() const;
    double getTotalAmount() const;
    TicketStatus getStatus() const;

    void setStatus(TicketStatus s);  // dùng khi exchange/cancel

    void overrideTicketNumberForLoad(const std::string& existingNumber);

    // I/O 1 dòng record (tuỳ format nhóm: CSV/TSV/pipe)
    std::string toRecordLine() const;                       // ghi object vào file
    static Ticket fromRecordLine(const std::string& line);  // đọc từ file
    void printTicket();
};

#endif // TICKET_H
