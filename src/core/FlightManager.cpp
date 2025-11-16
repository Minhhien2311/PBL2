#include "core/FlightManager.h"
#include "core/SeatManager.h"
#include <fstream>
#include <string>
#include <sstream>

// --- Constructor & Destructor ---

// Tên tham số đã được đổi: flightsFilePath -> routesFilePath, instancesFilePath -> flightsFilePath
FlightManager::FlightManager(const std::string& routesFilePath, const std::string& flightsFilePath) 
    : routesFilePath_(routesFilePath), flightsFilePath_(flightsFilePath), seatManager_(nullptr) {
    
    // Initialize SeatManager at startup
    seatManager_ = new SeatManager("data/seat_maps.txt", "data/seat_config.txt");
    
    // Đổi tên hàm load
    this->loadRoutesFromFile(routesFilePath);
    this->loadFlightsFromFile(flightsFilePath);

    // Đổi tên hàm xây dựng bảng băm
    this->buildRouteIdTable();
    this->buildFlightIdTable();
    this->buildRouteIndex();
}

FlightManager::~FlightManager() {
    // Save all data before destruction
    saveAllData();
    
    // Cleanup SeatManager
    delete seatManager_;
    
    // Cleanup routeIndex_ - need to iterate through hash table and delete RouteData
    // Since we don't have an iterator, we'll track which RouteData we've deleted
    std::vector<RouteData*> deletedRoutes;
    // Dùng allRoutes (trước là allFlights)
    for (Route* route : allRoutes) { 
        if (!route) continue;
        std::string routeKey = route->getDepartureAirport() + "_" + 
                               route->getArrivalAirport();
        RouteData** dataPtr = routeIndex_.find(routeKey);
        if (dataPtr && *dataPtr) {
            // Check if we haven't already deleted this RouteData
            bool alreadyDeleted = false;
            for (RouteData* rd : deletedRoutes) {
                if (rd == *dataPtr) {
                    alreadyDeleted = true;
                    break;
                }
            }
            if (!alreadyDeleted) {
                deletedRoutes.push_back(*dataPtr);
                delete *dataPtr;
            }
        }
    }
   
    // Đổi tên vector
    for (int i = 0; i < allRoutes.size(); i++) {
        delete allRoutes[i]; 
    }
    for (int i = 0; i < allFlights.size(); i++) {
        delete allFlights[i]; 
    }
}
// --- Hàm trợ giúp nội bộ ---

// Đổi tên hàm loadFlightsFromFile -> loadRoutesFromFile
void FlightManager::loadRoutesFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // Đổi Flight -> Route
                Route routeOnStack = Route::fromRecordLine(line);
                this->allRoutes.push_back(new Route(routeOnStack));
            }
        }
        file.close();
    }
}

// Đổi tên hàm loadInstancesFromFile -> loadFlightsFromFile
void FlightManager::loadFlightsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // Đổi FlightInstance -> Flight
                Flight flightOnStack = Flight::fromRecordLine(line);
                this->allFlights.push_back(new Flight(flightOnStack));
            }
        }
        file.close();
    }
}

// Xây dựng bảng băm
// Đổi tên buildFlightIdTable -> buildRouteIdTable
void FlightManager::buildRouteIdTable() {
    for (int i = 0; i < allRoutes.size(); ++i) {
        if (allRoutes[i] != nullptr) {
            // Đổi flightIdTable -> routeIdTable, getFlightId -> getRouteId
            routeIdTable.insert(allRoutes[i]->getRouteId(), allRoutes[i]);
        }
    }
}

// Đổi tên buildInstanceIdTable -> buildFlightIdTable
void FlightManager::buildFlightIdTable() {
    for (int i = 0; i < allFlights.size(); ++i) {
        if (allFlights[i] != nullptr) {
            // Đổi instanceIdTable -> flightIdTable, getInstanceId -> getFlightId
            flightIdTable.insert(allFlights[i]->getFlightId(), allFlights[i]);
        }
    }
}

// --- Chức năng Tạo mới (Create) ---

