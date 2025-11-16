#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H

#include <vector>
#include "entities/Route.h"           // Đối tượng tuyến đường gốc (Route)
#include "entities/Flight.h"          // Đối tượng chuyến bay cụ thể (Flight)
#include "DSA/HashTable.h"
#include <string>

class SeatManager;

class FlightManager {
private:
    // Lưu trữ một vector các con trỏ thay vì đối tượng
    std::vector<Route*> allRoutes;
    std::vector<Flight*> allFlights;

    // Thêm bảng băm để tra cứu bằng ID
    HashTable<std::string, Route*> routeIdTable;
    HashTable<std::string, Flight*> flightIdTable;
    
    // Lưu đường dẫn file để dùng trong saveAllData() và destructor
    std::string routesFilePath_;
    std::string flightsFilePath_;

    // SeatManager để quản lý sơ đồ chỗ ngồi
    SeatManager* seatManager_;

    // Route index for fast lookup
    struct RouteData {
        std::string routeKey;
        std::vector<Flight*> allFlights;  // sorted by datetime
        
        RouteData(const std::string& key) : routeKey(key) {}
        
        void addFlight(Flight* flight) {
            allFlights.push_back(flight);
        }
    };
    
    HashTable<std::string, RouteData*> routeIndex_;
    
    void buildRouteIndex();
    void sortFlightsByDateTime(std::vector<Flight*>& flights);
    static int compareDates(const std::string& date1, const std::string& date2);
    static int compareTimes(const std::string& time1, const std::string& time2);

    // Hàm xây dựng bảng băm
    void buildRouteIdTable();
    void buildFlightIdTable();

    void loadRoutesFromFile(const std::string& filePath);
    void loadFlightsFromFile(const std::string& filePath);

public:
    FlightManager();
    FlightManager(const std::string& routesFilePath, const std::string& flightsFilePath);
    ~FlightManager();

    // --- Các hàm tạo mới ---
    bool createNewRoute(const std::string& departureIATA,
                         const std::string& arrivalIATA);

    // Tạo mới chuyến bay cụ thể
    bool createNewFlight(const std::string& routeId,
                         const std::string& airline,
                         const std::string& flightNumber,
                         const std::string& departureDate,  
                         const std::string& departureTime,  
                         const std::string& arrivalDate,    
                         const std::string& arrivalTime,    
                         int totalCapacity,
                         int fareEconomy,
                         int fareBusiness);
    
    // --- Các hàm Update và Delete ---
    bool updateRoute(const std::string& routeId, const std::string& newDeparture, const std::string& newDestination);
    bool deleteRoute(const std::string& routeId);
    bool updateFlight(const std::string& flightId, const Flight& updatedFlight);
    bool deleteFlight(const std::string& flightId);

    // --- Các hàm tìm kiếm ---
    Route* findRouteById(const std::string& routeId) const;
    Flight* findFlightById(const std::string& flightId) const;
    std::vector<Flight*> findFlightsByRouteId(const std::string& routeId) const;
    std::vector<Route*> findRouteByRoute(const std::string& fromIATA, const std::string& toIATA) const;

    // --- Các hàm lưu ---
    bool saveRoutesToFiles(const std::string& routesFilePath) const;
    bool saveFlightsToFiles(const std::string& flightsFilePath) const;
    // Lưu tất cả dữ liệu (routes, flights)
    bool saveAllData();

    // --- Các hàm Getters ---
    const std::vector<Route*>& getAllRoutes() const;
    const std::vector<Flight*>& getAllFlights() const;
    SeatManager* getSeatManager() const;
    
    // --- Search and Filter Methods ---
    // Search criteria
    struct SearchCriteria {
        std::string fromIATA;           // Required
        std::string toIATA;             // Required
        std::string date;               // Optional: "" = all dates
        std::string airline;            // Optional: "" = all airlines
        int minPrice = 0;               // Optional: 0 = no min
        int maxPrice = 0;               // Optional: 0 = no max
        bool useAVLForPrice = false;    // false = linear, true = AVL
    };
    
    // Get all flights by route (sorted by datetime)
    std::vector<Flight*> getFlightsByRoute(
        const std::string& fromIATA,
        const std::string& toIATA
    ) const;
    
    // Filters (apply to vector)
    std::vector<Flight*> filterByDate(
        const std::vector<Flight*>& flights,
        const std::string& date
    ) const;
    
    std::vector<Flight*> filterByAirline(
        const std::vector<Flight*>& flights,
        const std::string& airline
    ) const;
    
    std::vector<Flight*> filterByPriceRange(
        const std::vector<Flight*>& flights,
        int minPrice,
        int maxPrice
    ) const;
    
    std::vector<Flight*> filterByPriceRangeAVL(
        const std::vector<Flight*>& flights,
        int minPrice,
        int maxPrice
    ) const;
    
    // Combined search with all filters
    std::vector<Flight*> searchFlights(
        const SearchCriteria& criteria
    ) const;
};

#endif // FLIGHT_MANAGER_H