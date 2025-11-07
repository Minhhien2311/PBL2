#include "core/FlightManager.h"
#include "core/SeatManager.h"
#include <fstream>
#include <string>
#include <sstream>

// --- Constructor & Destructor ---

FlightManager::FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath) 
    : flightsFilePath_(flightsFilePath), instancesFilePath_(instancesFilePath), seatManager_(nullptr) {
    
    // Initialize SeatManager at startup
    seatManager_ = new SeatManager("data/seat_maps.txt", "data/seat_config.txt");
    
    this->loadFlightsFromFile(flightsFilePath);
    this->loadInstancesFromFile(instancesFilePath);

    // Xây dựng bảng băm
    this->buildFlightIdTable();
    this->buildInstanceIdTable();
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
    for (Flight* flight : allFlights) {
        if (!flight) continue;
        std::string routeKey = flight->getDepartureAirport() + "_" + 
                              flight->getArrivalAirport();
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
   
    for (int i = 0; i < allFlights.size(); i++) {
        delete allFlights[i]; 
    }
    for (int i = 0; i < allInstances.size(); i++) {
        delete allInstances[i]; 
    }
}
// --- Hàm trợ giúp nội bộ ---

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

void FlightManager::loadInstancesFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // Sẽ tự động gọi fromRecordLine mới của FlightInstance
                FlightInstance instanceOnStack = FlightInstance::fromRecordLine(line);
                this->allInstances.push_back(new FlightInstance(instanceOnStack));
            }
        }
        file.close();
    }
}

// Xây dựng bảng băm
void FlightManager::buildFlightIdTable() {
    for (int i = 0; i < allFlights.size(); ++i) {
        if (allFlights[i] != nullptr) {
            flightIdTable.insert(allFlights[i]->getFlightId(), allFlights[i]);
        }
    }
}

void FlightManager::buildInstanceIdTable() {
    for (int i = 0; i < allInstances.size(); ++i) {
        if (allInstances[i] != nullptr) {
            instanceIdTable.insert(allInstances[i]->getInstanceId(), allInstances[i]);
        }
    }
}

// --- Chức năng Tạo mới (Create) ---

// <<< THAY ĐỔI: Cập nhật HashTable >>>
// Note: 'number' parameter kept for backward compatibility but not used
bool FlightManager::createNewFlight( const std::string& airline,
                                     const std::string& departureIATA,
                                     const std::string& arrivalIATA) {
    if (airline.empty() || departureIATA.empty() || arrivalIATA.empty()) return false;
    
    // Kiểm tra trùng lặp bằng ID mới (airline-departure-arrival)
    std::string newId = airline + "-" + departureIATA + "-" + arrivalIATA;
    if (findFlightById(newId) != nullptr) return false;

    Flight* newFlight = new Flight(airline, departureIATA, arrivalIATA);
    this->allFlights.push_back(newFlight);
    
    // <<< THÊM MỚI: Cập nhật bảng băm ID >>>
    this->flightIdTable.insert(newFlight->getFlightId(), newFlight);
    return true;
}

// <<< THAY ĐỔI: Cập nhật HashTable >>>
bool FlightManager::createNewInstance(const std::string& flightId,
                                      const std::string& flightNumber,
                                      const std::string& departureDate,
                                      const std::string& departureTime,
                                      const std::string& arrivalDate,
                                      const std::string& arrivalTime,
                                      int totalCapacity,
                                      int fareEconomy,
                                      int fareBusiness) {
    // Kiểm tra Flight gốc bằng HashTable
    if (findFlightById(flightId) == nullptr) return false; 
    if (totalCapacity <= 0 || fareEconomy <= 0.0 || fareBusiness <= 0.0) return false;
    if (flightId.empty() || flightNumber.empty() || departureDate.empty() || departureTime.empty() || arrivalDate.empty() || arrivalTime.empty()) return false;

    FlightInstance* newInstance = new FlightInstance(
        flightId, flightNumber, departureDate, departureTime, arrivalDate, arrivalTime, 
        totalCapacity, fareEconomy, fareBusiness
    );
    this->allInstances.push_back(newInstance);
    
    // <<< THÊM MỚI: Cập nhật bảng băm ID >>>
    this->instanceIdTable.insert(newInstance->getInstanceId(), newInstance);
    return true;
}

