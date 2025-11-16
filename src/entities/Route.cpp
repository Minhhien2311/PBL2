#include "entities/Route.h" // <-- Đã đổi
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

// Constructor đã đổi tên class
Route::Route(const std::string& departureIATA,
             const std::string& arrivalIATA)
    : routeId(departureIATA + "-" + arrivalIATA),
      departureAirport(departureIATA),
      arrivalAirport(arrivalIATA)
{}

// Getter (trả const-ref/int để tránh copy)
const std::string& Route::getRouteId() const { return routeId; }
const std::string& Route::getDepartureAirport() const { return departureAirport; }
const std::string& Route::getArrivalAirport() const { return arrivalAirport; }

// --- Đọc/Ghi file cấu hình ---

// Chuyển đổi đối tượng thành một dòng string, ngăn cách bởi dấu '|'.
std::string Route::toRecordLine() const { // <-- Đã đổi tên class
    return this->routeId + "|" +
           this->departureAirport + "|" +
           this->arrivalAirport;
}

// Tạo đối tượng Route từ một dòng string.
Route Route::fromRecordLine(const std::string& line) { // <-- Đã đổi tên class
    size_t start = 0;
    size_t end = line.find('|');
    
    // Tách các thành phần từ chuỗi
    std::string id = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string departure = line.substr(start, end - start);
    start = end + 1;
    end = line.length(); // Phần cuối cùng

    std::string arrival = line.substr(start, end - start);

    // Kỹ thuật "Tạo tạm rồi sửa lỗi":
    // 1. Dùng constructor public để tạo một đối tượng Route.
    //    Lúc này, nó sẽ có một ID được tạo từ airline-departure-arrival.
    Route route(departure, arrival); // <-- Đã đổi

    // 2. Dùng hàm helper để ghi đè ID tạm thời bằng ID chính xác đã đọc từ file.
    route.overrideIdForLoad(id); // <-- Đã đổi

    return route; // <-- Đã đổi
}

// --- Helper cho việc nạp dữ liệu ---

void Route::overrideIdForLoad(const std::string& existingId) { // <-- Đã đổi
    this->routeId = existingId; // <-- Đã đổi
}