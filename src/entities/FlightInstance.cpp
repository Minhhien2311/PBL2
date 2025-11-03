#include "entities/FlightInstance.h"
#include "utils/DateTime.h"         // Cần để chuyển đổi ngày
#include <iostream>                 // Cần cho hàm displayInfo

// Default values for backward compatibility
namespace {
    constexpr int DEFAULT_BUSINESS_SEATS = 0;
    constexpr int DEFAULT_ECONOMY_SEATS = 0;
}

/* --- Constructor ---
    Chỉ cần nhận vào thông tin gốc, sau đó tự động:
    1. Sinh ID duy nhất từ flightNumber-YYYYMMDD.
    2. Khởi tạo số ghế trống ban đầu bằng tổng số ghế.
*/
FlightInstance::FlightInstance(const std::string& flightId,
                               const std::string& flightNumber,
                               const std::string& departureDate,
                               const std::string& departureTime,
                               const std::string& arrivalDate,
                               const std::string& arrivalTime,
                               int totalEconomySeats,
                               int totalBusinessSeats,
                               int fareEconomy,
                               int fareBusiness)
    : instanceId(flightNumber),
      flightId(flightId),
      flightNumber(flightNumber),
      departureDate(departureDate), 
      departureTime(departureTime), 
      arrivalDate(arrivalDate),     
      arrivalTime(arrivalTime),     
      economyTotal(totalEconomySeats),
      economyAvailable(totalEconomySeats),
      businessTotal(totalBusinessSeats),
      businessAvailable(totalBusinessSeats),
      fareEconomy(fareEconomy),
      fareBusiness(fareBusiness),
      businessSeatsCount(totalBusinessSeats),
      economySeatsCount(totalEconomySeats) {}

// --- Getters ---
const std::string& FlightInstance::getInstanceId() const { return instanceId; }
const std::string& FlightInstance::getFlightId() const { return flightId; }
const std::string& FlightInstance::getFlightNumber() const { return flightNumber; }
const std::string& FlightInstance::getDepartureDate() const { return departureDate; }
const std::string& FlightInstance::getDepartureTime() const { return departureTime; }
const std::string& FlightInstance::getArrivalDate() const { return arrivalDate; }
const std::string& FlightInstance::getArrivalTime() const { return arrivalTime; }

// Các getters tính toán, đảm bảo dữ liệu luôn nhất quán
int FlightInstance::getTotalCapacity() const {
    return this->economyTotal + this->businessTotal;
}
int FlightInstance::getTotalSeatsAvailable() const {
    return this->economyAvailable + this->businessAvailable;
}
    
int FlightInstance::getEconomyTotal() const { return economyTotal; }
int FlightInstance::getEconomyAvailable() const { return economyAvailable; }
int FlightInstance::getBusinessTotal() const { return businessTotal; }
int FlightInstance::getBusinessAvailable() const { return businessAvailable; }
    
double FlightInstance::getFareEconomy() const { return fareEconomy; }
double FlightInstance::getFareBusiness() const { return fareBusiness; }

int FlightInstance::getBusinessSeatsCount() const { return businessSeatsCount; }
int FlightInstance::getEconomySeatsCount() const { return economySeatsCount; }

// --- Hàm nghiệp vụ ---
// Logic trung tâm để xử lý việc đặt vé một cách an toàn.
bool FlightInstance::bookSeats(SeatClass seatClass, int count) { // fix int count = 1
    if (count <= 0) return false; // Không thể đặt số lượng ghế âm hoặc bằng 0

    switch (seatClass) {
        case SeatClass::Economy:
            if (this->economyAvailable >= count) {
                this->economyAvailable -= count;
                return true; // Đặt thành công
            }
            break; // Nếu không đủ ghế, sẽ đi xuống và trả về false
        case SeatClass::Business:
            if (this->businessAvailable >= count) {
                this->businessAvailable -= count;
                return true; // Đặt thành công
            }
            break;
    }
    return false; // Không đủ ghế trống
}

