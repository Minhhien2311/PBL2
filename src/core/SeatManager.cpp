#include "core/SeatManager.h"
#include "entities/Flight.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <algorithm>


SeatManager::SeatManager(const std::string& seatStatusPath,
                         const std::string& seatConfigPath)
    : seatStatusFilePath_(seatStatusPath),
      seatConfigFilePath_(seatConfigPath),
      currentFlightId_(""),
      selectedSeat_(nullptr),
      seatRows_(0),
      seatCols_(8)
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
        seatCols_ = 8;
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

bool SeatManager::loadForFlight(const std::string& flightId) {
    clearCurrentMap();
    currentFlightId_ = flightId;
    
    // Will be called by loadSeatMapFor(Flightflight*)
    return true;
}

bool SeatManager::loadSeatMapFor(Flight* flight) {
    if (!flight) return false;
    
    clearCurrentMap();
    currentFlightId_ = flight->getFlightId();
    
    // Calculate rows from capacity
    int capacity = flight->getTotalCapacity();
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
        
        std::string fileflightId = line.substr(0, pos);
        if (fileflightId != currentFlightId_) continue;
        
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

// SeatManager.cpp - Line 136

bool SeatManager::bookSeat(const std::string& seatId) {
    
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getId() == seatId) {
            // ✅ THÊM: Log status hiện tại
            std::string statusStr = (activeSeatMap_[i]->getStatus() == SeatStatus::Available) 
                                    ? "Available" : "Booked";
            std::cout << "[DEBUG] Seat " << seatId << " current status: " << statusStr << std::endl;
            
            if (activeSeatMap_[i]->getStatus() == SeatStatus::Available) {
                activeSeatMap_[i]->setStatus(SeatStatus::Booked);
                std::cout << "[DEBUG] Seat " << seatId << " marked as Booked" << std::endl;
                return updateAndSaveChanges();
            }
            
            // ✅ THÊM: Log lỗi
            std::cerr << "[ERROR] Seat " << seatId << " is already Booked!" << std::endl;
            return false;
        }
    }
    
    // ✅ THÊM: Log không tìm thấy
    std::cerr << "[ERROR] Seat " << seatId << " not found in activeSeatMap_!" << std::endl;
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

// SeatManager.cpp - Line 166

bool SeatManager::updateAndSaveChanges() {
    // ✅ THÊM: Log bắt đầu
    std::cout << "[DEBUG] updateAndSaveChanges() called" << std::endl;
    std::cout << "[DEBUG] currentFlightId_: " << currentFlightId_ << std::endl;
    std::cout << "[DEBUG] seatStatusFilePath_: " << seatStatusFilePath_ << std::endl;
    
    if (currentFlightId_.empty()) {
        std::cerr << "[ERROR] currentFlightId_ is empty!" << std::endl;
        return false;
    }
    
    // ✅ THÊM: Log đọc file
    std::vector<std::string> allLines;
    std::ifstream inFile(seatStatusFilePath_);
    if (inFile.is_open()) {
        std::cout << "[DEBUG] Reading existing file..." << std::endl;
        std::string line;
        while (std::getline(inFile, line)) {
            allLines.push_back(line);
        }
        inFile.close();
        std::cout << "[DEBUG] Read " << allLines.size() << " lines" << std::endl;
    } else {
        std::cout << "[DEBUG] File not found, will create new" << std::endl;
    }
    
    // Build booked seats string
    std::ostringstream bookedSeats;
    bool first = true;
    int bookedCount = 0;  // ✅ THÊM: Đếm số ghế booked
    
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getStatus() == SeatStatus::Booked) {
            if (!first) bookedSeats << ",";
            bookedSeats << activeSeatMap_[i]->getId();
            first = false;
            bookedCount++;  // ✅ THÊM
        }
    }
    
    std::string newLine = currentFlightId_ + "|" + bookedSeats.str();
    std::cout << "[DEBUG] New line: " << newLine << std::endl;
    
    // Update or add line
    bool found = false;
    for (size_t i = 0; i < allLines.size(); i++) {
        if (allLines[i].find(currentFlightId_ + "|") == 0) {
            std::cout << "[DEBUG] Updating existing line " << i << std::endl;
            allLines[i] = newLine;
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::cout << "[DEBUG] Adding new line for flight" << std::endl;
        allLines.push_back(newLine);
    }
    
    // ✅ THÊM: Log trước khi ghi file
    std::cout << "[DEBUG] Opening file for writing: " << seatStatusFilePath_ << std::endl;
    
    // Write back
    std::ofstream outFile(seatStatusFilePath_);
    if (!outFile.is_open()) {
        // ✅ THÊM: Log lỗi chi tiết
        std::cerr << "[ERROR] Cannot open file for writing: " << seatStatusFilePath_ << std::endl;
        std::cerr << "[ERROR] Possible reasons:" << std::endl;
        std::cerr << "  - File is locked by another process" << std::endl;
        std::cerr << "  - No write permission" << std::endl;
        std::cerr << "  - Directory does not exist" << std::endl;
        std::cerr << "  - Disk is full" << std::endl;
        return false;
    }
    
    std::cout << "[DEBUG] Writing " << allLines.size() << " lines to file..." << std::endl;
    
    for (const auto& line : allLines) {
        outFile << line << "\n";
    }
    outFile.close();
    
    // ✅ THÊM: Log thành công
    std::cout << "[DEBUG] File saved successfully!" << std::endl;
    
    return true;
}

std::vector<Seat*>* SeatManager::getActiveSeatMap() {
    return &activeSeatMap_;
}

const std::vector<Seat*>& SeatManager::getAllSeats() const {
    return activeSeatMap_;
}

Seat* SeatManager::getSelectedSeat() const {
    return selectedSeat_;
}

const std::string& SeatManager::getCurrentFlightId() const {
    return currentFlightId_;
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

int SeatManager::getAvailableSeats() const {
    int count = 0;
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getStatus() == SeatStatus::Available) {
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
    currentFlightId_ = "";
    selectedSeat_ = nullptr;
}

std::string SeatManager::seatIdToString(int row, int col) const {
    // Seat ID format: {Column Letter}{Row Number with padding} e.g., A01, B12, H25
    char colChar = 'A' + col;
    int rowNum = row + 1;
    
    // Add leading zero for row numbers < 10
    std::string rowStr = (rowNum < 10) ? ("0" + std::to_string(rowNum)) : std::to_string(rowNum);
    
    return std::string(1, colChar) + rowStr;
}

SeatType SeatManager::determineSeatType(int row) const {
    int businessRows = seatRows_ / 5;  // First 20% are Business
    return (row < businessRows) ? SeatType::Business : SeatType::Economy;
}