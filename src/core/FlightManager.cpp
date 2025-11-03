#include "core/FlightManager.h" 
#include "core/SeatManager.h"
#include <fstream>
#include <string>
#include <sstream>

// --- Constructor & Destructor ---

FlightManager::FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath) {
    this->loadFlightsFromFile(flightsFilePath);
    this->loadInstancesFromFile(instancesFilePath);

    // Xây dựng bảng băm
    this->buildFlightIdTable();
    this->buildInstanceIdTable();

    // Khởi tạo cache ghế và SeatManager
    seatDataCache = new HashTable<std::string, std::string>();
    
    // Load dữ liệu ghế từ file (nếu có)
    std::ifstream seatFile("data/seat_maps.txt");
    if (seatFile.is_open()) {
        std::string line;
        while (std::getline(seatFile, line)) {
            if (line.empty()) continue;
            
            // Format: instanceId|seatData
            size_t pos = line.find('|');
            if (pos != std::string::npos) {
                std::string instanceId = line.substr(0, pos);
                std::string seatData = line.substr(pos + 1);
                seatDataCache->insert(instanceId, seatData);
            }
        }
        seatFile.close();
    }
    
    // Tạo SeatManager
    seatManager = new SeatManager(*seatDataCache);
}

FlightManager::~FlightManager() {
    // Save all data before destruction
    saveAllData();
    
    if (seatManager) {
        delete seatManager;
    }
    
    if (seatDataCache) {
        delete seatDataCache;
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
bool FlightManager::createNewFlight(const std::string& number,
                                     const std::string& airline,
                                     const std::string& departureIATA,
                                     const std::string& arrivalIATA) {
    (void)number; // Suppress unused parameter warning
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
                                      int totalEconomySeats,
                                      int totalBusinessSeats,
                                      int fareEconomy,
                                      int fareBusiness) {
    // Kiểm tra Flight gốc bằng HashTable
    if (findFlightById(flightId) == nullptr) return false; 
    if (totalEconomySeats <= 0 || totalBusinessSeats <= 0 || fareEconomy <= 0.0 || fareBusiness <= 0.0) return false;
    if (flightId.empty() || flightNumber.empty() || departureDate.empty() || departureTime.empty() || arrivalDate.empty() || arrivalTime.empty()) return false;

    FlightInstance* newInstance = new FlightInstance(
        flightId, flightNumber, departureDate, departureTime, arrivalDate, arrivalTime, 
        totalEconomySeats, totalBusinessSeats, fareEconomy, fareBusiness
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

SeatManager* FlightManager::getSeatManager() {
    return seatManager;
}

bool FlightManager::saveAllData() {
    bool success = true;
    
    // Save seat changes
    if (seatManager) {
        seatManager->saveChanges();
        
        // Save seat cache to file
        std::ofstream seatFile("data/seat_maps.txt");
        if (seatFile.is_open()) {
            // Save each instance in cache
            for (int i = 0; i < allInstances.size(); ++i) {
                if (allInstances[i]) {
                    std::string instanceId = allInstances[i]->getInstanceId();
                    std::string* seatData = seatDataCache->find(instanceId);
                    if (seatData && !seatData->empty()) {
                        seatFile << instanceId << "|" << *seatData << "\n";
                    }
                }
            }
            seatFile.close();
        } else {
            success = false;
        }
    }
    
    // Save flights and instances using existing methods
    success = success && saveFlightsToFiles("data/flights.txt");
    success = success && saveInstancesToFiles("data/flight_instances.txt");
    
    return success;
}