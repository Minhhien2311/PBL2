#include "C:/PBL2/include/entities/Booking.h"
#include <algorithm>

namespace {
    static double clampNonNegative(double v) {
        return (v < 0.0) ? 0.0 : v;
    }
}

//  Constructor 
Booking::Booking(const std::string& bookingId,
                 const std::string& pnr,
                 const std::string& agentId,
                 const std::string& flightInstanceId,
                 const std::string& passengerId,
                 const std::string& bookingDate,
                 BookingClass bookingClass,
                 double baseFare)
    : bookingId(bookingId),
      pnr(pnr),
      agentId(agentId),
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
const std::string& Booking::getPNR()            const { return pnr; }
const std::string& Booking::getAgentId()        const { return agentId; }
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
