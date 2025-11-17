#include "core/FlightManager.h"
#include "core/SeatManager.h"
#include <fstream>
#include <string>
#include <sstream>

// --- Constructor & Destructor ---

FlightManager::FlightManager(const std::string& routesFilePath, const std::string& flightsFilePath) 
    : routesFilePath_(routesFilePath), flightsFilePath_(flightsFilePath), seatManager_(nullptr) {
    
    seatManager_ = new SeatManager("data/seat_maps.txt", "data/seat_config.txt");
    
    this->loadRoutesFromFile(routesFilePath);
    this->loadFlightsFromFile(flightsFilePath);

    this->buildRouteIdTable();
    this->buildFlightIdTable();
    this->buildRouteIndex();
}

FlightManager::~FlightManager() {
    saveAllData();
    
    delete seatManager_;
    
    std::vector<RouteData*> deletedRoutes;
    for (Route* route : allRoutes) { 
        if (!route) continue;
        std::string routeKey = route->getDepartureAirport() + "_" + 
                               route->getArrivalAirport();
        RouteData** dataPtr = routeIndex_.find(routeKey);
        if (dataPtr && *dataPtr) {
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
   
    for (int i = 0; i < allRoutes.size(); i++) {
        delete allRoutes[i]; 
    }
    for (int i = 0; i < allFlights.size(); i++) {
        delete allFlights[i]; 
    }
}

// --- Hàm trợ giúp nội bộ ---

void FlightManager::loadRoutesFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                Route routeOnStack = Route::fromRecordLine(line);
                this->allRoutes.push_back(new Route(routeOnStack));
            }
        }
        file.close();
    }
}

void FlightManager::loadFlightsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                Flight flightOnStack = Flight::fromRecordLine(line);
                this->allFlights.push_back(new Flight(flightOnStack));
            }
        }
        file.close();
    }
}

void FlightManager::buildRouteIdTable() {
    for (int i = 0; i < allRoutes.size(); ++i) {
        if (allRoutes[i] != nullptr) {
            routeIdTable.insert(allRoutes[i]->getRouteId(), allRoutes[i]);
        }
    }
}

void FlightManager::buildFlightIdTable() {
    for (int i = 0; i < allFlights.size(); ++i) {
        if (allFlights[i] != nullptr) {
            flightIdTable.insert(allFlights[i]->getFlightId(), allFlights[i]);
        }
    }
}

// --- RouteIndex Management Helpers ---

bool FlightManager::addFlightToRouteIndex(Flight* flight) {
    if (!flight) return false;
    
    Route* route = findRouteById(flight->getRouteId());
    if (!route) return false;
    
    std::string routeKey = route->getDepartureAirport() + "_" + 
                           route->getArrivalAirport();
    
    RouteData** dataPtr = routeIndex_.find(routeKey);
    
    if (dataPtr && *dataPtr) {
        (*dataPtr)->addFlight(flight);
        sortFlightsByDateTime((*dataPtr)->allFlights);
    } else {
        RouteData* newData = new RouteData(routeKey);
        newData->addFlight(flight);
        routeIndex_.insert(routeKey, newData);
    }
    
    return true;
}

bool FlightManager::removeFlightFromRouteIndex(Flight* flight) {
    if (!flight) return false;
    
    Route* route = findRouteById(flight->getRouteId());
    if (!route) return false;
    
    std::string routeKey = route->getDepartureAirport() + "_" + 
                           route->getArrivalAirport();
    
    RouteData** dataPtr = routeIndex_.find(routeKey);
    if (!dataPtr || !(*dataPtr)) return false;
    
    std::vector<Flight*>& flights = (*dataPtr)->allFlights;
    for (size_t i = 0; i < flights.size(); ++i) {
        if (flights[i] == flight) {
            flights.erase(flights.begin() + i);
            return true;
        }
    }
    
    return false;
}

bool FlightManager::updateFlightInRouteIndex(Flight* flight, const std::string& oldRouteId) {
    if (!flight) return false;
    
    std::string newRouteId = flight->getRouteId();
    
    if (oldRouteId == newRouteId) {
        Route* route = findRouteById(newRouteId);
        if (route) {
            std::string routeKey = route->getDepartureAirport() + "_" + 
                                   route->getArrivalAirport();
            
            RouteData** dataPtr = routeIndex_.find(routeKey);
            if (dataPtr && *dataPtr) {
                sortFlightsByDateTime((*dataPtr)->allFlights);
                return true;
            }
        }
        return false;
    }
    
    Route* oldRoute = findRouteById(oldRouteId);
    if (oldRoute) {
        std::string oldRouteKey = oldRoute->getDepartureAirport() + "_" + 
                                  oldRoute->getArrivalAirport();
        
        RouteData** oldDataPtr = routeIndex_.find(oldRouteKey);
        if (oldDataPtr && *oldDataPtr) {
            std::vector<Flight*>& oldFlights = (*oldDataPtr)->allFlights;
            for (size_t i = 0; i < oldFlights.size(); ++i) {
                if (oldFlights[i] == flight) {
                    oldFlights.erase(oldFlights.begin() + i);
                    break;
                }
            }
        }
    }
    
    return addFlightToRouteIndex(flight);
}

