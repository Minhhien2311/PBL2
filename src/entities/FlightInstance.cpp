#include "C:/PBL2/include/entities/FlightInstance.h"
#include "C:/PBL2/include/utils/GenID.h"          // Cần để gọi hàm tạo ID
#include <iostream>                             // Cần cho hàm displayInfo

/* --- Constructor ---
    Chỉ cần nhận vào thông tin gốc, sau đó tự động:
    1. Sinh ID duy nhất.
    2. Khởi tạo số ghế trống ban đầu bằng tổng số ghế.
*/
FlightInstance::FlightInstance(const std::string& flightId,
                               const std::string& departureIso,
                               const std::string& arrivalIso,
                               int totalEconomySeats,
                               int totalBusinessSeats,
                               double fareEconomy,
                               double fareBusiness)
    : instanceId(IdGenerator::generateInstanceId()), // Tự động gọi hàm tạo ID
      flightId(flightId),
      departureIso(departureIso),
      arrivalIso(arrivalIso),
      // Đảm bảo các giá trị không âm
      economyTotal(totalEconomySeats),
      economyAvailable(totalEconomySeats), // Ban đầu, số ghế trống = tổng số ghế
      businessTotal(totalBusinessSeats),
      businessAvailable(totalBusinessSeats), // Ban đầu, số ghế trống = tổng số ghế
      fareEconomy(fareEconomy),
      fareBusiness(fareBusiness) {}

// --- Getters ---
const std::string& FlightInstance::getInstanceId() const { return instanceId; }
const std::string& FlightInstance::getFlightId() const { return flightId; }
const std::string& FlightInstance::getDepartureIso() const { return departureIso; }
const std::string& FlightInstance::getArrivalIso() const { return arrivalIso; }

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

// --- Hàm nghiệp vụ ---
// Logic trung tâm để xử lý việc đặt vé một cách an toàn.
bool FlightInstance::bookSeats(SeatClass seatClass, int count = 1) {
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
void FlightInstance::releaseSeats(SeatClass seatClass, int count = 1) {
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