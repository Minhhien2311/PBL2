#ifndef AIRPORTMANAGER_H
#define AIRPORTMANAGER_H

#include "DSA/SimpleMap.h"
#include "DSA/DynamicArray.h"
#include <string>

class FlightManager;

//  Quản lý danh sách sân bay
//  Ánh xạ 2 chiều: Display Name ↔ IATA Code. Ví dụ: "TP. Hồ Chí Minh (SGN)" ↔ "SGN"
class AirportManager {
private:
    SimpleMap<std::string, std::string> displayToIATA_;  // "Hà Nội (HAN)" → "HAN"
    SimpleMap<std::string, std::string> iataToDisplay_;  // "HAN" → "Hà Nội (HAN)"

    // Load danh sách sân bay mặc định (Việt Nam + quốc tế)
    void loadDefaultAirports();
    
public:
    // Constructor - tự động load danh sách sân bay mặc định
    AirportManager();

    // Destructor
    ~AirportManager() = default;
    // Thêm sân bay mới vào danh sách
    void addAirport(const std::string& displayName, const std::string& iataCode);

    // Lấy mã IATA từ tên hiển thị
    std::string getIATACode(const std::string& displayName) const;

    // Lấy tên hiển thị từ mã IATA
    std::string getDisplayName(const std::string& iataCode) const;

    // Lấy danh sách tất cả tên hiển thị (đã sắp xếp alphabet)
    DynamicArray<std::string> getAllDisplayNames() const;

    // Load thêm sân bay từ FlightManager, quét tất cả chuyến bay, tự động thêm sân bay chưa có
    void loadAirportsFromFlights(FlightManager* flightManager);

    // Kiểm tra sân bay đã tồn tại chưa
    bool hasAirport(const std::string& iataCode) const;

    // Lấy số lượng sân bay
    int getAirportCount() const;
};

#endif // AIRPORTMANAGER_H
