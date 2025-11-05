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
}

FlightManager::~FlightManager() {
    // Save all data before destruction
    saveAllData();
    
    // Cleanup SeatManager
    delete seatManager_;
   
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
Flight* FlightManager::findFlightById(const std::string& flightId) {
    Flight** flightPtrPtr = flightIdTable.find(flightId);
    return (flightPtrPtr != nullptr) ? *flightPtrPtr : nullptr;
}

// <<< THAY ĐỔI: Dùng HashTable >>>
FlightInstance* FlightManager::findInstanceById(const std::string& instanceId) {
    FlightInstance** instancePtrPtr = instanceIdTable.find(instanceId);
    return (instancePtrPtr != nullptr) ? *instancePtrPtr : nullptr;
}

// (findInstancesByFlightId không đổi vì cần tìm nhiều, vẫn phải lặp)
DynamicArray<FlightInstance*> FlightManager::findInstancesByFlightId(const std::string& flightId) {
    DynamicArray<FlightInstance*> results;
    for (size_t i = 0; i < allInstances.size(); ++i) {
        if (allInstances[i]->getFlightId() == flightId) {
            results.push_back(allInstances[i]);
        }
    }
    return results;
}

DynamicArray<Flight*> FlightManager::findFlightByRoute(const std::string& fromIATA, const std::string& toIATA) {
    DynamicArray<Flight*> results;
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
const DynamicArray<Flight*>& FlightManager::getAllFlights() const{
    return this->allFlights;
}     

const DynamicArray<FlightInstance*>& FlightManager::getAllInstances() const{
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
            allFlights.erase(i);
            
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
            allInstances.erase(i);
            
            saveInstancesToFiles(instancesFilePath_);
            return true;
        }
    }
    return false;
}

SeatManager* FlightManager::getSeatManager() const {
    return seatManager_;
}
