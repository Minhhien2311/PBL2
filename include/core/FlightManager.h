#pragma once

#include <string>
#include <vector>
#include <ctime>
#include "entities/Route.h"
#include "entities/Flight.h"
#include "DSA/HashTable.h"
#include "DSA/AVLTree.h"

class SeatManager;

class FlightManager {
public:
    struct SearchCriteria {
        std::string fromIATA;
        std::string toIATA;
        std::string date;
        std::string airline;
        int minPrice = 0;
        int maxPrice = 0;
        bool useAVLForPrice = false;
    };

    // --- Constructor & Destructor ---
    FlightManager();
    FlightManager(const std::string& routesFilePath, const std::string& flightsFilePath);
    ~FlightManager();

    // --- Route Management (Quản lý Tuyến bay) ---
    bool createNewRoute(const std::string& departureIATA, const std::string& arrivalIATA);
    bool updateRoute(const std::string& routeId, const std::string& newDeparture, const std::string& newDestination);
    bool deleteRoute(const std::string& routeId);
    
    Route* findRouteById(const std::string& routeId) const;
    std::vector<Route*> findRouteByRoute(const std::string& fromIATA, const std::string& toIATA) const;

    // --- Flight Management (Quản lý Chuyến bay) ---
    bool createNewFlight(const std::string& routeId, const std::string& airline, const std::string& flightNumber,
                         const std::string& departureDate, const std::string& departureTime,
                         const std::string& arrivalDate, const std::string& arrivalTime, 
                         int totalCapacity, int availableSeats, int fareEconomy, int fareBusiness);
    bool updateFlight(const std::string& flightId, const Flight& updatedFlight);
    bool deleteFlight(const std::string& flightId);

    Flight* findFlightById(const std::string& flightId) const;
    std::vector<Flight*> findFlightsByRouteId(const std::string& routeId) const;

    // --- Search & Filter (Tìm kiếm & Lọc chuyến bay) ---
    // Tìm kiếm tổng hợp theo tiêu chí
    std::vector<Flight*> searchFlights(const SearchCriteria& criteria) const;
    
    // Lấy chuyến bay theo tuyến (cơ bản)
    std::vector<Flight*> getFlightsByRoute(const std::string& fromIATA, const std::string& toIATA) const;
    
    // Lấy chuyến bay tương lai (onlyFuture=true: chưa bay, false: tất cả)
    std::vector<Flight*> getFutureFlights(bool onlyFuture = false);

    // Các bộ lọc chi tiết
    std::vector<Flight*> filterByDate(const std::vector<Flight*>& flights, const std::string& date) const;
    std::vector<Flight*> filterByAirline(const std::vector<Flight*>& flights, const std::string& airline) const;
    std::vector<Flight*> filterByPriceRangeAVL(const std::vector<Flight*>& flights, int minPrice, int maxPrice) const;

    // --- Data Access (Truy xuất dữ liệu) ---
    const std::vector<Route*>& getAllRoutes() const;
    const std::vector<Flight*>& getAllFlights() const;
    SeatManager* getSeatManager() const;

    // --- Persistence (Lưu trữ file) ---
    bool saveRoutesToFiles(const std::string& routesFilePath) const;
    bool saveFlightsToFiles(const std::string& flightsFilePath) const;
    bool saveAllData();

private:
    // --- Data Storage ---
    std::vector<Route*> allRoutes;
    std::vector<Flight*> allFlights;
    HashTable<std::string, Route*> routeIdTable;
    HashTable<std::string, Flight*> flightIdTable;
    
    SeatManager* seatManager_;
    std::string routesFilePath_;
    std::string flightsFilePath_;

    // --- Indexing Structures ---
    struct RouteData {
        std::string routeKey;
        std::vector<Flight*> allFlights;
        RouteData(const std::string& key) : routeKey(key) {}
        void addFlight(Flight* flight) { allFlights.push_back(flight); }
    };
    HashTable<std::string, RouteData*> routeIndex_;
    
    // Key: time_t, Value: List of FlightIDs
    AVLTree<time_t, std::vector<std::string>> flightTimeTree;
    // Key: EconomyPrice, Value: List of FlightIDs
    AVLTree<int, std::vector<std::string>> flightPriceTree; 

    // --- Internal Helpers: Indexing Maintenance ---
    void buildRouteIndex();
    void buildRouteIdTable();
    void buildFlightIdTable();

    // AVL Tree Helpers
    void indexFlightTime(const Flight& flight);
    time_t getFlightTimeKey(const Flight& flight) const;
    void addFlightToTimeIndex(Flight* flight);
    void removeFlightFromTimeIndex(Flight* flight);
    void removeFlightFromTimeIndexByKey(time_t key, const std::string& flightId);
    void moveFlightTimeIndex(Flight* flight, time_t oldKey);

    void addFlightToPriceIndex(Flight* flight);
    void removeFlightFromPriceIndex(Flight* flight);
    void moveFlightPriceIndex(Flight* flight, int oldPrice);

    // Route Index Helpers
    bool addFlightToRouteIndex(Flight* flight);
    bool removeFlightFromRouteIndex(Flight* flight);
    bool updateFlightInRouteIndex(Flight* flight, const std::string& oldRouteId);

    // --- Internal Helpers: Utils ---
    void loadRoutesFromFile(const std::string& filePath);
    void loadFlightsFromFile(const std::string& filePath);
    void sortFlightsByDateTime(std::vector<Flight*>& flights);
    static int compareDates(const std::string& date1, const std::string& date2);
    static int compareTimes(const std::string& time1, const std::string& time2);
};