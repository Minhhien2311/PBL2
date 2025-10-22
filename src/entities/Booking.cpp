#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/utils/GenID.h"
#include <algorithm>

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
      seatId(""),
      bookingDate(bookingDate),
      status(BookingStatus::Confirmed),     // tạo mới ở trạng thái Confirmed (theo hướng tối giản)
      bookingClass(bookingClass),
      baseFare(baseFare < 0.0 ? 0.0 : baseFare),
      discount(0.0),
      totalAmount(baseFare < 0.0 ? 0.0 : baseFare),
      cancelReason("")
{
    // totalAmount = baseFare - discount (discount mặc định 0)
}

//  Getters 
const std::string& Booking::getBookingId()      const { return bookingId; }
const std::string& Booking::getFlightInstanceId() const { return flightInstanceId; }
const std::string& Booking::getPassengerId()    const { return passengerId; }
const std::string& Booking::getSeatId()         const { return seatId; }

const std::string& Booking::getBookingDate()    const { return bookingDate; }
BookingStatus      Booking::getStatus()         const { return status; }
BookingClass       Booking::getClass()          const { return bookingClass; }

double             Booking::getBaseFare()       const { return baseFare; }
double             Booking::getTotalAmount()    const { return totalAmount; }

const std::string& Booking::getCancelReason()   const { return cancelReason; }

//  Thay đổi thông tin 
void Booking::setStatus(BookingStatus newStatus) {
    // Không cho đổi từ Cancelled sang trạng thái khác
    if (status == BookingStatus::Cancelled) return;

    // Nếu đã Issued thì không cho quay lại (theo rule tối giản)
    if (status == BookingStatus::Issued && newStatus != BookingStatus::Issued) return;

    status = newStatus;
}

void Booking::setCancelReason(const std::string& reason) {
    cancelReason = reason;
}

void Booking::setBaseFare(double value) {
    if (value < 0.0) value = 0.0;
    baseFare = value;
    // tổng tiền = baseFare - discount (discount hiện không có setter → mặc định 0)
    totalAmount = clampNonNegative(baseFare - discount);
}

//  Business rules 
bool Booking::canBeCancelled() const {
    // Tối giản: chỉ hủy khi đang Confirmed
    return status == BookingStatus::Confirmed;
}

bool Booking::canBeChanged() const {
    // Tối giản: chỉ cho phép thay đổi khi đang Confirmed
    return status == BookingStatus::Confirmed;
}

//  Seat ops 
bool Booking::assignSeat(const std::string& newSeatId) {
    // Chỉ cho gán ghế khi đang Confirmed
    if (!canBeChanged()) return false;

    // newSeatId rỗng thì coi như không hợp lệ
    if (newSeatId.empty()) return false;

    seatId = newSeatId;
    return true;
}

void Booking::unassignSeat() {
    // Chỉ cho bỏ ghế khi đang Confirmed
    if (!canBeChanged()) return;
    seatId.clear();
}

// --- Đọc/Ghi file ---

// Chuyển đổi đối tượng thành một dòng string để lưu vào file.
std::string Booking::toRecordLine() const {
    // Chuyển đổi enum thành số nguyên để lưu file
    std::string statusStr = std::to_string(static_cast<int>(this->status));
    std::string classStr = std::to_string(static_cast<int>(this->bookingClass));

    return this->bookingId + "|" +
           this->flightInstanceId + "|" +
           this->passengerId + "|" +
           this->seatId + "|" +
           this->bookingDate + "|" +
           statusStr + "|" +
           classStr + "|" +
           std::to_string(this->baseFare) + "|" +
           std::to_string(this->discount) + "|" +
           std::to_string(this->totalAmount) + "|" +
           this->cancelReason;
}

// Tạo đối tượng Booking từ một dòng string đọc từ file.
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
    
    std::string seatId = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string bookingDate = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    BookingStatus status = static_cast<BookingStatus>(std::stoi(line.substr(start, end - start)));
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
    end = line.find('|', start);

    double totalAmount = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.length();

    std::string cancelReason = line.substr(start, end - start);

    // Dùng constructor để tạo đối tượng
    // Lưu ý: ID sẽ được sinh tự động, sau đó ta ghi đè lại
    Booking booking(instanceId, passengerId, bookingDate, bClass, baseFare);
    
    // Ghi đè các giá trị đã đọc từ file
    booking.overrideIdForLoad(id);
    booking.status = status;
    booking.seatId = seatId;
    booking.discount = discount;
    booking.totalAmount = totalAmount;
    booking.cancelReason = cancelReason;

    return booking;
}

// Helper cho việc nạp dữ liệu từ file.
void Booking::overrideIdForLoad(const std::string& existingId) {
    this->bookingId = existingId;
}
