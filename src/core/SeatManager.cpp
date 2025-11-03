#include "core/SeatManager.h"
#include <sstream>

// --- Constructor & Destructor ---

SeatManager::SeatManager(HashTable<std::string, std::string>& cache)
    : seatDataCache(cache), activeSeatMap(nullptr), activeInstanceId(""), isDirty(false) {
}

SeatManager::~SeatManager() {
    unload();
}

// --- Hàm helper private ---

DynamicArray<Seat*>* SeatManager::generateDefaultSeatMap(FlightInstance* instance) {
    if (!instance) return nullptr;

    auto* seatMap = new DynamicArray<Seat*>();
    
    int businessRows = (instance->getBusinessTotal() + DEFAULT_SEAT_COLS - 1) / DEFAULT_SEAT_COLS;
    int economyRows = (instance->getEconomyTotal() + DEFAULT_SEAT_COLS - 1) / DEFAULT_SEAT_COLS;

    // Tạo ghế Business (hàng 1 đến businessRows)
    for (int row = 1; row <= businessRows; ++row) {
        for (int col = 0; col < DEFAULT_SEAT_COLS; ++col) {
            std::string seatId = Seat::coordinatesToId(row, col);
            Seat* seat = new Seat(seatId, SeatStatus::Available, SeatType::Business);
            seatMap->push_back(seat);
        }
    }

    // Tạo ghế Economy (hàng businessRows+1 đến businessRows+economyRows)
    for (int row = businessRows + 1; row <= businessRows + economyRows; ++row) {
        for (int col = 0; col < DEFAULT_SEAT_COLS; ++col) {
            std::string seatId = Seat::coordinatesToId(row, col);
            Seat* seat = new Seat(seatId, SeatStatus::Available, SeatType::Economy);
            seatMap->push_back(seat);
        }
    }

    return seatMap;
}

DynamicArray<Seat*>* SeatManager::parseSeatData(const std::string& data) {
    auto* seatMap = new DynamicArray<Seat*>();
    
    std::stringstream ss(data);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            Seat* seat = new Seat(Seat::fromRecordLine(line));
            seatMap->push_back(seat);
        }
    }
    
    return seatMap;
}

std::string SeatManager::serializeSeatMap(DynamicArray<Seat*>* seatMap) {
    if (!seatMap) return "";
    
    std::string result;
    for (int i = 0; i < seatMap->size(); ++i) {
        if ((*seatMap)[i]) {
            result += (*seatMap)[i]->toRecordLine();
            if (i < seatMap->size() - 1) {
                result += "\n";
            }
        }
    }
    
    return result;
}

// --- Chức năng chính ---

bool SeatManager::loadSeatMapFor(FlightInstance* instance) {
    if (!instance) return false;

    // Nếu đang load instance khác, unload trước
    if (activeSeatMap && activeInstanceId != instance->getInstanceId()) {
        unload();
    }

    // Nếu đã load instance này rồi, không cần load lại
    if (activeSeatMap && activeInstanceId == instance->getInstanceId()) {
        return true;
    }

    // Lưu instance ID
    activeInstanceId = instance->getInstanceId();

    // Kiểm tra cache
    std::string* cachedData = seatDataCache.find(activeInstanceId);
    
    if (cachedData && !cachedData->empty()) {
        // Parse từ cache
        activeSeatMap = parseSeatData(*cachedData);
    } else {
        // Tạo sơ đồ mặc định
        activeSeatMap = generateDefaultSeatMap(instance);
        
        // Lưu vào cache
        std::string serialized = serializeSeatMap(activeSeatMap);
        seatDataCache.insert(activeInstanceId, serialized);
    }

    isDirty = false;
    return activeSeatMap != nullptr;
}

bool SeatManager::bookSeat(const std::string& seatId) {
    if (!activeSeatMap) return false;

    // Tìm ghế trong sơ đồ
    for (int i = 0; i < activeSeatMap->size(); ++i) {
        Seat* seat = (*activeSeatMap)[i];
        if (seat && seat->getId() == seatId) {
            // Chỉ đặt được ghế Available
            if (seat->getStatus() == SeatStatus::Available) {
                seat->setStatus(SeatStatus::Booked);
                isDirty = true;
                return true;
            }
            return false;
        }
    }
    
    return false;
}

bool SeatManager::releaseSeat(const std::string& seatId) {
    if (!activeSeatMap) return false;

    // Tìm ghế trong sơ đồ
    for (int i = 0; i < activeSeatMap->size(); ++i) {
        Seat* seat = (*activeSeatMap)[i];
        if (seat && seat->getId() == seatId) {
            // Chỉ hủy được ghế Booked
            if (seat->getStatus() == SeatStatus::Booked) {
                seat->setStatus(SeatStatus::Available);
                isDirty = true;
                return true;
            }
            return false;
        }
    }
    
    return false;
}

void SeatManager::saveChanges() {
    if (!isDirty || !activeSeatMap || activeInstanceId.empty()) {
        return;
    }

    // Serialize và lưu vào cache
    std::string serialized = serializeSeatMap(activeSeatMap);
    seatDataCache.insert(activeInstanceId, serialized);
    
    isDirty = false;
}

void SeatManager::unload() {
    // Save trước khi unload nếu có thay đổi
    saveChanges();

    // Giải phóng bộ nhớ
    if (activeSeatMap) {
        for (int i = 0; i < activeSeatMap->size(); ++i) {
            delete (*activeSeatMap)[i];
        }
        delete activeSeatMap;
        activeSeatMap = nullptr;
    }

    activeInstanceId = "";
    isDirty = false;
}

// --- Getters ---

DynamicArray<Seat*>* SeatManager::getActiveSeatMap() const {
    return activeSeatMap;
}

bool SeatManager::isLoaded() const {
    return activeSeatMap != nullptr;
}

int SeatManager::getSeatColumns() const {
    return DEFAULT_SEAT_COLS;
}
