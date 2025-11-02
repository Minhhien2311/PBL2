#include "core/SeatManager.h"
#include <fstream>
#include <sstream>

SeatManager::SeatManager(HashTable<std::string, std::string>& cache) 
    : seatDataCache(cache), isDirty(false), totalRows(0), totalCols(0), businessClassEndRow(0) {
    // Load configuration from file
    loadConfiguration(std::string(DATA_DIR) + "/seat_config.txt");
}

SeatManager::~SeatManager() {
    // Clean up allocated seat map
    unload();
}

void SeatManager::loadConfiguration(const std::string& configFilePath) {
    std::ifstream file(configFilePath);
    if (!file.is_open()) {
        // Set default values if config file doesn't exist
        totalRows = 30;
        totalCols = 6;
        businessClassEndRow = 5;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "TOTAL_ROWS") {
            totalRows = std::stoi(value);
        } else if (key == "TOTAL_COLS") {
            totalCols = std::stoi(value);
        } else if (key == "BUSINESS_CLASS_END_ROW") {
            businessClassEndRow = std::stoi(value);
        }
    }
    file.close();
}

void SeatManager::createEmptySeatMap() {
    // Clear existing map - properly delete all allocated rows
    for (int i = 0; i < activeSeatMap.size(); ++i) {
        delete activeSeatMap[i];
    }
    activeSeatMap.clear(); // Use clear instead of assignment

    // Create seat map based on configuration
    for (int row = 0; row < totalRows; ++row) {
        DynamicArray<Seat>* rowSeats = new DynamicArray<Seat>();
        
        // Determine seat type based on row
        SeatType type = (row < businessClassEndRow) ? SeatType::BUSINESS : SeatType::ECONOMY;

        for (int col = 0; col < totalCols; ++col) {
            std::string seatId = Seat::coordinatesToId(row, col);
            rowSeats->push_back(Seat(seatId, type));
        }
        
        activeSeatMap.push_back(rowSeats);
    }
}

void SeatManager::loadSeatMapFor(const std::string& instanceId) {
    activeFlightInstanceId = instanceId;
    isDirty = false;

    // Create empty seat map first
    createEmptySeatMap();

    // Look up booked seats from cache
    std::string* bookedSeatsStr = seatDataCache.find(instanceId);
    if (bookedSeatsStr == nullptr || bookedSeatsStr->empty()) {
        // No booked seats for this instance
        return;
    }

    // Parse booked seats (format: "A1,B5,C3")
    std::stringstream ss(*bookedSeatsStr);
    std::string seatId;
    
    while (std::getline(ss, seatId, ',')) {
        if (seatId.empty()) continue;

        auto coords = Seat::idToCoordinates(seatId);
        int row = coords.first;
        int col = coords.second;

        // Validate coordinates
        if (row >= 0 && row < totalRows && col >= 0 && col < totalCols) {
            (*activeSeatMap[row])[col].book();
        }
    }
}

bool SeatManager::bookSeat(const std::string& seatId) {
    auto coords = Seat::idToCoordinates(seatId);
    int row = coords.first;
    int col = coords.second;

    // Validate coordinates
    if (row < 0 || row >= totalRows || col < 0 || col >= totalCols) {
        return false;
    }

    // Check if seat is available
    if (!(*activeSeatMap[row])[col].isAvailable()) {
        return false;
    }

    // Book the seat
    (*activeSeatMap[row])[col].book();
    isDirty = true;
    return true;
}

bool SeatManager::releaseSeat(const std::string& seatId) {
    auto coords = Seat::idToCoordinates(seatId);
    int row = coords.first;
    int col = coords.second;

    // Validate coordinates
    if (row < 0 || row >= totalRows || col < 0 || col >= totalCols) {
        return false;
    }

    // Release the seat
    (*activeSeatMap[row])[col].release();
    isDirty = true;
    return true;
}

void SeatManager::saveChanges() {
    if (!isDirty || activeFlightInstanceId.empty()) {
        return;
    }

    // Collect all booked seats
    std::string bookedSeats;
    for (int row = 0; row < totalRows; ++row) {
        for (int col = 0; col < totalCols; ++col) {
            if (!(*activeSeatMap[row])[col].isAvailable()) {
                if (!bookedSeats.empty()) {
                    bookedSeats += ",";
                }
                bookedSeats += (*activeSeatMap[row])[col].getId();
            }
        }
    }

    // Update cache
    seatDataCache.insert(activeFlightInstanceId, bookedSeats);

    // Write entire cache to file
    std::ofstream file(std::string(DATA_DIR) + "/seat_maps.txt");
    if (file.is_open()) {
        // Iterate through all entries in the cache using forEach
        seatDataCache.forEach([&file](const std::string& key, const std::string& value) {
            if (!value.empty()) {
                file << key << "," << value << "\n";
            }
        });
        file.close();
    }

    isDirty = false;
}

void SeatManager::unload() {
    // Delete all allocated rows
    for (int i = 0; i < activeSeatMap.size(); ++i) {
        delete activeSeatMap[i];
    }
    activeSeatMap.clear(); // Use clear instead of assignment
    activeFlightInstanceId = "";
    isDirty = false;
}

const DynamicArray<DynamicArray<Seat>*>& SeatManager::getActiveSeatMap() const {
    return activeSeatMap;
}

std::string SeatManager::getActiveFlightInstanceId() const {
    return activeFlightInstanceId;
}

int SeatManager::getTotalRows() const {
    return totalRows;
}

int SeatManager::getTotalCols() const {
    return totalCols;
}

int SeatManager::getBusinessClassEndRow() const {
    return businessClassEndRow;
}