// Đổi createNewFlight -> createNewRoute
bool FlightManager::createNewRoute( const std::string& departureIATA,
                                    const std::string& arrivalIATA) {
    if (departureIATA.empty() || arrivalIATA.empty()) return false;
    
    // Kiểm tra trùng lặp bằng ID mới (airline-departure-arrival)
    std::string newId = departureIATA + "-" + arrivalIATA;
    // Đổi findFlightById -> findRouteById
    if (findRouteById(newId) != nullptr) return false;

    // Đổi Flight -> Route
    Route* newRoute = new Route(departureIATA, arrivalIATA);
    this->allRoutes.push_back(newRoute);
    
    // Cập nhật bảng băm ID
    this->routeIdTable.insert(newRoute->getRouteId(), newRoute);
    return true;
}

// Đổi createNewInstance -> createNewFlight
bool FlightManager::createNewFlight(const std::string& routeId, // Đổi flightId -> routeId
                                    const std::string& flightNumber,
                                    const std::string& airline,
                                    const std::string& departureDate,
                                    const std::string& departureTime,
                                    const std::string& arrivalDate,
                                    const std::string& arrivalTime,
                                    int totalCapacity,
                                    int fareEconomy,
                                    int fareBusiness) {
    // Kiểm tra Route (Flight cũ) gốc bằng HashTable
    if (findRouteById(routeId) == nullptr) return false; // Đổi hàm
    if (totalCapacity <= 0 || fareEconomy <= 0.0 || fareBusiness <= 0.0) return false;
    if (routeId.empty() || flightNumber.empty() || airline.empty() || departureDate.empty() || departureTime.empty() || arrivalDate.empty() || arrivalTime.empty()) return false;

    // Đổi FlightInstance -> Flight
    Flight* newFlight = new Flight(
        routeId, flightNumber, airline, departureDate, departureTime, arrivalDate, arrivalTime, 
        totalCapacity, fareEconomy, fareBusiness
    );
    this->allFlights.push_back(newFlight);
    
    // Cập nhật bảng băm ID
    this->flightIdTable.insert(newFlight->getFlightId(), newFlight);
    return true;
}

// --- Chức năng Đọc/Tìm kiếm (Read) ---

// Đổi findFlightById -> findRouteById
Route* FlightManager::findRouteById(const std::string& routeId) const {
    Route** routePtrPtr = routeIdTable.find(routeId); // Đổi
    return (routePtrPtr != nullptr) ? *routePtrPtr : nullptr;
}

// Đổi findInstanceById -> findFlightById
Flight* FlightManager::findFlightById(const std::string& flightId) const {
    Flight** flightPtrPtr = flightIdTable.find(flightId); // Đổi
    return (flightPtrPtr != nullptr) ? *flightPtrPtr : nullptr;
}

// Đổi findInstancesByFlightId -> findFlightsByRouteId
std::vector<Flight*> FlightManager::findFlightsByRouteId(const std::string& routeId) const {
    std::vector<Flight*> results; // Đổi
    for (size_t i = 0; i < allFlights.size(); ++i) { // Đổi
        if (allFlights[i]->getRouteId() == routeId) { // Đổi
            results.push_back(allFlights[i]); // Đổi
        }
    }
    return results;
}

// Đổi findFlightByRoute -> findRouteByRoute
std::vector<Route*> FlightManager::findRouteByRoute(const std::string& fromIATA, const std::string& toIATA) const {
    std::vector<Route*> results; // Đổi
    for (size_t i = 0; i < allRoutes.size(); ++i) { // Đổi
        if (allRoutes[i]->getDepartureAirport() == fromIATA && allRoutes[i]->getArrivalAirport() == toIATA) { // Đổi
            results.push_back(allRoutes[i]); // Đổi
        }
    }
    return results;
}

// --- Chức năng Lưu trữ (Persistence) ---

// Đổi saveFlightsToFiles -> saveRoutesToFiles
bool FlightManager::saveRoutesToFiles(const std::string& routesFilePath) const {
    std::ofstream routesFile(routesFilePath); // Đổi
    if (!routesFile.is_open()) return false;
    for (size_t i = 0; i < allRoutes.size(); ++i) { // Đổi
        routesFile << allRoutes[i]->toRecordLine() << "\n"; // Đổi
    }
    routesFile.close(); // Đổi
    return true;
}

