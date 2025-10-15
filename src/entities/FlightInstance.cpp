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
           this->departureIso + "|" +
           this->arrivalIso + "|" +
           std::to_string(this->economyTotal) + "|" +
           std::to_string(this->economyAvailable) + "|" +
           std::to_string(this->businessTotal) + "|" +
           std::to_string(this->businessAvailable) + "|" +
           std::to_string(this->fareEconomy) + "|" +
           std::to_string(this->fareBusiness);
}

// Tạo đối tượng FlightInstance từ một dòng string.
FlightInstance FlightInstance::fromRecordLine(const std::string& line) {
    size_t start = 0;
    size_t end = line.find('|');
    
    // Tách các thành phần từ chuỗi
    std::string id = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string fId = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string depIso = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string arrIso = line.substr(start, end - start);
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
    end = line.length();

    double fareBus = std::stod(line.substr(start, end - start));

    // Kỹ thuật "Tạo tạm rồi sửa lỗi":
    // 1. Dùng constructor public để tạo đối tượng. Nó sẽ có ID tạm thời.
    FlightInstance instance(fId, depIso, arrIso, ecoTotal, busTotal, fareEco, fareBus);

    // 2. Ghi đè lại các giá trị có thể bị tính toán sai bởi constructor.
    //    Trong trường hợp này, ID và số ghế trống cần được cập nhật.
    instance.overrideIdForLoad(id);
    instance.economyAvailable = ecoAvail;   // Ghi đè trực tiếp vì không có setter
    instance.businessAvailable = busAvail; // Ghi đè trực tiếp vì không có setter

    return instance;
}

// --- Helper cho việc nạp dữ liệu ---

void FlightInstance::overrideIdForLoad(const std::string& existingId) {
    this->instanceId = existingId;
}