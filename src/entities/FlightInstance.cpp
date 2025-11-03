#include "entities/FlightInstance.h"
#include "utils/GenID.h"
#include "utils/DateTime.h"         // Cần để chuyển đổi ngày
#include <iostream>                 // Cần cho hàm displayInfo

/* --- Constructor ---
    Chỉ cần nhận vào thông tin gốc và totalCapacity.
*/
FlightInstance::FlightInstance(const std::string& flightId,
                               const std::string& flightNumber,
                               const std::string& departureDate,
                               const std::string& departureTime,
                               const std::string& arrivalDate,
                               const std::string& arrivalTime,
                               int totalCapacity,
                               int fareEconomy,
                               int fareBusiness)
    : instanceId(IdGenerator::generateInstanceId()),
      flightId(flightId),
      flightNumber(flightNumber),
      departureDate(departureDate), 
      departureTime(departureTime), 
      arrivalDate(arrivalDate),     
      arrivalTime(arrivalTime),     
      totalCapacity(totalCapacity),
      fareEconomy(fareEconomy),
      fareBusiness(fareBusiness) {}

// --- Getters ---
const std::string& FlightInstance::getInstanceId() const { return instanceId; }
const std::string& FlightInstance::getFlightId() const { return flightId; }
const std::string& FlightInstance::getFlightNumber() const { return flightNumber; }
const std::string& FlightInstance::getDepartureDate() const { return departureDate; }
const std::string& FlightInstance::getDepartureTime() const { return departureTime; }
const std::string& FlightInstance::getArrivalDate() const { return arrivalDate; }
const std::string& FlightInstance::getArrivalTime() const { return arrivalTime; }

int FlightInstance::getTotalCapacity() const {
    return totalCapacity;
}

double FlightInstance::getFareEconomy() const { return fareEconomy; }
double FlightInstance::getFareBusiness() const { return fareBusiness; }

// --- Setters cho các thông tin có thể thay đổi ---
void FlightInstance::setFareEconomy(double fare) {
    this->fareEconomy = fare;
}
void FlightInstance::setFareBusiness(double fare) {
    this->fareBusiness = fare;
}

// --- Hàm tiện ích ---
void FlightInstance::displayInfo() const {
    // Deploy sau nha
    return;
}

// --- Đọc/Ghi file cấu hình ---

// Chuyển đổi đối tượng thành một dòng string, ngăn cách bởi dấu '|'.
std::string FlightInstance::toRecordLine() const {
    return this->instanceId + "|" +
           this->flightId + "|" +
           this->flightNumber + "|" +
           this->departureDate + "|" + 
           this->departureTime + "|" + 
           this->arrivalDate + "|" +   
           this->arrivalTime + "|" +   
           std::to_string(this->totalCapacity) + "|" +
           std::to_string(this->fareEconomy) + "|" +
           std::to_string(this->fareBusiness);
}

FlightInstance FlightInstance::fromRecordLine(const std::string& line) {
    size_t start = 0;
    size_t end = line.find('|');
    
    std::string id = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string fId = line.substr(start, end - start);
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

    // Dùng constructor mới chỉ với totalCapacity
    FlightInstance instance(fId, flightNumber, depDate, depTime, arrDate, arrTime, totalCap, fareEco, fareBus);

    // Ghi đè ID
    instance.overrideIdForLoad(id);

    return instance;
}
// --- Helper cho việc nạp dữ liệu ---

void FlightInstance::overrideIdForLoad(const std::string& existingId) {
    this->instanceId = existingId;
}