// --- Chức năng Đọc/Tìm kiếm (Read) ---

// <<< THAY ĐỔI: Dùng HashTable >>>
Flight* FlightManager::findFlightById(const std::string& flightId) const {
    Flight** flightPtrPtr = flightIdTable.find(flightId);
    return (flightPtrPtr != nullptr) ? *flightPtrPtr : nullptr;
}

// <<< THAY ĐỔI: Dùng HashTable >>>
FlightInstance* FlightManager::findInstanceById(const std::string& instanceId) const {
    FlightInstance** instancePtrPtr = instanceIdTable.find(instanceId);
    return (instancePtrPtr != nullptr) ? *instancePtrPtr : nullptr;
}

// (findInstancesByFlightId không đổi vì cần tìm nhiều, vẫn phải lặp)
std::vector<FlightInstance*> FlightManager::findInstancesByFlightId(const std::string& flightId) const {
    std::vector<FlightInstance*> results;
    for (size_t i = 0; i < allInstances.size(); ++i) {
        if (allInstances[i]->getFlightId() == flightId) {
            results.push_back(allInstances[i]);
        }
    }
    return results;
}

std::vector<Flight*> FlightManager::findFlightByRoute(const std::string& fromIATA, const std::string& toIATA) const {
    std::vector<Flight*> results;
    for (size_t i = 0; i < allFlights.size(); ++i) {
        if (allFlights[i]->getDepartureAirport() == fromIATA && allFlights[i]->getArrivalAirport() == toIATA) {
            results.push_back(allFlights[i]);
        }
    }
    return results;
}

// --- Chức năng Lưu trữ (Persistence) ---

bool FlightManager::saveFlightsToFiles(const std::string& flightsFilePath) const {
    std::ofstream flightsFile(flightsFilePath);
    if (!flightsFile.is_open()) return false;
    for (size_t i = 0; i < allFlights.size(); ++i) {
        flightsFile << allFlights[i]->toRecordLine() << "\n";
    }
    flightsFile.close();
    return true;
}

bool FlightManager::saveInstancesToFiles(const std::string& instancesFilePath) const {
    std::ofstream instancesFile(instancesFilePath);
    if (!instancesFile.is_open()) return false;
    for (size_t i = 0; i < allInstances.size(); ++i) {
        instancesFile << allInstances[i]->toRecordLine() << "\n";
    }
    instancesFile.close();
    return true;
}

// --- Getters ---
const std::vector<Flight*>& FlightManager::getAllFlights() const{
    return this->allFlights;
}     

const std::vector<FlightInstance*>& FlightManager::getAllInstances() const{
    return this->allInstances;
}

// --- Save All Data ---
bool FlightManager::saveAllData() {
    bool flightsSaved = saveFlightsToFiles(flightsFilePath_);
    bool instancesSaved = saveInstancesToFiles(instancesFilePath_);
    return flightsSaved && instancesSaved;
}

// --- Update and Delete Functions ---

bool FlightManager::updateFlight(const std::string& flightId, 
                                  const std::string& newAirline,
                                  const std::string& newDeparture, 
                                  const std::string& newDestination) {
    Flight* flight = findFlightById(flightId);
    if (!flight) return false;
    
    // Since Flight doesn't have setters, we create a new Flight and replace it
    Flight* newFlight = new Flight(newAirline, newDeparture, newDestination);
    
    // Find and replace in the array
    for (int i = 0; i < allFlights.size(); i++) {
        if (allFlights[i]->getFlightId() == flightId) {
            // Remove old from hash table
            flightIdTable.remove(flightId);
            
            // Delete old flight and replace with new
            delete allFlights[i];
            allFlights[i] = newFlight;
            
            // Add new to hash table
            flightIdTable.insert(newFlight->getFlightId(), newFlight);
            
            saveFlightsToFiles(flightsFilePath_);
            return true;
        }
    }
    
    delete newFlight; // Clean up if not used
    return false;
}

