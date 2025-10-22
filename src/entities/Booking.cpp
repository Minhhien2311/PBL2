#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/utils/GenID.h"
#include <algorithm>
#include <string> 

namespace {
    static double clampNonNegative(double v) {
        return (v < 0.0) ? 0.0 : v;
    }
}

//  Constructor
Booking::Booking(const std::string& flightInstanceId,
                 const std::string& passengerId,
                 const std::string& bookingDate,
                 BookingClass bookingClass,
                 double baseFare)
    : bookingId(IdGenerator::generateBookingId()),
      flightInstanceId(flightInstanceId),
      passengerId(passengerId),
      // (Đã xóa seatId)
      bookingDate(bookingDate),
      bookingClass(bookingClass),
      baseFare(baseFare < 0.0 ? 0.0 : baseFare),
      discount(0.0), 
      totalAmount(baseFare < 0.0 ? 0.0 : baseFare)
{
}

//  Getters
const std::string& Booking::getBookingId()      const { return bookingId; }
const std::string& Booking::getFlightInstanceId() const { return flightInstanceId; }
const std::string& Booking::getPassengerId()    const { return passengerId; }
const std::string& Booking::getBookingDate()    const { return bookingDate; }
BookingClass       Booking::getClass()          const { return bookingClass; }
double             Booking::getBaseFare()       const { return baseFare; }
double             Booking::getTotalAmount()    const { return totalAmount; }

//  Setters
void Booking::setBaseFare(double value) {
    if (value < 0.0) value = 0.0;
    baseFare = value;
    totalAmount = clampNonNegative(baseFare - discount);
}

// --- Đọc/Ghi file (Đã bỏ seatId) ---

std::string Booking::toRecordLine() const {
    std::string classStr = std::to_string(static_cast<int>(this->bookingClass));

    return this->bookingId + "|" +
           this->flightInstanceId + "|" +
           this->passengerId + "|" +
           // (Đã xóa seatId)
           this->bookingDate + "|" +
           classStr + "|" +
           std::to_string(this->baseFare) + "|" +
           std::to_string(this->discount) + "|" +
           std::to_string(this->totalAmount);
}

Booking Booking::fromRecordLine(const std::string& line) {
    size_t start = 0;
    size_t end = line.find('|');
    
    std::string id = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string instanceId = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string passengerId = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);
    
    // (Đã xóa logic đọc seatId)

    std::string bookingDate = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    BookingClass bClass = static_cast<BookingClass>(std::stoi(line.substr(start, end - start)));
    start = end + 1;
    end = line.find('|', start);

    double baseFare = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    double discount = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.length(); 

    double totalAmount = std::stod(line.substr(start, end - start));

    // Tạo đối tượng
    Booking booking(instanceId, passengerId, bookingDate, bClass, baseFare);
    
    // Ghi đè các giá trị đã đọc từ file
    booking.overrideIdForLoad(id);
    // (Đã xóa gán seatId)
    booking.discount = discount;
    booking.totalAmount = totalAmount;

    return booking;
}

// Helper cho việc nạp dữ liệu từ file.
void Booking::overrideIdForLoad(const std::string& existingId) {
    this->bookingId = existingId;
}