// Đổi saveInstancesToFiles -> saveFlightsToFiles
bool FlightManager::saveFlightsToFiles(const std::string& flightsFilePath) const {
    std::ofstream flightsFile(flightsFilePath); // Đổi
    if (!flightsFile.is_open()) return false;
    for (size_t i = 0; i < allFlights.size(); ++i) { // Đổi
        flightsFile << allFlights[i]->toRecordLine() << "\n"; // Đổi
    }
    flightsFile.close(); // Đổi
    return true;
}

// --- Getters ---
// Đổi getAllFlights -> getAllRoutes
const std::vector<Route*>& FlightManager::getAllRoutes() const{
    return this->allRoutes;
}     

// Đổi getAllInstances -> getAllFlights
const std::vector<Flight*>& FlightManager::getAllFlights() const{
    return this->allFlights;
}

// --- Save All Data ---
bool FlightManager::saveAllData() {
    // Đổi tên biến và hàm
    bool routesSaved = saveRoutesToFiles(routesFilePath_);
    bool flightsSaved = saveFlightsToFiles(flightsFilePath_);
    return routesSaved && flightsSaved;
}

// --- Update and Delete Functions ---

// Đổi updateFlight -> updateRoute
bool FlightManager::updateRoute(const std::string& routeId,
                                const std::string& newDeparture, 
                                const std::string& newDestination) {
    Route* route = findRouteById(routeId); // Đổi
    if (!route) return false;
    
    Route* newRoute = new Route(newDeparture, newDestination); // Đổi
    
    for (int i = 0; i < allRoutes.size(); i++) { // Đổi
        if (allRoutes[i]->getRouteId() == routeId) { // Đổi
            routeIdTable.remove(routeId); // Đổi
            
            delete allRoutes[i]; // Đổi
            allRoutes[i] = newRoute; // Đổi
            
            routeIdTable.insert(newRoute->getRouteId(), newRoute); // Đổi
            
            saveRoutesToFiles(routesFilePath_); // Đổi
            return true;
        }
    }
    
    delete newRoute; // Đổi
    return false;
}

// Đổi deleteFlight -> deleteRoute
bool FlightManager::deleteRoute(const std::string& routeId) {
    for (int i = 0; i < allRoutes.size(); i++) { // Đổi
        if (allRoutes[i]->getRouteId() == routeId) { // Đổi
            routeIdTable.remove(routeId); // Đổi
            
            delete allRoutes[i]; // Đổi
            allRoutes.erase(allRoutes.begin() + i); // Đổi
            
            saveRoutesToFiles(routesFilePath_); // Đổi
            return true;
        }
    }
    return false;
}

// Đổi updateInstance -> updateFlight
bool FlightManager::updateFlight(const std::string& flightId, 
                                 const Flight& updatedFlight) { // Đổi
    Flight* flight = findFlightById(flightId); // Đổi
    if (!flight) return false;
    
    *flight = updatedFlight; // Đổi
    
    saveFlightsToFiles(flightsFilePath_); // Đổi
    return true;
}

// Đổi deleteInstance -> deleteFlight
bool FlightManager::deleteFlight(const std::string& flightId) {
    for (int i = 0; i < allFlights.size(); i++) { // Đổi
        if (allFlights[i]->getFlightId() == flightId) { // Đổi
            flightIdTable.remove(flightId); // Đổi
            
            delete allFlights[i]; // Đổi
            allFlights.erase(allFlights.begin() + i); // Đổi
            
            saveFlightsToFiles(flightsFilePath_); // Đổi
            return true;
        }
    }
    return false;
}

SeatManager* FlightManager::getSeatManager() const {
    return seatManager_;
}

// --- Route Index and Search Implementation ---

