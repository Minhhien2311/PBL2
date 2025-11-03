#include "core/SeatManager.h"
#include "entities/FlightInstance.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

SeatManager::SeatManager(const std::string& seatStatusPath,
                         const std::string& seatConfigPath)
    : seatStatusFilePath_(seatStatusPath),
      seatConfigFilePath_(seatConfigPath),
      currentInstanceId_(""),
      selectedSeat_(nullptr),
      seatRows_(0),
      seatCols_(6)
{
    loadConfiguration();
}

SeatManager::~SeatManager() {
    clearCurrentMap();
}

void SeatManager::loadConfiguration() {
    std::ifstream file(seatConfigFilePath_);
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open " << seatConfigFilePath_ << ". Using defaults.\n";
        seatCols_ = 6;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("SEAT_LAYOUT_COLS=") == 0) {
            seatCols_ = std::stoi(line.substr(17));
        }
    }
    file.close();
}

bool SeatManager::loadForFlight(const std::string& instanceId) {
    clearCurrentMap();
    currentInstanceId_ = instanceId;
    
    // Will be called by loadSeatMapFor(FlightInstance*)
    return true;
}

bool SeatManager::loadSeatMapFor(FlightInstance* instance) {
    if (!instance) return false;
    
    clearCurrentMap();
    currentInstanceId_ = instance->getInstanceId();
    
    // Calculate rows from capacity
    int capacity = instance->getTotalCapacity();
    seatRows_ = (capacity + seatCols_ - 1) / seatCols_;
    
    // Create all seats
    for (int row = 0; row < seatRows_; row++) {
        for (int col = 0; col < seatCols_; col++) {
            std::string seatId = seatIdToString(row, col);
            SeatType type = determineSeatType(row);
            
            Seat* seat = new Seat(seatId, type, SeatStatus::Available);
            activeSeatMap_.push_back(seat);
        }
    }
    
    // Load booked seats from file
    std::ifstream file(seatStatusFilePath_);
    if (!file.is_open()) {
        return true;  // No bookings yet
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        size_t pos = line.find('|');
        if (pos == std::string::npos) continue;
        
        std::string fileInstanceId = line.substr(0, pos);
        if (fileInstanceId != currentInstanceId_) continue;
        
        std::string bookedSeatsStr = line.substr(pos + 1);
        if (bookedSeatsStr.empty()) break;
        
        // Parse booked seats (format: "A01,B05,C10")
        std::istringstream ss(bookedSeatsStr);
        std::string seatId;
        while (std::getline(ss, seatId, ',')) {
            for (int i = 0; i < activeSeatMap_.size(); i++) {
                if (activeSeatMap_[i]->getId() == seatId) {
                    activeSeatMap_[i]->setStatus(SeatStatus::Booked);
                    break;
                }
            }
        }
        break;
    }
    file.close();
    
    return true;
}

bool SeatManager::selectSeat(const std::string& seatId) {
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getId() == seatId) {
            if (activeSeatMap_[i]->getStatus() == SeatStatus::Available) {
                selectedSeat_ = activeSeatMap_[i];
                return true;
            }
            return false;
        }
    }
    return false;
}

void SeatManager::cancelSelection() {
    selectedSeat_ = nullptr;
}

bool SeatManager::confirmSelection() {
    if (!selectedSeat_) return false;
    
    std::string seatId = selectedSeat_->getId();
    selectedSeat_ = nullptr;  // IMPORTANT: Reset before returning
    
    return bookSeat(seatId);
}

bool SeatManager::bookSeat(const std::string& seatId) {
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getId() == seatId) {
            if (activeSeatMap_[i]->getStatus() == SeatStatus::Available) {
                activeSeatMap_[i]->setStatus(SeatStatus::Booked);
                return updateAndSaveChanges();
            }
            return false;
        }
    }
    return false;
}

bool SeatManager::releaseSeat(const std::string& seatId) {
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getId() == seatId) {
            if (activeSeatMap_[i]->getStatus() == SeatStatus::Booked) {
                activeSeatMap_[i]->setStatus(SeatStatus::Available);
                return updateAndSaveChanges();
            }
            return false;
        }
    }
    return false;
}

bool SeatManager::saveChanges() {
    return updateAndSaveChanges();
}

bool SeatManager::updateAndSaveChanges() {
    if (currentInstanceId_.empty()) return false;
    
    // Read existing file
    std::vector<std::string> allLines;
    std::ifstream inFile(seatStatusFilePath_);
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            allLines.push_back(line);
        }
        inFile.close();
    }
    
    // Build booked seats string
    std::ostringstream bookedSeats;
    bool first = true;
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getStatus() == SeatStatus::Booked) {
            if (!first) bookedSeats << ",";
            bookedSeats << activeSeatMap_[i]->getId();
            first = false;
        }
    }
    
    std::string newLine = currentInstanceId_ + "|" + bookedSeats.str();
    
    // Update or add line
    bool found = false;
    for (size_t i = 0; i < allLines.size(); i++) {
        if (allLines[i].find(currentInstanceId_ + "|") == 0) {
            allLines[i] = newLine;
            found = true;
            break;
        }
    }
    
    if (!found) {
        allLines.push_back(newLine);
    }
    
    // Write back
    std::ofstream outFile(seatStatusFilePath_);
    if (!outFile.is_open()) return false;
    
    for (const auto& line : allLines) {
        outFile << line << "\n";
    }
    outFile.close();
    
    return true;
}

DynamicArray<Seat*>* SeatManager::getActiveSeatMap() {
    return &activeSeatMap_;
}

const DynamicArray<Seat*>& SeatManager::getAllSeats() const {
    return activeSeatMap_;
}

Seat* SeatManager::getSelectedSeat() const {
    return selectedSeat_;
}

const std::string& SeatManager::getCurrentInstanceId() const {
    return currentInstanceId_;
}

int SeatManager::getSeatRows() const {
    return seatRows_;
}

int SeatManager::getSeatColumns() const {
    return seatCols_;
}

int SeatManager::getAvailableSeats(SeatClass seatClass) const {
    int count = 0;
    int businessRows = seatRows_ / 5;  // 20% Business
    
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        std::string id = activeSeatMap_[i]->getId();
        int row = (id[0] - 'A');
        
        bool isBusiness = (row < businessRows);
        bool match = (seatClass == SeatClass::Business && isBusiness) ||
                     (seatClass == SeatClass::Economy && !isBusiness);
        
        if (match && activeSeatMap_[i]->getStatus() == SeatStatus::Available) {
            count++;
        }
    }
    
    return count;
}

void SeatManager::clearCurrentMap() {
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        delete activeSeatMap_[i];
    }
    activeSeatMap_.clear();
    currentInstanceId_ = "";
    selectedSeat_ = nullptr;
}

std::string SeatManager::seatIdToString(int row, int col) const {
    char rowChar = 'A' + row;
    std::string colStr = (col < 9) ? ("0" + std::to_string(col + 1)) : std::to_string(col + 1);
    return std::string(1, rowChar) + colStr;
}

SeatType SeatManager::determineSeatType(int row) const {
    int businessRows = seatRows_ / 5;  // First 20% are Business
    return (row < businessRows) ? SeatType::Business : SeatType::Economy;
}