void FlightManager::buildRouteIndex() {
    for (Flight* flight : allFlights) {
        if (!flight) continue;
        addFlightToRouteIndex(flight);
    }
}

// --- Chức năng Tạo mới (Create) ---

bool FlightManager::createNewRoute(const std::string& departureIATA,
                                   const std::string& arrivalIATA) {
    if (departureIATA.empty() || arrivalIATA.empty()) return false;
    
    std::string newId = departureIATA + "-" + arrivalIATA;
    if (findRouteById(newId) != nullptr) return false;

    Route* newRoute = new Route(departureIATA, arrivalIATA);
    this->allRoutes.push_back(newRoute);
    this->routeIdTable.insert(newRoute->getRouteId(), newRoute);
    return true;
}

bool FlightManager::createNewFlight(const std::string& routeId,
                                    const std::string& airline,
                                    const std::string& flightNumber,
                                    const std::string& departureDate,
                                    const std::string& departureTime,
                                    const std::string& arrivalDate,
                                    const std::string& arrivalTime,
                                    int totalCapacity,
                                    int fareEconomy,
                                    int fareBusiness) {
    if (findRouteById(routeId) == nullptr) return false;
    if (totalCapacity <= 0 || fareEconomy <= 0.0 || fareBusiness <= 0.0) return false;
    if (routeId.empty() || flightNumber.empty() || airline.empty() || 
        departureDate.empty() || departureTime.empty() || 
        arrivalDate.empty() || arrivalTime.empty()) return false;

    Flight* newFlight = new Flight(
        routeId, airline, flightNumber, departureDate, departureTime, 
        arrivalDate, arrivalTime, totalCapacity, fareEconomy, fareBusiness
    );
    
    this->allFlights.push_back(newFlight);
    this->flightIdTable.insert(newFlight->getFlightId(), newFlight);
    
    if (!addFlightToRouteIndex(newFlight)) {
        allFlights.pop_back();
        flightIdTable.remove(newFlight->getFlightId());
        delete newFlight;
        return false;
    }
    
    return true;
}

// --- Chức năng Đọc/Tìm kiếm (Read) ---

Route* FlightManager::findRouteById(const std::string& routeId) const {
    Route** routePtrPtr = routeIdTable.find(routeId);
    return (routePtrPtr != nullptr) ? *routePtrPtr : nullptr;
}

Flight* FlightManager::findFlightById(const std::string& flightId) const {
    Flight** flightPtrPtr = flightIdTable.find(flightId);
    return (flightPtrPtr != nullptr) ? *flightPtrPtr : nullptr;
}

std::vector<Flight*> FlightManager::findFlightsByRouteId(const std::string& routeId) const {
    std::vector<Flight*> results;
    for (size_t i = 0; i < allFlights.size(); ++i) {
        if (allFlights[i]->getRouteId() == routeId) {
            results.push_back(allFlights[i]);
        }
    }
    return results;
}

std::vector<Route*> FlightManager::findRouteByRoute(const std::string& fromIATA, const std::string& toIATA) const {
    std::vector<Route*> results;
    for (size_t i = 0; i < allRoutes.size(); ++i) {
        if (allRoutes[i]->getDepartureAirport() == fromIATA && 
            allRoutes[i]->getArrivalAirport() == toIATA) {
            results.push_back(allRoutes[i]);
        }
    }
    return results;
}

// --- Chức năng Lưu trữ (Persistence) ---

bool FlightManager::saveRoutesToFiles(const std::string& routesFilePath) const {
    std::ofstream routesFile(routesFilePath);
    if (!routesFile.is_open()) return false;
    for (size_t i = 0; i < allRoutes.size(); ++i) {
        routesFile << allRoutes[i]->toRecordLine() << "\n";
    }
    routesFile.close();
    return true;
}

bool FlightManager::saveFlightsToFiles(const std::string& flightsFilePath) const {
    std::ofstream flightsFile(flightsFilePath);
    if (!flightsFile.is_open()) return false;
    for (size_t i = 0; i < allFlights.size(); ++i) {
        flightsFile << allFlights[i]->toRecordLine() << "\n";
    }
    flightsFile.close();
    return true;
}

// --- Getters ---

const std::vector<Route*>& FlightManager::getAllRoutes() const {
    return this->allRoutes;
}     

const std::vector<Flight*>& FlightManager::getAllFlights() const {
    return this->allFlights;
}

// --- Save All Data ---

bool FlightManager::saveAllData() {
    bool routesSaved = saveRoutesToFiles(routesFilePath_);
    bool flightsSaved = saveFlightsToFiles(flightsFilePath_);
    return routesSaved && flightsSaved;
}

// --- Update and Delete Functions ---

