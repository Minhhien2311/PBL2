#include "C:/PBL2/include/entities/Flight.h"

Flight::Flight(const std::string& id,
           const std::string& number,
           const std::string& airline,
           const std::string& aircraft,
           const std::string& departureIATA,
           const std::string& arrivalIATA)
    : flightId(id), flightNumber(number), airline(airline), aircraftType(aircraft), departureAirport(departureIATA), arrivalAirport(arrivalIATA)
{}

// Getter (trả const-ref/int để tránh copy)
const std::string& Flight::getFlightId() const { return flightId; }
const std::string& Flight::getFlightNumber() const { return flightNumber; }
const std::string& Flight::getAirline() const { return airline; }
const std::string& Flight::getAircraftType() const { return aircraftType; }
const std::string& Flight::getDepartureAirport() const { return departureAirport; }
const std::string& Flight::getArrivalAirport() const { return arrivalAirport; }

// Setter tối thiểu (nếu nghiệp vụ cho phép đổi)
void Flight::setAircraftType(const std::string& aircraft) {
    aircraftType = aircraft;
}

// Tiện ích nhẹ (không dính I/O hoặc DB)
std::string Flight::getFlightInfo() const {
    // Sẽ triển khai sau
}