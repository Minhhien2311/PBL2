#include "entities/Flight.h" // <-- Đã đổi
#include "utils/GenID.h"
#include "utils/DateTime.h"         // Cần để chuyển đổi ngày
#include <iostream>                 // Cần cho hàm displayInfo

/* --- Constructor ---
    Chỉ cần nhận vào thông tin gốc và totalCapacity.
*/
Flight::Flight(const std::string& routeId, // <-- Đã đổi
               const std::string& airline,
               const std::string& flightNumber,
               const std::string& departureDate,
               const std::string& departureTime,
               const std::string& arrivalDate,
               const std::string& arrivalTime,
               int totalCapacity,
               int fareEconomy,
               int fareBusiness)
    : flightId(IdGenerator::generateFlightId()), // <-- Đã đổi
      routeId(routeId), // <-- Đã đổi
      airline(airline),
      flightNumber(flightNumber),
      departureDate(departureDate), 
      departureTime(departureTime), 
      arrivalDate(arrivalDate),     
      arrivalTime(arrivalTime),     
      totalCapacity(totalCapacity),
      fareEconomy(fareEconomy),
      fareBusiness(fareBusiness) {}

// --- Getters ---
const std::string& Flight::getFlightId() const { return flightId; } // <-- Đã đổi
const std::string& Flight::getRouteId() const { return routeId; } // <-- Đã đổi
const std::string& Flight::getAirline() const { return airline; }
const std::string& Flight::getFlightNumber() const { return flightNumber; }
const std::string& Flight::getDepartureDate() const { return departureDate; }
const std::string& Flight::getDepartureTime() const { return departureTime; }
const std::string& Flight::getArrivalDate() const { return arrivalDate; }
const std::string& Flight::getArrivalTime() const { return arrivalTime; }

int Flight::getTotalCapacity() const {
    return totalCapacity;
}

double Flight::getFareEconomy() const { return fareEconomy; }
double Flight::getFareBusiness() const { return fareBusiness; }

// --- Setters cho các thông tin có thể thay đổi ---
void Flight::setFareEconomy(double fare) {
    this->fareEconomy = fare;
}
void Flight::setFareBusiness(double fare) {
    this->fareBusiness = fare;
}

// --- Đọc/Ghi file cấu hình ---

// Chuyển đổi đối tượng thành một dòng string, ngăn cách bởi dấu '|'.
std::string Flight::toRecordLine() const {
    return this->flightId + "|" + // <-- Đã đổi
           this->routeId + "|" + // <-- Đã đổi
           this->airline + "|" +
           this->flightNumber + "|" +
           this->departureDate + "|" + 
           this->departureTime + "|" + 
           this->arrivalDate + "|" +   
           this->arrivalTime + "|" +   
           std::to_string(this->totalCapacity) + "|" +
           std::to_string(this->fareEconomy) + "|" +
           std::to_string(this->fareBusiness);
}

Flight Flight::fromRecordLine(const std::string& line) {
    size_t start = 0;
    size_t end = line.find('|');
    
    std::string id = line.substr(start, end - start); // Đây là flightId
    start = end + 1;
    end = line.find('|', start);

    std::string routeId = line.substr(start, end - start); // Đây là routeId
    start = end + 1;
    end = line.find('|', start);

    std::string airline = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string flightNumber = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string depDate = line.substr(start, end - start); 
    start = end + 1;
    end = line.find('|', start);

    std::string depTime = line.substr(start, end - start); 
    start = end + 1;
    end = line.find('|', start);

    std::string arrDate = line.substr(start, end - start); 
    start = end + 1;
    end = line.find('|', start);

    std::string arrTime = line.substr(start, end - start); 
    start = end + 1;
    end = line.find('|', start);

    int totalCap = std::stoi(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    double fareEco = std::stod(line.substr(start, end - start));
    start = end + 1;
    end = line.length();

    double fareBus = std::stod(line.substr(start, end - start));

    // Dùng constructor mới
    Flight flight(routeId, airline, flightNumber, depDate, depTime, arrDate, arrTime, totalCap, fareEco, fareBus);

    // Ghi đè ID
    flight.overrideIdForLoad(id);

    return flight;
}
// --- Helper cho việc nạp dữ liệu ---

void Flight::overrideIdForLoad(const std::string& existingId) {
    this->flightId = existingId; // <-- Đã đổi
}