void FlightManager::buildRouteIndex() {
    // Đổi instance -> flight, allInstances -> allFlights
    for (Flight* flight : allFlights) {
        if (!flight) continue;
        
        // Đổi instance->getFlightId() -> flight->getRouteId()
        // Đổi findFlightById -> findRouteById
        // Đổi Flight* flight -> Route* route
        Route* route = findRouteById(flight->getRouteId());
        if (!route) continue;
        
        std::string routeKey = route->getDepartureAirport() + "_" + 
                               route->getArrivalAirport();
        
        RouteData** dataPtr = routeIndex_.find(routeKey);
        
        if (dataPtr == nullptr) {
            RouteData* newData = new RouteData(routeKey);
            newData->addFlight(flight); // Đổi addInstance -> addFlight
            routeIndex_.insert(routeKey, newData);
        } else {
            (*dataPtr)->addFlight(flight); // Đổi addInstance -> addFlight
        }
    }
    
    // Sort each route by datetime
    std::vector<std::string> sortedRoutes;
    // Đổi allFlights -> allRoutes
    for (Route* route : allRoutes) {
        if (!route) continue;
        std::string routeKey = route->getDepartureAirport() + "_" + 
                               route->getArrivalAirport();
        
        bool alreadySorted = false;
        for (const std::string& sorted : sortedRoutes) {
            if (sorted == routeKey) {
                alreadySorted = true;
                break;
            }
        }
        
        if (!alreadySorted) {
            RouteData** dataPtr = routeIndex_.find(routeKey);
            if (dataPtr && *dataPtr) {
                // Đổi sortInstancesByDateTime -> sortFlightsByDateTime
                // Đổi (*dataPtr)->allInstances -> (*dataPtr)->allFlights
                sortFlightsByDateTime((*dataPtr)->allFlights);
                sortedRoutes.push_back(routeKey);
            }
        }
    }
}

// Đổi sortInstancesByDateTime -> sortFlightsByDateTime
void FlightManager::sortFlightsByDateTime(std::vector<Flight*>& flights) {
    // Bubble sort with custom comparator
    for (size_t i = 0; i < flights.size(); i++) {
        for (size_t j = i + 1; j < flights.size(); j++) {
            if (!flights[i] || !flights[j]) continue;
            
            int dateCmp = compareDates(flights[i]->getDepartureDate(),
                                       flights[j]->getDepartureDate());
            if (dateCmp > 0) {
                std::swap(flights[i], flights[j]);
            } else if (dateCmp == 0) {
                int timeCmp = compareTimes(flights[i]->getDepartureTime(),
                                           flights[j]->getDepartureTime());
                if (timeCmp > 0) {
                    std::swap(flights[i], flights[j]);
                }
            }
        }
    }
}

// (compareDates và compareTimes là static, không thay đổi)
int FlightManager::compareDates(const std::string& date1, const std::string& date2) {
    // ... (logic không đổi) ...
    if (date1.length() != 10 || date2.length() != 10 ||
        date1[2] != '/' || date1[5] != '/' ||
        date2[2] != '/' || date2[5] != '/') {
        return 0; // Invalid format, treat as equal
    }

    try {
        int d1 = std::stoi(date1.substr(0, 2));
        int m1 = std::stoi(date1.substr(3, 2));
        int y1 = std::stoi(date1.substr(6, 4));
        int d2 = std::stoi(date2.substr(0, 2));
        int m2 = std::stoi(date2.substr(3, 2));
        int y2 = std::stoi(date2.substr(6, 4));
        if (d1 < 1 || d1 > 31 || m1 < 1 || m1 > 12 ||
            d2 < 1 || d2 > 31 || m2 < 1 || m2 > 12) {
             return 0; 
        }
        int val1 = y1 * 10000 + m1 * 100 + d1;
        int val2 = y2 * 10000 + m2 * 100 + d2;
        if (val1 < val2) return -1;
        if (val1 > val2) return 1;
        return 0;
    } catch (const std::invalid_argument&) {
        return 0; 
    } catch (const std::out_of_range&) {
        return 0;
    }
}