bool FlightManager::deleteFlight(const std::string& flightId) {
    for (int i = 0; i < allFlights.size(); i++) {
        if (allFlights[i]->getFlightId() == flightId) {
            // Remove from hash table
            flightIdTable.remove(flightId);
            
            // Delete and remove from array
            delete allFlights[i];
            // allFlights.erase(i);
            allFlights.erase(allFlights.begin() + i);
            
            saveFlightsToFiles(flightsFilePath_);
            return true;
        }
    }
    return false;
}

bool FlightManager::updateInstance(const std::string& instanceId, 
                                     const FlightInstance& updatedInstance) {
    FlightInstance* instance = findInstanceById(instanceId);
    if (!instance) return false;
    
    // Update the instance by copying data
    *instance = updatedInstance;
    
    saveInstancesToFiles(instancesFilePath_);
    return true;
}

bool FlightManager::deleteInstance(const std::string& instanceId) {
    for (int i = 0; i < allInstances.size(); i++) {
        if (allInstances[i]->getInstanceId() == instanceId) {
            // Remove from hash table
            instanceIdTable.remove(instanceId);
            
            // Delete and remove from array
            delete allInstances[i];
<<<<<<< HEAD
=======
            // allInstances.erase(i);
>>>>>>> b3de18c70a83979139713280049fc072ed00d0d7
            allInstances.erase(allInstances.begin() + i);
            
            saveInstancesToFiles(instancesFilePath_);
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
    for (FlightInstance* instance : allInstances) {
        if (!instance) continue;
        
        Flight* flight = findFlightById(instance->getFlightId());
        if (!flight) continue;
        
        std::string routeKey = flight->getDepartureAirport() + "_" + 
                              flight->getArrivalAirport();
        
        RouteData** dataPtr = routeIndex_.find(routeKey);
        
        if (dataPtr == nullptr) {
            RouteData* newData = new RouteData(routeKey);
            newData->addInstance(instance);
            routeIndex_.insert(routeKey, newData);
        } else {
            (*dataPtr)->addInstance(instance);
        }
    }
    
    // Sort each route by datetime - iterate through unique routes
    // Track which routes we've already sorted to avoid duplicates
    std::vector<std::string> sortedRoutes;
    for (Flight* flight : allFlights) {
        if (!flight) continue;
        std::string routeKey = flight->getDepartureAirport() + "_" + 
                              flight->getArrivalAirport();
        
        // Check if we've already sorted this route
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
                sortInstancesByDateTime((*dataPtr)->allInstances);
                sortedRoutes.push_back(routeKey);
            }
        }
    }
}

void FlightManager::sortInstancesByDateTime(std::vector<FlightInstance*>& instances) {
    // Bubble sort with custom comparator
    for (size_t i = 0; i < instances.size(); i++) {
        for (size_t j = i + 1; j < instances.size(); j++) {
            if (!instances[i] || !instances[j]) continue;
            
            int dateCmp = compareDates(instances[i]->getDepartureDate(),
                                      instances[j]->getDepartureDate());
            if (dateCmp > 0) {
                std::swap(instances[i], instances[j]);
            } else if (dateCmp == 0) {
                int timeCmp = compareTimes(instances[i]->getDepartureTime(),
                                          instances[j]->getDepartureTime());
                if (timeCmp > 0) {
                    std::swap(instances[i], instances[j]);
                }
            }
        }
    }
}

int FlightManager::compareDates(const std::string& date1, const std::string& date2) {
    // Validate format: DD/MM/YYYY (10 characters with slashes at positions 2 and 5)
    if (date1.length() != 10 || date2.length() != 10 ||
        date1[2] != '/' || date1[5] != '/' ||
        date2[2] != '/' || date2[5] != '/') {
        return 0; // Invalid format, treat as equal
    }
    
    try {
        // Parse DD/MM/YYYY
        int d1 = std::stoi(date1.substr(0, 2));
        int m1 = std::stoi(date1.substr(3, 2));
        int y1 = std::stoi(date1.substr(6, 4));
        
        int d2 = std::stoi(date2.substr(0, 2));
        int m2 = std::stoi(date2.substr(3, 2));
        int y2 = std::stoi(date2.substr(6, 4));
        
        // Validate ranges
        if (d1 < 1 || d1 > 31 || m1 < 1 || m1 > 12 ||
            d2 < 1 || d2 > 31 || m2 < 1 || m2 > 12) {
            return 0; // Invalid date, treat as equal
        }
        
        int val1 = y1 * 10000 + m1 * 100 + d1;
        int val2 = y2 * 10000 + m2 * 100 + d2;
        
        if (val1 < val2) return -1;
        if (val1 > val2) return 1;
        return 0;
    } catch (const std::invalid_argument&) {
        return 0; // Parse error, treat as equal
    } catch (const std::out_of_range&) {
        return 0; // Parse error, treat as equal
    }
}

