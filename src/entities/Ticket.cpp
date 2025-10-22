#include "C:/PBL2/include/entities/Ticket.h"
#include "C:/PBL2/include/utils/GenID.h"
#include <string>
#include <iostream> // Thêm thư viện để dùng cho printTicket

Ticket::Ticket(const std::string& bookingId,
           const std::string& passengerId,
           const std::string& flightInstanceId,
           const std::string& seatId,
           const std::string& bookingClass,
           const std::string& issueDateTime,
           double baseFare,
           double discount,
           double totalAmount,
           TicketStatus status) // Bỏ giá trị mặc định ở đây để tránh lỗi biên dịch
        : ticketNumber(IdGenerator::generateTicketNumber()), bookingId(bookingId),
          passengerId(passengerId), flightInstanceId(flightInstanceId), seatId(seatId),
          bookingClass(bookingClass), issueDateTime(issueDateTime),
          baseFare(baseFare), discount(discount), totalAmount(totalAmount), status(status) {}

// --- Getters (đã có) ---
const std::string& Ticket::getTicketNumber() const { return ticketNumber; }
const std::string& Ticket::getBookingId() const { return bookingId; }
const std::string& Ticket::getPassengerId() const { return passengerId; }
const std::string& Ticket::getFlightInstanceId() const { return flightInstanceId; }
const std::string& Ticket::getSeatId() const { return seatId; }
const std::string& Ticket::getBookingClass() const { return bookingClass; }
const std::string& Ticket::getIssueDateTime() const { return issueDateTime; }
double Ticket::getBaseFare() const { return baseFare; }
double Ticket::getDiscount() const { return discount; }
double Ticket::getTotalAmount() const { return totalAmount; }
TicketStatus Ticket::getStatus() const { return status; }

void Ticket::setStatus(TicketStatus newStatus) { status = newStatus; }

// --- Đọc/Ghi file ---

// Chuyển đối tượng thành 1 dòng string để lưu vào file.
std::string Ticket::toRecordLine() const {
    // Chuyển enum TicketStatus thành số nguyên (0: Active, 1: Exchanged, 2: Cancelled)
    std::string statusStr = std::to_string(static_cast<int>(this->status));

    return this->ticketNumber + "|" +
           this->bookingId + "|" +
           this->passengerId + "|" +
           this->flightInstanceId + "|" +
           this->seatId + "|" +
           this->bookingClass + "|" +
           this->issueDateTime + "|" +
           std::to_string(this->baseFare) + "|" +
           std::to_string(this->discount) + "|" +
           std::to_string(this->totalAmount) + "|" +
           statusStr;
}

// Tạo đối tượng Ticket từ 1 dòng string đọc từ file.
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

    std::string seatId = line.substr(start, end - start);
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
    
    double discount = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    double totalAmount = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.length();

    int statusInt = std::stoi(line.substr(start, end - start));
    TicketStatus status = static_cast<TicketStatus>(statusInt);

    // Tạo đối tượng bằng constructor và sau đó gán lại ticketNumber
    Ticket t(bookingId, passengerId, flightInstanceId, seatId, bookingClass, issueDateTime, baseFare, discount, totalAmount, status);
    
    // Ghi đè ID tự sinh bằng ID đọc từ file
    t.overrideTicketNumberForLoad(ticketNum);
    
    return t;
}

void Ticket::overrideTicketNumberForLoad(const std::string& existingNumber) {
    this->ticketNumber = existingNumber;
}

void Ticket::printTicket() { 
    return;
}