int FlightManager::compareTimes(const std::string& time1, const std::string& time2) {
    // ... (logic không đổi) ...
    if (time1.length() != 5 || time2.length() != 5 ||
        time1[2] != ':' || time2[2] != ':') {
        return 0; 
    }

    try {
        int h1 = std::stoi(time1.substr(0, 2));
        int min1 = std::stoi(time1.substr(3, 2));
        int h2 = std::stoi(time2.substr(0, 2));
        int min2 = std::stoi(time2.substr(3, 2));
        if (h1 < 0 || h1 > 23 || min1 < 0 || min1 > 59 ||
            h2 < 0 || h2 > 23 || min2 < 0 || min2 > 59) {
            return 0; 
        }
        int val1 = h1 * 60 + min1;
        int val2 = h2 * 60 + min2;
        if (val1 < val2) return -1;
        if (val1 > val2) return 1;
        return 0;
    } catch (const std::invalid_argument&) {
        return 0; 
    } catch (const std::out_of_range&) {
        return 0;
    }
}

// Đổi tên hàm và kiểu trả về
std::vector<Flight*> FlightManager::getFlightsByRoute(
    const std::string& fromIATA,
    const std::string& toIATA) const 
{
    std::string routeKey = fromIATA + "_" + toIATA;
    RouteData** dataPtr = routeIndex_.find(routeKey);
    
    if (dataPtr && *dataPtr) {
        return (*dataPtr)->allFlights; // Đổi allInstances -> allFlights
    }
    
    return std::vector<Flight*>(); // Đổi
}

// Đổi tên hàm và kiểu tham số
std::vector<Flight*> FlightManager::filterByDate(
    const std::vector<Flight*>& flights, // Đổi
    const std::string& date) const 
{
    std::vector<Flight*> results; // Đổi
    
    for (Flight* flight : flights) { // Đổi
        if (flight && flight->getDepartureDate() == date) { // Đổi
            results.push_back(flight); // Đổi
        }
    }
    
    return results;
}

// Đổi tên hàm và kiểu tham số
std::vector<Flight*> FlightManager::filterByAirline(
    const std::vector<Flight*>& flights, // Đổi
    const std::string& airline) const 
{
    std::vector<Flight*> results; // Đổi
    
    for (Flight* flight : flights) { // Đổi
        if (!flight) continue;
        
        // Đổi logic: Lấy Route từ Flight để check airline
        Route* route = findRouteById(flight->getRouteId()); 
        if (route && flight->getAirline() == airline) { 
            results.push_back(flight); // Đổi
        }
    }
    
    return results;
}

// Đổi tên hàm và kiểu tham số
std::vector<Flight*> FlightManager::filterByPriceRange(
    const std::vector<Flight*>& flights, // Đổi
    int minPrice,
    int maxPrice) const 
{
    std::vector<Flight*> results; // Đổi
    
    for (Flight* flight : flights) { // Đổi
        if (!flight) continue;
        
        int price = flight->getFareEconomy(); // Đổi
        
        bool passesMin = (minPrice == 0 || price >= minPrice);
        bool passesMax = (maxPrice == 0 || price <= maxPrice);
        
        if (passesMin && passesMax) {
            results.push_back(flight); // Đổi
        }
    }
    
    return results;
}

// Đổi tên hàm và kiểu tham số
std::vector<Flight*> FlightManager::filterByPriceRangeAVL(
    const std::vector<Flight*>& flights, // Đổi
    int minPrice,
    int maxPrice) const 
{
    return filterByPriceRange(flights, minPrice, maxPrice);
}

// Đổi kiểu trả về
std::vector<Flight*> FlightManager::searchFlights(
    const SearchCriteria& criteria) const 
{
    // Step 1: Get by route
    std::vector<Flight*> results = // Đổi
        getFlightsByRoute(criteria.fromIATA, criteria.toIATA);
    
    if (results.empty()) return results;
    
    // Step 2: Filter by date
    if (!criteria.date.empty()) {
        results = filterByDate(results, criteria.date);
    }
    
    // Step 3: Filter by airline
    if (!criteria.airline.empty()) {
        results = filterByAirline(results, criteria.airline);
    }
    
    // Step 4: Filter by price
    if (criteria.minPrice > 0 || criteria.maxPrice > 0) {
        if (criteria.useAVLForPrice) {
            results = filterByPriceRangeAVL(results, criteria.minPrice, criteria.maxPrice);
        } else {
            results = filterByPriceRange(results, criteria.minPrice, criteria.maxPrice);
        }
    }
    
    return results;
}