// Logic trung tâm để xử lý việc hủy vé một cách an toàn.
void FlightInstance::releaseSeats(SeatClass seatClass, int count) { // fix int count = 1
    if (count <= 0) return; // Không thể hủy số lượng ghế âm hoặc bằng 0

    switch (seatClass) {
        case SeatClass::Economy: {
            int newCount = this->economyAvailable + count;
            // Đảm bảo số ghế hủy không bao giờ vượt quá tổng số ghế
            this->economyAvailable = (newCount > this->economyTotal) ? this->economyTotal : newCount;
            break;
        }
        case SeatClass::Business: {
            int newCount = this->businessAvailable + count;
            // Đảm bảo số ghế hủy không bao giờ vượt quá tổng số ghế
            this->businessAvailable = (newCount > this->businessTotal) ? this->businessTotal : newCount;
            break;
        }
    }
}

// --- Setters cho các thông tin có thể thay đổi ---
void FlightInstance::setFareEconomy(double fare) {
    this->fareEconomy = fare;
}
void FlightInstance::setFareBusiness(double fare) {
    this->fareBusiness = fare;
}

// --- Hàm tiện ích ---
bool FlightInstance::hasAvailableSeats() const {
    // Gọi getter tính toán để đảm bảo logic nhất quán
    return this->getTotalSeatsAvailable() > 0;
}

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
           std::to_string(this->economyTotal) + "|" +
           std::to_string(this->economyAvailable) + "|" +
           std::to_string(this->businessTotal) + "|" +
           std::to_string(this->businessAvailable) + "|" +
           std::to_string(this->fareEconomy) + "|" +
           std::to_string(this->fareBusiness) + "|" +
           std::to_string(this->businessSeatsCount) + "|" +
           std::to_string(this->economySeatsCount);
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

    std::string flightNum = line.substr(start, end - start);
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

    int ecoTotal = std::stoi(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    int ecoAvail = std::stoi(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    int busTotal = std::stoi(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    int busAvail = std::stoi(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    double fareEco = std::stod(line.substr(start, end - start));
    start = end + 1;
    
    // Check if we have the new fields (businessSeatsCount and economySeatsCount)
    std::string remaining = line.substr(start);
    double fareBus;
    int businessSeats = DEFAULT_BUSINESS_SEATS;
    int economySeats = DEFAULT_ECONOMY_SEATS;
    
    // Find next delimiter
    end = remaining.find('|');
    if (end == std::string::npos) {
        // Old format: only fareBusiness
        fareBus = std::stod(remaining);
        // Use total seats as defaults
        businessSeats = busTotal;
        economySeats = ecoTotal;
    } else {
        // New format: fareBusiness | businessSeatsCount | economySeatsCount
        fareBus = std::stod(remaining.substr(0, end));
        
        size_t start2 = end + 1;
        end = remaining.find('|', start2);
        if (end != std::string::npos) {
            businessSeats = std::stoi(remaining.substr(start2, end - start2));
            economySeats = std::stoi(remaining.substr(end + 1));
        } else {
            // Fallback if only one more field
            businessSeats = busTotal;
            economySeats = ecoTotal;
        }
    }

    // Dùng constructor public đã thay đổi
    FlightInstance instance(fId, flightNum, depDate, depTime, arrDate, arrTime, ecoTotal, busTotal, fareEco, fareBus);

    // Ghi đè lại các giá trị
    instance.overrideIdForLoad(id);
    instance.economyAvailable = ecoAvail;   
    instance.businessAvailable = busAvail;
    instance.businessSeatsCount = businessSeats;
    instance.economySeatsCount = economySeats;

    return instance;
}

// --- Helper cho việc nạp dữ liệu ---

void FlightInstance::overrideIdForLoad(const std::string& existingId) {
    this->instanceId = existingId;
}