int FlightManager::compareTimes(const std::string& time1, const std::string& time2) {
    // Validate format: HH:MM (5 characters with colon at position 2)
    if (time1.length() != 5 || time2.length() != 5 ||
        time1[2] != ':' || time2[2] != ':') {
        return 0; // Invalid format, treat as equal
    }
    
    try {
        // Parse HH:MM
        int h1 = std::stoi(time1.substr(0, 2));
        int min1 = std::stoi(time1.substr(3, 2));
        
        int h2 = std::stoi(time2.substr(0, 2));
        int min2 = std::stoi(time2.substr(3, 2));
        
        // Validate ranges
        if (h1 < 0 || h1 > 23 || min1 < 0 || min1 > 59 ||
            h2 < 0 || h2 > 23 || min2 < 0 || min2 > 59) {
            return 0; // Invalid time, treat as equal
        }
        
        int val1 = h1 * 60 + min1;
        int val2 = h2 * 60 + min2;
        
        if (val1 < val2) return -1;
        if (val1 > val2) return 1;
        return 0;
    } catch (const std::invalid_argument&) {
        return 0; // Parse error, treat as equal
    } catch (const std::out_of_range&) {
        return 0; // Parse error, treat as equal
    }
}

std::vector<FlightInstance*> FlightManager::getFlightsByRoute(
    const std::string& fromIATA,
    const std::string& toIATA) const 
{
    std::string routeKey = fromIATA + "_" + toIATA;
    RouteData** dataPtr = routeIndex_.find(routeKey);
    
    if (dataPtr && *dataPtr) {
        return (*dataPtr)->allInstances;
    }
    
    return std::vector<FlightInstance*>();
}

std::vector<FlightInstance*> FlightManager::filterByDate(
    const std::vector<FlightInstance*>& flights,
    const std::string& date) const 
{
    std::vector<FlightInstance*> results;
    
    for (FlightInstance* instance : flights) {
        if (instance && instance->getDepartureDate() == date) {
            results.push_back(instance);
        }
    }
    
    return results;
}

std::vector<FlightInstance*> FlightManager::filterByAirline(
    const std::vector<FlightInstance*>& flights,
    const std::string& airline) const 
{
    std::vector<FlightInstance*> results;
    
    for (FlightInstance* instance : flights) {
        if (!instance) continue;
        
        Flight* flight = findFlightById(instance->getFlightId());
        if (flight && flight->getAirline() == airline) {
            results.push_back(instance);
        }
    }
    
    return results;
}

std::vector<FlightInstance*> FlightManager::filterByPriceRange(
    const std::vector<FlightInstance*>& flights,
    int minPrice,
    int maxPrice) const 
{
    std::vector<FlightInstance*> results;
    
    for (FlightInstance* instance : flights) {
        if (!instance) continue;
        
        int price = instance->getFareEconomy();
        
        bool passesMin = (minPrice == 0 || price >= minPrice);
        bool passesMax = (maxPrice == 0 || price <= maxPrice);
        
        if (passesMin && passesMax) {
            results.push_back(instance);
        }
    }
    
    return results;
}

std::vector<FlightInstance*> FlightManager::filterByPriceRangeAVL(
    const std::vector<FlightInstance*>& flights,
    int minPrice,
    int maxPrice) const 
{
    // For now, use linear search as AVL tree is not implemented yet
    // This can be enhanced later with AVL tree from PR #16
    return filterByPriceRange(flights, minPrice, maxPrice);
}

std::vector<FlightInstance*> FlightManager::searchFlights(
    const SearchCriteria& criteria) const 
{
    // Step 1: Get by route
    std::vector<FlightInstance*> results = 
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
