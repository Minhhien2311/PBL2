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
{}

SeatManager::~SeatManager() {
    clearCurrentMap();
}

bool SeatManager::loadForFlight(const std::string& flightId) {
    clearCurrentMap();
    currentFlightId_ = flightId;
    
    // Sẽ được gọi bởi loadSeatMapFor(Flight* flight)
    return true;
}

bool SeatManager::loadSeatMapFor(Flight* flight) {
    if (!flight) return false;
    
    clearCurrentMap();
    currentFlightId_ = flight->getFlightId();
    
    // Tính toán số hàng ghế dựa trên sức chứa
    int capacity = flight->getTotalCapacity();
    seatRows_ = (capacity + seatCols_ - 1) / seatCols_;
    
    // Tạo bản đồ ghế trống
    for (int row = 0; row < seatRows_; row++) {
        for (int col = 0; col < seatCols_; col++) {
            std::string seatId = seatIdToString(row, col);
            SeatType type = determineSeatType(row);
            
            Seat* seat = new Seat(seatId, type, SeatStatus::Available);
            activeSeatMap_.push_back(seat);
        }
    }
    
    // Tải trạng thái ghế đã đặt từ file
    std::ifstream file(seatStatusFilePath_);
    if (!file.is_open()) {
        return true;  // Nếu không mở được file, coi như chưa có ghế nào được đặt
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
    selectedSeat_ = nullptr;  // Reset trước khi return
    
    return bookSeat(seatId);
}

bool SeatManager::bookSeat(const std::string& seatId) {
    
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getId() == seatId) {
            // Log status hiện tại
            std::string statusStr = (activeSeatMap_[i]->getStatus() == SeatStatus::Available) 
                                    ? "Available" : "Booked";
            
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
    
    if (currentFlightId_.empty()) {
        std::cerr << "[ERROR] currentFlightId_ is empty!" << std::endl;
        return false;
    }
    
    // Log đọc file
    std::vector<std::string> allLines;
    std::ifstream inFile(seatStatusFilePath_);
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            allLines.push_back(line);
        }
        inFile.close();
    } else {
        std::cout << "[DEBUG] File not found, will create new" << std::endl;
    }
    
    // Tạo dãy ghế đã đặt
    std::ostringstream bookedSeats;
    bool first = true;
    int bookedCount = 0;
    
    for (int i = 0; i < activeSeatMap_.size(); i++) {
        if (activeSeatMap_[i]->getStatus() == SeatStatus::Booked) {
            if (!first) bookedSeats << ",";
            bookedSeats << activeSeatMap_[i]->getId();
            first = false;
            bookedCount++;
        }
    }
    
    std::string newLine = currentFlightId_ + "|" + bookedSeats.str();
    
    // Cập nhật hoặc thêm dòng mới
    bool found = false;
    for (size_t i = 0; i < allLines.size(); i++) {
        if (allLines[i].find(currentFlightId_ + "|") == 0) {
            allLines[i] = newLine;
            found = true;
            break;
        }
    }
    
    if (!found) {
        allLines.push_back(newLine);
    }
    
    // Ghi lại toàn bộ file
    std::ofstream outFile(seatStatusFilePath_);
    if (!outFile.is_open()) {
        std::cerr << "[ERROR] Cannot open file for writing: " << seatStatusFilePath_ << std::endl;
        std::cerr << "[ERROR] Possible reasons:" << std::endl;
        std::cerr << "  - File is locked by another process" << std::endl;
        std::cerr << "  - No write permission" << std::endl;
        std::cerr << "  - Directory does not exist" << std::endl;
        std::cerr << "  - Disk is full" << std::endl;
        return false;
    }
    
    for (const auto& line : allLines) {
        outFile << line << "\n";
    }
    outFile.close();
    
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
    // Định dạng ID ghế: {Chữ cái cột}{Số hàng có padding} ví dụ: A01, B12, H25
    char colChar = 'A' + col;
    int rowNum = row + 1;
    
    // Thêm số 0 ở đầu cho các số hàng < 10
    std::string rowStr = (rowNum < 10) ? ("0" + std::to_string(rowNum)) : std::to_string(rowNum);
    
    return std::string(1, colChar) + rowStr;
}

SeatType SeatManager::determineSeatType(int row) const {
    int businessRows = seatRows_ / 5;  // 20% đầu là Business
    return (row < businessRows) ? SeatType::Business : SeatType::Economy;
}