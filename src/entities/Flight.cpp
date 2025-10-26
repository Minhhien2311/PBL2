#include "entities/Flight.h"
#include "utils/GenID.h"

Flight::Flight( const std::string& number,
                const std::string& airline,
                const std::string& departureIATA,
                const std::string& arrivalIATA)
    : flightId(IdGenerator::generateFlightId()), flightNumber(number), airline(airline), departureAirport(departureIATA), arrivalAirport(arrivalIATA)
{}

// Getter (trả const-ref/int để tránh copy)
const std::string& Flight::getFlightId() const { return flightId; }
const std::string& Flight::getFlightNumber() const { return flightNumber; }
const std::string& Flight::getAirline() const { return airline; }
const std::string& Flight::getDepartureAirport() const { return departureAirport; }
const std::string& Flight::getArrivalAirport() const { return arrivalAirport; }

// --- Đọc/Ghi file cấu hình ---

// Chuyển đổi đối tượng thành một dòng string, ngăn cách bởi dấu '|'.
std::string Flight::toRecordLine() const {
    return this->flightId + "|" +
           this->flightNumber + "|" +
           this->airline + "|" +
           this->departureAirport + "|" +
           this->arrivalAirport;
}

// Tạo đối tượng Flight từ một dòng string.
Flight Flight::fromRecordLine(const std::string& line) {
    size_t start = 0;
    size_t end = line.find('|');
    
    // Tách các thành phần từ chuỗi
    std::string id = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string number = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string airline = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string departure = line.substr(start, end - start);
    start = end + 1;
    end = line.length(); // Phần cuối cùng

    std::string arrival = line.substr(start, end - start);

    // Kỹ thuật "Tạo tạm rồi sửa lỗi":
    // 1. Dùng constructor public để tạo một đối tượng Flight.
    //    Lúc này, nó sẽ có một ID tạm thời được sinh tự động.
    Flight flight(number, airline, departure, arrival);

    // 2. Dùng hàm helper để ghi đè ID tạm thời bằng ID chính xác đã đọc từ file.
    flight.overrideIdForLoad(id);

    return flight;
}

// --- Helper cho việc nạp dữ liệu ---

void Flight::overrideIdForLoad(const std::string& existingId) {
    this->flightId = existingId;
}

// Tiện ích nhẹ (không dính I/O hoặc DB)
std::string Flight::getFlightInfo() const {
    // Sẽ triển khai sau
}