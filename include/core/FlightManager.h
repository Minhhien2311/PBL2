#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H

#include <vector>                        // STL để lưu trữ
#include "entities/Flight.h"            // Đối tượng tuyến bay gốc
#include "entities/FlightInstance.h"    // Đối tượng chuyến bay cụ thể
#include "DSA/HashTable.h"
#include <string>

class SeatManager;

class FlightManager {
private:
    // Lưu trữ một vector các con trỏ thay vì đối tượng
    std::vector<Flight*> allFlights;         
    std::vector<FlightInstance*> allInstances;

    // Thêm bảng băm để tra cứu bằng ID
    HashTable<std::string, Flight*> flightIdTable;
    HashTable<std::string, FlightInstance*> instanceIdTable;
    
    // Lưu đường dẫn file để dùng trong saveAllData() và destructor
    std::string flightsFilePath_;
    std::string instancesFilePath_;

    // SeatManager để quản lý sơ đồ chỗ ngồi
    SeatManager* seatManager_;

    // Route index for fast lookup
    struct RouteData {
        std::string routeKey;
        std::vector<FlightInstance*> allInstances;  // sorted by datetime
        
        RouteData(const std::string& key) : routeKey(key) {}
        
        void addInstance(FlightInstance* instance) {
            allInstances.push_back(instance);
        }
    };
    
    HashTable<std::string, RouteData*> routeIndex_;
    
    void buildRouteIndex();
    void sortInstancesByDateTime(std::vector<FlightInstance*>& instances);
    static int compareDates(const std::string& date1, const std::string& date2);
    static int compareTimes(const std::string& time1, const std::string& time2);

    // Hàm xây dựng bảng băm
    void buildFlightIdTable();
    void buildInstanceIdTable();

    void loadFlightsFromFile(const std::string& filePath);
    void loadInstancesFromFile(const std::string& filePath);

public:
    FlightManager();
    FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath);
    ~FlightManager();

    // --- Các hàm tạo mới ---
    bool createNewFlight(const std::string& airline,
                         const std::string& departureIATA,
                         const std::string& arrivalIATA);

    // Tạo mới chuyến bay cụ thể dựa trên
    bool createNewInstance(const std::string& flightId,
                           const std::string& flightNumber,
                           const std::string& departureDate,  
                           const std::string& departureTime,  
                           const std::string& arrivalDate,    
                           const std::string& arrivalTime,    
                           int totalCapacity,
                           int fareEconomy,
                           int fareBusiness);
    
    // --- Các hàm Update và Delete ---
    bool updateFlight(const std::string& flightId, const std::string& newAirline, 
                      const std::string& newDeparture, const std::string& newDestination);
    bool deleteFlight(const std::string& flightId);
    bool updateInstance(const std::string& instanceId, const FlightInstance& updatedInstance);
    bool deleteInstance(const std::string& instanceId);

    // --- Các hàm tìm kiếm ---
    Flight* findFlightById(const std::string& flightId) const;
    FlightInstance* findInstanceById(const std::string& instanceId) const;
    std::vector<FlightInstance*> findInstancesByFlightId(const std::string& flightId) const;
    std::vector<Flight*> findFlightByRoute(const std::string& fromIATA, const std::string& toIATA) const;

    // --- Các hàm lưu ---
    bool saveFlightsToFiles(const std::string& flightsFilePath) const;
    bool saveInstancesToFiles(const std::string& instancesFilePath) const;
    // Lưu tất cả dữ liệu (flights, instances)
    bool saveAllData();

    // --- Các hàm Getters ---
    const std::vector<Flight*>& getAllFlights() const;
    const std::vector<FlightInstance*>& getAllInstances() const;
    SeatManager* getSeatManager() const;
    
    // --- Search and Filter Methods ---
    // Search criteria
    struct SearchCriteria {
        std::string fromIATA;      // Required
        std::string toIATA;        // Required
        std::string date;          // Optional: "" = all dates
        std::string airline;       // Optional: "" = all airlines
        int minPrice = 0;          // Optional: 0 = no min
        int maxPrice = 0;          // Optional: 0 = no max
        bool useAVLForPrice = false;  // false = linear, true = AVL
    };
    
    // Get all flights by route (sorted by datetime)
    std::vector<FlightInstance*> getFlightsByRoute(
        const std::string& fromIATA,
        const std::string& toIATA
    ) const;
    
    // Filters (apply to vector)
    std::vector<FlightInstance*> filterByDate(
        const std::vector<FlightInstance*>& flights,
        const std::string& date
    ) const;
    
    std::vector<FlightInstance*> filterByAirline(
        const std::vector<FlightInstance*>& flights,
        const std::string& airline
    ) const;
    
    std::vector<FlightInstance*> filterByPriceRange(
        const std::vector<FlightInstance*>& flights,
        int minPrice,
        int maxPrice
    ) const;
    
    std::vector<FlightInstance*> filterByPriceRangeAVL(
        const std::vector<FlightInstance*>& flights,
        int minPrice,
        int maxPrice
    ) const;
    
    // Combined search with all filters
    std::vector<FlightInstance*> searchFlights(
        const SearchCriteria& criteria
    ) const;
};

#endif