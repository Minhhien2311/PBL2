#include <iostream>
#include "C:/PBL2/include/entities/Seat.h"

Seat::Seat(const std::string& id, const std::string& instanceId, const std::string& number,
         const std::string& sClass, bool available, double fee)
    : seatId(id), instanceId(instanceId), seatNumber(number), seatClass(sClass),
      isAvailable(available), extraFee(fee) {}

    
    // Getter methods (definitions)
    std::string Seat::getSeatId() const { return seatId; }              // get SeatId
    std::string Seat::getInstanceId() const { return instanceId; }      // get InstanceId
    std::string Seat::getSeatNumber() const { return seatNumber; }      // get SeatNumber
    std::string Seat::getSeatClass() const { return seatClass; }        // get SeatClass
    bool Seat::getIsAvailable() const { return isAvailable; }           // get IsAvailable
    double Seat::getExtraFee() const { return extraFee; }               // get ExtraFee
    
    // Setter methods
    void Seat::setIsAvailable(bool available) { isAvailable = available; }
    void Seat::setExtraFee(double fee) { extraFee = fee; }
    
    // Business methods
    double Seat::calculateTotalPrice(double basePrice) const {
        // Hàm tính giá ghế? Để coi thử có cần không
        return basePrice + extraFee;
    }
    
    bool Seat::isBusinessClass() const {
        // Logic kiểm tra ghế thương gia
        return seatClass == "Business";
    }
    
    bool Seat::isEconomyClass() const {
        // Logic kiểm tra ghế phổ thông
        return seatClass == "Economy";
    }
    
    static std::vector<Seat> loadSeatsForInstance(const std::string& instanceId) {}
    static Seat findSeatById(const std::string& seatId) {}
    static bool reserveSeat(const std::string& seatId) {}
    static bool releaseSeat(const std::string& seatId) {}