bool FlightManager::updateRoute(const std::string& routeId,
                                const std::string& newDeparture, 
                                const std::string& newDestination) {
    Route* route = findRouteById(routeId);
    if (!route) return false;
    
    Route* newRoute = new Route(newDeparture, newDestination);
    
    for (int i = 0; i < allRoutes.size(); i++) {
        if (allRoutes[i]->getRouteId() == routeId) {
            routeIdTable.remove(routeId);
            delete allRoutes[i];
            allRoutes[i] = newRoute;
            routeIdTable.insert(newRoute->getRouteId(), newRoute);
            saveRoutesToFiles(routesFilePath_);
            return true;
        }
    }
    
    delete newRoute;
    return false;
}

bool FlightManager::deleteRoute(const std::string& routeId) {
    for (int i = 0; i < allRoutes.size(); i++) {
        if (allRoutes[i]->getRouteId() == routeId) {
            routeIdTable.remove(routeId);
            delete allRoutes[i];
            allRoutes.erase(allRoutes.begin() + i);
            saveRoutesToFiles(routesFilePath_);
            return true;
        }
    }
    return false;
}

bool FlightManager::updateFlight(const std::string& flightId, 
                                 const Flight& updatedFlight) {
    Flight* oldFlight = findFlightById(flightId);
    if (!oldFlight) return false;
    
    std::string oldRouteId = oldFlight->getRouteId();
    
    *oldFlight = updatedFlight;
    
    updateFlightInRouteIndex(oldFlight, oldRouteId);
    
    saveFlightsToFiles(flightsFilePath_);
    return true;
}

bool FlightManager::deleteFlight(const std::string& flightId) {
    for (int i = 0; i < allFlights.size(); i++) {
        if (allFlights[i]->getFlightId() == flightId) {
            Flight* flightToDelete = allFlights[i];
            
            removeFlightFromRouteIndex(flightToDelete);
            
            flightIdTable.remove(flightId);
            
            delete flightToDelete;
            allFlights.erase(allFlights.begin() + i);
            
            saveFlightsToFiles(flightsFilePath_);
            
            return true;
        }
    }
    return false;
}

SeatManager* FlightManager::getSeatManager() const {
    return seatManager_;
}

// --- Route Index and Search Implementation ---

void FlightManager::sortFlightsByDateTime(std::vector<Flight*>& flights) {
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

int FlightManager::compareDates(const std::string& date1, const std::string& date2) {
    if (date1.length() != 10 || date2.length() != 10 ||
        date1[2] != '/' || date1[5] != '/' ||
        date2[2] != '/' || date2[5] != '/') {
        return 0;
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

std::vector<Flight*> FlightManager::getFlightsByRoute(
    const std::string& fromIATA,
    const std::string& toIATA) const 
{
    std::string routeKey = fromIATA + "_" + toIATA;
    RouteData** dataPtr = routeIndex_.find(routeKey);
    
    if (dataPtr && *dataPtr) {
        return (*dataPtr)->allFlights;
    }
    
    return std::vector<Flight*>();
}

std::vector<Flight*> FlightManager::filterByDate(
    const std::vector<Flight*>& flights,
    const std::string& date) const 
{
    std::vector<Flight*> results;
    
    for (Flight* flight : flights) {
        if (flight && flight->getDepartureDate() == date) {
            results.push_back(flight);
        }
    }
    
    return results;
}

std::vector<Flight*> FlightManager::filterByAirline(
    const std::vector<Flight*>& flights,
    const std::string& airline) const 
{
    std::vector<Flight*> results;
    
    for (Flight* flight : flights) {
        if (!flight) continue;
        
        Route* route = findRouteById(flight->getRouteId()); 
        if (route && flight->getAirline() == airline) { 
            results.push_back(flight);
        }
    }
    
    return results;
}

std::vector<Flight*> FlightManager::filterByPriceRange(
    const std::vector<Flight*>& flights,
    int minPrice,
    int maxPrice) const 
{
    std::vector<Flight*> results;
    
    for (Flight* flight : flights) {
        if (!flight) continue;
        
        int price = flight->getFareEconomy();
        
        bool passesMin = (minPrice == 0 || price >= minPrice);
        bool passesMax = (maxPrice == 0 || price <= maxPrice);
        
        if (passesMin && passesMax) {
            results.push_back(flight);
        }
    }
    
    return results;
}

std::vector<Flight*> FlightManager::filterByPriceRangeAVL(
    const std::vector<Flight*>& flights,
    int minPrice,
    int maxPrice) const 
{
    return filterByPriceRange(flights, minPrice, maxPrice);
}

std::vector<Flight*> FlightManager::searchFlights(
    const SearchCriteria& criteria) const 
{
    std::vector<Flight*> results = 
        getFlightsByRoute(criteria.fromIATA, criteria.toIATA);
    
    if (results.empty()) return results;
    
    if (!criteria.date.empty()) {
        results = filterByDate(results, criteria.date);
    }
    
    if (!criteria.airline.empty()) {
        results = filterByAirline(results, criteria.airline);
    }
    
    if (criteria.minPrice > 0 || criteria.maxPrice > 0) {
        if (criteria.useAVLForPrice) {
            results = filterByPriceRangeAVL(results, criteria.minPrice, criteria.maxPrice);
        } else {
            results = filterByPriceRange(results, criteria.minPrice, criteria.maxPrice);
        }
    }
    
    return results;
}