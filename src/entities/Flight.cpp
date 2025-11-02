#include "entities/Flight.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

// Code lại map
inline std::string airlineCode(std::string_view name) {
    // normalize: lowercase + gọn khoảng trắng
    std::string key(name);
    std::transform(key.begin(), key.end(), key.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    auto trim = [](std::string& s){
        auto ns = [](unsigned char c){ return !std::isspace(c); };
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), ns));
        s.erase(std::find_if(s.rbegin(), s.rend(), ns).base(), s.end());
    };
    trim(key);
    // thay nhiều khoảng trắng bằng 1
    {
        std::string t; t.reserve(key.size());
        bool sp=false;
        for (unsigned char c: key){
            bool issp = std::isspace(c);
            if (issp){ if(!sp) t.push_back(' '); }
            else t.push_back(c);
            sp = issp;
        }
        key.swap(t);
    }

    struct M { const char* needle; const char* code; };
    static const M map[] = {
        {"vietjet air","VJ"}, {"vietjet","VJ"}, {"vj","VJ"},
        {"bamboo airways","QH"}, {"bamboo","QH"}, {"qh","QH"},
        {"vietnam airlines","VN"}, {"vietnam airline","VN"}, {"vn","VN"},
        {"pacific airlines","BL"}, {"jetstar pacific","BL"}, {"pacific","BL"}, {"bl","BL"},
        {"vietravel airlines","VU"}, {"vietravel","VU"}, {"vu","VU"},
    };
    for (auto& m : map)
        if (key == m.needle || key.find(m.needle) != std::string::npos)
            return m.code;

    // Fallback: ghép chữ cái đầu các từ (tối đa 3)
    std::string code; code.reserve(3);
    bool nw = true;
    for (unsigned char c : std::string(name)) {
        if (std::isalpha(c)) {
            if (nw) { code.push_back(std::toupper(c)); if (code.size() >= 3) break; nw = false; }
        } else nw = std::isspace(c);
    }
    if (code.empty()) code = "XX";
    return code;
}


Flight::Flight(const std::string& airline,
               const std::string& departureIATA,
               const std::string& arrivalIATA)
    : flightId(airlineCode(airline) + "-" + departureIATA + "-" + arrivalIATA),
      airline(airline), 
      departureAirport(departureIATA), 
      arrivalAirport(arrivalIATA)
{}

// Getter (trả const-ref/int để tránh copy)
const std::string& Flight::getFlightId() const { return flightId; }
const std::string& Flight::getAirline() const { return airline; }
const std::string& Flight::getDepartureAirport() const { return departureAirport; }
const std::string& Flight::getArrivalAirport() const { return arrivalAirport; }

// --- Đọc/Ghi file cấu hình ---

// Chuyển đổi đối tượng thành một dòng string, ngăn cách bởi dấu '|'.
std::string Flight::toRecordLine() const {
    return this->flightId + "|" +
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

    std::string airline = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string departure = line.substr(start, end - start);
    start = end + 1;
    end = line.length(); // Phần cuối cùng

    std::string arrival = line.substr(start, end - start);

    // Kỹ thuật "Tạo tạm rồi sửa lỗi":
    // 1. Dùng constructor public để tạo một đối tượng Flight.
    //    Lúc này, nó sẽ có một ID được tạo từ airline-departure-arrival.
    Flight flight(airline, departure, arrival);

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
    return this->airline + " " + this->departureAirport + " -> " + this->arrivalAirport;
}