#ifndef AIRPORTMANAGER_H
#define AIRPORTMANAGER_H

#include "DSA/SimpleMap.h"
#include "DSA/DynamicArray.h"
#include <string>

class FlightManager; // Forward declaration

/**
 * @brief Quản lý danh sách sân bay
 * @details Ánh xạ 2 chiều: Display Name ↔ IATA Code
 *          Ví dụ: "TP. Hồ Chí Minh (SGN)" ↔ "SGN"
 */
class AirportManager {
private:
    SimpleMap<std::string, std::string> displayToIATA_;  // "Hà Nội (HAN)" → "HAN"
    SimpleMap<std::string, std::string> iataToDisplay_;  // "HAN" → "Hà Nội (HAN)"
    
    /**
     * @brief Load danh sách sân bay mặc định (Việt Nam + quốc tế)
     */
    void loadDefaultAirports();
    
public:
    /**
     * @brief Constructor - tự động load danh sách sân bay mặc định
     */
    AirportManager();
    
    /**
     * @brief Destructor
     */
    ~AirportManager() = default;
    
    /**
     * @brief Thêm sân bay mới vào danh sách
     * @param displayName Tên hiển thị (VD: "Hà Nội (HAN)")
     * @param iataCode Mã IATA (VD: "HAN")
     */
    void addAirport(const std::string& displayName, const std::string& iataCode);
    
    /**
     * @brief Lấy mã IATA từ tên hiển thị
     * @param displayName Tên hiển thị
     * @return Mã IATA, hoặc chuỗi rỗng nếu không tìm thấy
     */
    std::string getIATACode(const std::string& displayName) const;
    
    /**
     * @brief Lấy tên hiển thị từ mã IATA
     * @param iataCode Mã IATA
     * @return Tên hiển thị, hoặc chuỗi rỗng nếu không tìm thấy
     */
    std::string getDisplayName(const std::string& iataCode) const;
    
    /**
     * @brief Lấy danh sách tất cả tên hiển thị (đã sắp xếp alphabet)
     * @return DynamicArray chứa tất cả tên hiển thị
     */
    DynamicArray<std::string> getAllDisplayNames() const;
    
    /**
     * @brief Load thêm sân bay từ FlightManager
     * @details Quét tất cả chuyến bay, tự động thêm sân bay chưa có
     * @param flightManager Con trỏ đến FlightManager
     */
    void loadAirportsFromFlights(FlightManager* flightManager);
    
    /**
     * @brief Kiểm tra sân bay đã tồn tại chưa
     * @param iataCode Mã IATA cần kiểm tra
     * @return true nếu đã tồn tại, false nếu chưa
     */
    bool hasAirport(const std::string& iataCode) const;
    
    /**
     * @brief Lấy số lượng sân bay
     * @return Số lượng sân bay trong danh sách
     */
    int getAirportCount() const;
};

#endif // AIRPORTMANAGER_H
