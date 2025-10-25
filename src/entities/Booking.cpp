#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/utils/GenID.h"
#include <string> 

//  Constructor
Booking::Booking(const std::string& flightInstanceId,
                 const std::string& passengerId,
                 const std::string& bookingDate,
                 BookingClass bookingClass,
                 int baseFare,
                BookingStatus status)
    : bookingId(IdGenerator::generateBookingId()),
      flightInstanceId(flightInstanceId),
      passengerId(passengerId),
      bookingDate(bookingDate),
      bookingClass(bookingClass),
      baseFare(baseFare < 0 ? 0 : baseFare),
      status(status) {}

//  Getters
const std::string& Booking::getBookingId()      const { return bookingId; }
const std::string& Booking::getFlightInstanceId() const { return flightInstanceId; }
const std::string& Booking::getPassengerId()    const { return passengerId; }
const std::string& Booking::getBookingDate()    const { return bookingDate; }
BookingClass       Booking::getClass()          const { return bookingClass; }
int                Booking::getBaseFare()       const { return baseFare; }
BookingStatus      Booking::getStatus()         const { return status; }

//  Setters
void Booking::setBaseFare(int value) {
    if (value < 0) value = 0;
    baseFare = value;
}

void Booking::setStatus(BookingStatus newStatus) {
    this->status = newStatus;
}

// --- Đọc/Ghi file ---

std::string Booking::toRecordLine() const {
    std::string classStr = std::to_string(static_cast<int>(this->bookingClass));
    std::string statusStr = std::to_string(static_cast<int>(this->status)); // Thêm status

    return this->bookingId + "|" +
           this->flightInstanceId + "|" +
           this->passengerId + "|" +
           this->bookingDate + "|" +
           classStr + "|" +
           std::to_string(this->baseFare) + "|" +
           statusStr; // Nối status vào cuối
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
    
    std::string bookingDate = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    BookingClass bClass = static_cast<BookingClass>(std::stoi(line.substr(start, end - start)));
    start = end + 1;
    end = line.find('|', start);

    int baseFare = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.length(); // Thay đổi ở đây

    // Đọc status từ cuối dòng
    int statusInt = std::stoi(line.substr(start, end - start));
    BookingStatus status = static_cast<BookingStatus>(statusInt);

    // Tạo đối tượng với đầy đủ thông tin
    Booking booking(instanceId, passengerId, bookingDate, bClass, baseFare, status);
    
    // Ghi đè ID
    booking.overrideIdForLoad(id);

    return booking;
}

// Helper cho việc nạp dữ liệu từ file.
void Booking::overrideIdForLoad(const std::string& existingId) {
    this->bookingId = existingId;
}