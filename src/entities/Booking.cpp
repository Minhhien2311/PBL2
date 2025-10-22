#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/utils/GenID.h"
#include <string> 

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
      baseFare(baseFare < 0.0 ? 0.0 : baseFare) {}

//  Getters
const std::string& Booking::getBookingId()      const { return bookingId; }
const std::string& Booking::getFlightInstanceId() const { return flightInstanceId; }
const std::string& Booking::getPassengerId()    const { return passengerId; }
const std::string& Booking::getBookingDate()    const { return bookingDate; }
BookingClass       Booking::getClass()          const { return bookingClass; }
double             Booking::getBaseFare()       const { return baseFare; }

//  Setters
void Booking::setBaseFare(double value) {
    if (value < 0.0) value = 0.0;
    baseFare = value;
}

// --- Đọc/Ghi file ---

std::string Booking::toRecordLine() const {
    std::string classStr = std::to_string(static_cast<int>(this->bookingClass));

    return this->bookingId + "|" +
           this->flightInstanceId + "|" +
           this->passengerId + "|" +
           // (Đã xóa seatId)
           this->bookingDate + "|" +
           classStr + "|" +
           std::to_string(this->baseFare);
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

    // Tạo đối tượng
    Booking booking(instanceId, passengerId, bookingDate, bClass, baseFare);
    
    // Ghi đè các giá trị đã đọc từ file
    booking.overrideIdForLoad(id);

    return booking;
}

// Helper cho việc nạp dữ liệu từ file.
void Booking::overrideIdForLoad(const std::string& existingId) {
    this->bookingId = existingId;
}