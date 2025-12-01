#include "core/FlightManager.h"
#include "core/SeatManager.h"
#include <fstream>
#include <string>
#include <climits>
#include <sstream>
#include "utils/DateTime.h"
#include <algorithm>
#include <iterator>
#include <ctime>
#include <set>

// --- Constructor & Destructor ---

FlightManager::FlightManager(const std::string& routesFilePath, const std::string& flightsFilePath) 
    : routesFilePath_(routesFilePath), flightsFilePath_(flightsFilePath), seatManager_(nullptr) {
    
    seatManager_ = new SeatManager("C:/PBL2/data/seat_maps.txt", "C:/PBL2/data/seat_config.txt");
    
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
                // Create heap object and store pointer
                Flight* p = new Flight(flightOnStack);
                this->allFlights.push_back(p);
                // Maintain id table
                this->flightIdTable.insert(p->getFlightId(), p);
                // Index this flight by time (AVL)
                addFlightToTimeIndex(p);
                addFlightToPriceIndex(p);
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
                                    int availableSeats,
                                    int fareEconomy,
                                    int fareBusiness) {
    if (findRouteById(routeId) == nullptr) return false;
    if (totalCapacity <= 0 || fareEconomy <= 0.0 || fareBusiness <= 0.0) return false;
    if (routeId.empty() || flightNumber.empty() || airline.empty() || 
        departureDate.empty() || departureTime.empty() || 
        arrivalDate.empty() || arrivalTime.empty()) return false;

    Flight* newFlight = new Flight(
        routeId, airline, flightNumber, departureDate, departureTime, 
        arrivalDate, arrivalTime,  totalCapacity, availableSeats, fareEconomy, fareBusiness
    );
    
    this->allFlights.push_back(newFlight);
    this->flightIdTable.insert(newFlight->getFlightId(), newFlight);
    
    if (!addFlightToRouteIndex(newFlight)) {
        allFlights.pop_back();
        flightIdTable.remove(newFlight->getFlightId());
        delete newFlight;
        return false;
    }

    // Add to time index (AVL)
    addFlightToTimeIndex(newFlight);
    addFlightToPriceIndex(newFlight);
    
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
// FlightManager.cpp

bool FlightManager::updateRoute(const std::string& routeId,
                                const std::string& newDeparture, 
                                const std::string& newDestination) {
    // === STEP 1: Validate input ===
    if (newDeparture.empty() || newDestination.empty()) {
        return false;
    }
    
    // === STEP 2: Tìm route hiện tại ===
    Route* oldRoute = findRouteById(routeId);
    if (!oldRoute) return false;
    
    // === STEP 3: Tính ID mới ===
    std::string newRouteId = newDeparture + "-" + newDestination;
    
    // === STEP 4: Kiểm tra trùng lặp ===
    
    // Case 1: Nếu KHÔNG THAY ĐỔI gì → Chấp nhận (không cần update)
    if (newRouteId == routeId) {
        return true;  // ← Thành công nhưng không làm gì
    }
    
    // Case 2: Nếu THAY ĐỔI → Kiểm tra ID mới có trùng không
    Route* existingRoute = findRouteById(newRouteId);
    if (existingRoute != nullptr) {
        // ID mới đã tồn tại ở route KHÁC
        return false;
    }
    
    // === STEP 5: Thực hiện update ===
    Route* newRoute = new Route(newDeparture, newDestination);
    
    for (int i = 0; i < allRoutes.size(); i++) {
        if (allRoutes[i]->getRouteId() == routeId) {
            // Xóa ID cũ khỏi hash table
            routeIdTable.remove(routeId);
            
            // Thay thế route
            delete allRoutes[i];
            allRoutes[i] = newRoute;
            
            // Thêm ID mới vào hash table
            routeIdTable.insert(newRoute->getRouteId(), newRoute);
            
            // ✅ KHÔNG lưu file ngay (theo chiến lược lazy write)
            
            return true;
        }
    }
    
    // Cleanup nếu không tìm thấy route
    delete newRoute;
    return false;
}

// Tương tự cho deleteRoute(), updateFlight(), deleteFlight()
bool FlightManager::deleteRoute(const std::string& routeId) {
    for (int i = 0; i < allRoutes.size(); i++) {
        if (allRoutes[i]->getRouteId() == routeId) {
            routeIdTable.remove(routeId);
            delete allRoutes[i];
            allRoutes.erase(allRoutes.begin() + i);
            
            // ✅ BỎ: saveRoutesToFiles(routesFilePath_);
            
            return true;
        }
    }
    return false;
}

bool FlightManager::updateFlight(const std::string& flightId, 
                                 const Flight& updatedFlight) {
    Flight* oldFlight = findFlightById(flightId);
    if (!oldFlight) return false;
    
    // Save old time key BEFORE changing object
    time_t oldKey = getFlightTimeKey(*oldFlight);
    int oldPriceKey = oldFlight->getFareEconomy();
    
    std::string oldRouteId = oldFlight->getRouteId();
    *oldFlight = updatedFlight;
    updateFlightInRouteIndex(oldFlight, oldRouteId);

    // Move id in time index if departure time changed
    moveFlightTimeIndex(oldFlight, oldKey);
    moveFlightPriceIndex(oldFlight, oldPriceKey);
    
    return true;
}

bool FlightManager::deleteFlight(const std::string& flightId) {
    for (int i = 0; i < allFlights.size(); i++) {
        if (allFlights[i]->getFlightId() == flightId) {
            Flight* flightToDelete = allFlights[i];
            
            // Remove from time index first
            removeFlightFromTimeIndex(flightToDelete);
            removeFlightFromPriceIndex(flightToDelete);

            removeFlightFromRouteIndex(flightToDelete);
            flightIdTable.remove(flightId);
            delete flightToDelete;
            allFlights.erase(allFlights.begin() + i);
            
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

std::vector<Flight*> FlightManager::filterByPriceRangeAVL(
    const std::vector<Flight*>& flights,
    int minPrice,
    int maxPrice) const 
{
    // 1. Xử lý trường hợp biên
    if (flights.empty()) return {};
    if (minPrice <= 0 && maxPrice <= 0) return flights; // Không lọc nếu không có khoảng giá

    int effectiveMin = (minPrice > 0) ? minPrice : 0;
    int effectiveMax = (maxPrice > 0) ? maxPrice : INT_MAX;

    // 2. Truy vấn khoảng trên cây AVL (O(log N + K))
    // Kết quả là danh sách các danh sách ID: [[id1, id2], [id3], ...]
    std::vector<std::vector<std::string>> avlResults = 
        flightPriceTree.rangeQuery(effectiveMin, effectiveMax);

    // 3. Đưa tất cả ID tìm được vào một std::set để tra cứu cực nhanh (O(1) trung bình)
    std::set<std::string> validPriceFlightIds;
    for (const auto& idList : avlResults) {
        for (const std::string& id : idList) {
            validPriceFlightIds.insert(id);
        }
    }

    // 4. Thực hiện phép GIAO: Duyệt danh sách đầu vào và chỉ giữ lại những cái có trong set
    std::vector<Flight*> finalResults;
    for (Flight* flight : flights) {
        if (flight && validPriceFlightIds.count(flight->getFlightId())) {
            finalResults.push_back(flight);
        }
    }
    
    return finalResults;
}

std::vector<Flight*> FlightManager::searchFlights( const SearchCriteria& criteria) const 
{
    std::vector<Flight*> results = getAllFlights();
    
    if (results.empty()) return results;

    if (!criteria.fromIATA.empty() && !criteria.toIATA.empty()) {
        std::vector<Flight*> routeFiltered;
        for (Flight* flight : results) {
            Route* route = findRouteById(flight->getRouteId());
            if (route && route->getDepartureAirport() == criteria.fromIATA &&
                route->getArrivalAirport() == criteria.toIATA) {
                routeFiltered.push_back(flight);
            }
        }
        results = routeFiltered;
    }
    
    if (!criteria.date.empty()) {
        results = filterByDate(results, criteria.date);
    }
    
    if (!criteria.airline.empty()) {
        results = filterByAirline(results, criteria.airline);
    }
    
    if (criteria.minPrice > 0 || criteria.maxPrice > 0) {
        if (criteria.useAVLForPrice) {
            results = filterByPriceRangeAVL(results, criteria.minPrice, criteria.maxPrice);
        }
    }
    
    return results;
}

// --- Time index helpers (AVL) ---

time_t FlightManager::getFlightTimeKey(const Flight& flight) const {
    // Use DateTime helper to parse dd/MM/yyyy and HH:MM
    auto tp = utils::DateTime::fromDmYHm(flight.getDepartureDate(), flight.getDepartureTime());
    return utils::DateTime::toUnix(tp);
}

void FlightManager::addFlightToTimeIndex(Flight* flight) {
    if (!flight) return;
    time_t key = getFlightTimeKey(*flight);
    std::vector<std::string>* existing = flightTimeTree.find(key);
    if (existing) {
        // ensure not duplicate
        if (std::find(existing->begin(), existing->end(), flight->getFlightId()) == existing->end()) {
            existing->push_back(flight->getFlightId());
        }
    } else {
        std::vector<std::string> list;
        list.push_back(flight->getFlightId());
        flightTimeTree.insert(key, list); // AVL will copy the vector
    }
}

void FlightManager::removeFlightFromTimeIndexByKey(time_t key, const std::string& flightId) {
    std::vector<std::string>* existing = flightTimeTree.find(key);
    if (!existing) return;
    auto it = std::find(existing->begin(), existing->end(), flightId);
    if (it != existing->end()) {
        existing->erase(it);
        if (existing->empty()) {
            flightTimeTree.remove(key);
        }
    }
}

void FlightManager::removeFlightFromTimeIndex(Flight* flight) {
    if (!flight) return;
    time_t key = getFlightTimeKey(*flight);
    removeFlightFromTimeIndexByKey(key, flight->getFlightId());
}

void FlightManager::moveFlightTimeIndex(Flight* flight, time_t oldKey) {
    if (!flight) return;
    time_t newKey = getFlightTimeKey(*flight);
    if (oldKey == newKey) return; // nothing changed
    // Remove from old key
    removeFlightFromTimeIndexByKey(oldKey, flight->getFlightId());
    // Add to new key
    addFlightToTimeIndex(flight);
}

// Existing function (kept for compatibility) — now delegates to addFlightToTimeIndex
void FlightManager::indexFlightTime(const Flight& flight) {
    // Find the Flight* in flightIdTable if it exists
    Flight** pptr = flightIdTable.find(flight.getFlightId());
    if (pptr && *pptr) {
        addFlightToTimeIndex(*pptr);
    } else {
        // fallback: create a temporary vector under the key (rare)
        time_t key = getFlightTimeKey(flight);
        std::vector<std::string>* existing = flightTimeTree.find(key);
        if (existing) {
            if (std::find(existing->begin(), existing->end(), flight.getFlightId()) == existing->end()) {
                existing->push_back(flight.getFlightId());
            }
        } else {
            std::vector<std::string> list;
            list.push_back(flight.getFlightId());
            flightTimeTree.insert(key, list);
        }
    }
}

std::vector<Flight*> FlightManager::getFutureFlights(bool onlyFuture) {
    std::vector<std::string> flightIds;
    
    if (onlyFuture) {
        // === LOGIC CHO AGENT (FILTER MODE) ===
        time_t now = utils::DateTime::toUnix(utils::DateTime::nowUtc());
        time_t farFuture = std::numeric_limits<time_t>::max();
        
        // Dùng AVL Range Query: O(log N + K) - Cực nhanh!
        std::vector<std::vector<std::string>> nestedResult = 
            flightTimeTree.rangeQuery(now, farFuture);
        
        // Flatten kết quả
        for (const auto& listIds : nestedResult) {
            flightIds.insert(flightIds.end(), listIds.begin(), listIds.end());
        }
    } else {
        // === LOGIC CHO ADMIN (SHOW ALL MODE) ===
        std::vector<std::vector<std::string>> nestedResult = 
            flightTimeTree.getAllValues();
        
        // Flatten kết quả
        for (const auto& listIds : nestedResult) {
            flightIds.insert(flightIds.end(), listIds.begin(), listIds.end());
        }
    }
    
    // Chuyển từ ID sang Flight*
    std::vector<Flight*> result;
    for (const std::string& id : flightIds) {
        Flight* flight = findFlightById(id);
        if (flight) {
            result.push_back(flight);
        }
    }
    
    return result;
}

// --- Price index helpers (AVL) ---

// Hàm thêm chuyến bay vào index giá
void FlightManager::addFlightToPriceIndex(Flight* flight) {
    if (!flight) return;
    // Chúng ta dùng Giá Economy làm key để index
    int key = flight->getFareEconomy();
    
    std::vector<std::string>* existing = flightPriceTree.find(key);
    if (existing) {
        // Nếu giá này đã có, thêm ID vào danh sách (tránh trùng lặp)
        if (std::find(existing->begin(), existing->end(), flight->getFlightId()) == existing->end()) {
            existing->push_back(flight->getFlightId());
        }
    } else {
        // Nếu giá này chưa có, tạo danh sách mới
        std::vector<std::string> list;
        list.push_back(flight->getFlightId());
        flightPriceTree.insert(key, list);
    }
}

// Hàm xóa chuyến bay khỏi index giá
void FlightManager::removeFlightFromPriceIndex(Flight* flight) {
    if (!flight) return;
    int key = flight->getFareEconomy();
    std::string flightId = flight->getFlightId();

    std::vector<std::string>* existing = flightPriceTree.find(key);
    if (!existing) return;

    auto it = std::find(existing->begin(), existing->end(), flightId);
    if (it != existing->end()) {
        existing->erase(it);
        // Nếu danh sách rỗng sau khi xóa, xóa luôn node khỏi cây AVL
        if (existing->empty()) {
            flightPriceTree.remove(key);
        }
    }
}

// Hàm cập nhật index khi giá thay đổi
void FlightManager::moveFlightPriceIndex(Flight* flight, int oldPrice) {
    if (!flight) return;
    int newPrice = flight->getFareEconomy();
    if (oldPrice == newPrice) return; // Giá không đổi, không làm gì

    // Xóa khỏi key giá cũ
    // Lưu ý: ta phải tự tìm ID và xóa thủ công vì object flight lúc này đã mang giá mới
    std::vector<std::string>* oldList = flightPriceTree.find(oldPrice);
    if (oldList) {
        auto it = std::find(oldList->begin(), oldList->end(), flight->getFlightId());
        if (it != oldList->end()) {
            oldList->erase(it);
            if (oldList->empty()) flightPriceTree.remove(oldPrice);
        }
    }

    // Thêm vào key giá mới
    addFlightToPriceIndex(flight);
}