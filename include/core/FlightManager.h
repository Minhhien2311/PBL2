#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H

#include <vector>
#include "entities/Route.h"
#include "entities/Flight.h"
#include "DSA/HashTable.h"
#include <string>

class SeatManager;

class FlightManager {
private:
    std::vector<Route*> allRoutes;
    std::vector<Flight*> allFlights;

    HashTable<std::string, Route*> routeIdTable;
    HashTable<std::string, Flight*> flightIdTable;
    
    std::string routesFilePath_;
    std::string flightsFilePath_;

    SeatManager* seatManager_;

    // Route index for fast lookup
    struct RouteData {
        std::string routeKey;
        std::vector<Flight*> allFlights;
        
        RouteData(const std::string& key) : routeKey(key) {}
        
        void addFlight(Flight* flight) {
            allFlights.push_back(flight);
        }
    };
    
    HashTable<std::string, RouteData*> routeIndex_;
    
    // RouteIndex management helpers
    bool addFlightToRouteIndex(Flight* flight);
    bool removeFlightFromRouteIndex(Flight* flight);
    bool updateFlightInRouteIndex(Flight* flight, const std::string& oldRouteId);
    
    void buildRouteIndex();
    void sortFlightsByDateTime(std::vector<Flight*>& flights);
    static int compareDates(const std::string& date1, const std::string& date2);
    static int compareTimes(const std::string& time1, const std::string& time2);

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
    bool saveAllData();

    // --- Các hàm Getters ---
    const std::vector<Route*>& getAllRoutes() const;
    const std::vector<Flight*>& getAllFlights() const;
    SeatManager* getSeatManager() const;
    
    // --- Search and Filter Methods ---
    struct SearchCriteria {
        std::string fromIATA;
        std::string toIATA;
        std::string date;
        std::string airline;
        int minPrice = 0;
        int maxPrice = 0;
        bool useAVLForPrice = false;
    };
    
    std::vector<Flight*> getFlightsByRoute(
        const std::string& fromIATA,
        const std::string& toIATA
    ) const;
    
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
    
    std::vector<Flight*> searchFlights(
        const SearchCriteria& criteria
    ) const;
};

#endif // FLIGHT_MANAGER_H