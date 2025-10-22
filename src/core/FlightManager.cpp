#include "C:/PBL2/include/core/FlightManager.h"
#include "C:/PBL2/include/utils/GenID.h"
#include <fstream>
#include <string>

// --- Constructor & Destructor ---

FlightManager::FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath) {
    // Constructor gọi các hàm helper để nạp dữ liệu từ file ngay khi được tạo.
    this->loadFlightsFromFile(flightsFilePath);
    this->loadInstancesFromFile(instancesFilePath);
}

// --- Hàm trợ giúp nội bộ ---

void FlightManager::loadFlightsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // 1. Tạo đối tượng Flight tạm trên stack từ dữ liệu file
                Flight flightOnStack = Flight::fromRecordLine(line);
                // 2. Tạo bản sao trên heap
                Flight* newFlight = new Flight(flightOnStack);
                
                if (newFlight) { // Kiểm tra cấp phát thành công
                    // 3. Lấy index *tiếp theo* (chính là size hiện tại)
                    size_t newIndex = static_cast<size_t>(allFlights.size());
                    // 4. Thêm vào mảng (lúc này size() mới tăng lên)
                    allFlights.push_back(newFlight);
                    // 5. Thêm vào HashTable: ánh xạ ID tới vị trí (index) vừa thêm
                    flightIdMap.insert(newFlight->getFlightId(), newIndex);
                }
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
                // Tương tự như loadFlightsFromFile
                FlightInstance instanceOnStack = FlightInstance::fromRecordLine(line);
                FlightInstance* newInstance = new FlightInstance(instanceOnStack);
                 if (newInstance) {
                    size_t newIndex = static_cast<size_t>(allInstances.size());
                    allInstances.push_back(newInstance);
                    instanceIdMap.insert(newInstance->getInstanceId(), newIndex);
                }
            }
        }
        file.close();
    }
}

// --- Chức năng Tạo mới (Create) ---

bool FlightManager::createNewFlight(const std::string& number,
                                     const std::string& airline,
                                     const std::string& departureIATA,
                                     const std::string& arrivalIATA) {
    // 1. Kiểm tra dữ liệu đầu vào
    if (number.empty() || airline.empty() || departureIATA.empty() || arrivalIATA.empty()) {
        return false;
    }
    // 2. Kiểm tra trùng lặp số hiệu chuyến bay (Flight Number, vd: "VN123")
    // (Lưu ý: cách này duyệt O(N), nếu cần tối ưu có thể thêm 1 HashTable nữa cho flightNumber)
    for (int i = 0; i < allFlights.size(); ++i) {
        if (allFlights[i] && allFlights[i]->getFlightNumber() == number) {
            return false; // Đã tồn tại
        }
    }

    // 3. Tạo đối tượng mới trên heap
    // (Constructor của Flight sẽ tự động gọi GenID để tạo ID mới, vd: "FL-005")
    Flight* newFlight = new Flight(number, airline, departureIATA, arrivalIATA);
    if (!newFlight) return false; // Lỗi cấp phát bộ nhớ

    // 4. Thêm vào mảng
    size_t newIndex = static_cast<size_t>(allFlights.size());
    allFlights.push_back(newFlight);

    // 5. Thêm vào HashTable để tra cứu bằng ID
    flightIdMap.insert(newFlight->getFlightId(), newIndex);

    return true;
}

bool FlightManager::createNewInstance(const std::string& flightId,
                                      const std::string& departureIso,
                                      const std::string& arrivalIso,
                                      int totalEconomySeats,
                                      int totalBusinessSeats,
                                      double fareEconomy,
                                      double fareBusiness) {
    // 1. Kiểm tra Flight gốc (vd: "FL-001") có tồn tại không
    if (findFlightById(flightId) == nullptr) {
        return false; // Không tìm thấy tuyến bay gốc
    }
    // 2. Kiểm tra dữ liệu đầu vào
    if (departureIso.empty() || arrivalIso.empty() || totalEconomySeats < 0 || totalBusinessSeats < 0 || fareEconomy < 0.0 || fareBusiness < 0.0) {
        return false;
    }

    // 3. Tạo đối tượng mới trên heap
    // (Constructor của FlightInstance sẽ tự gọi GenID để tạo ID mới, vd: "FI-10006")
    FlightInstance* newInstance = new FlightInstance(flightId, departureIso, arrivalIso, totalEconomySeats, totalBusinessSeats, fareEconomy, fareBusiness);
    if (!newInstance) return false;

    // 4. Thêm vào mảng
    size_t newIndex = static_cast<size_t>(allInstances.size());
    allInstances.push_back(newInstance);

    // 5. Thêm vào HashTable để tra cứu
    instanceIdMap.insert(newInstance->getInstanceId(), newIndex);

    return true;
}

// --- Chức năng Đọc/Tìm kiếm (Read) ---

Flight* FlightManager::findFlightById(const std::string& flightId) {
    // 1. Dùng HashTable để tìm index
    size_t* indexPtr = flightIdMap.find(flightId);
    if (indexPtr == nullptr) {
        return nullptr; // Không có trong HashTable
    }
    
    // 2. Lấy giá trị index
    int index = static_cast<int>(*indexPtr);
    
    // 3. Kiểm tra tính hợp lệ của index (phòng trường hợp dữ liệu lỗi)
    if (index < 0 || index >= allFlights.size()) {
        flightIdMap.remove(flightId); // Xóa entry lỗi khỏi map
        return nullptr;
    }
    // 4. Trả về con trỏ tại vị trí index
    return allFlights[index];
}

FlightInstance* FlightManager::findInstanceById(const std::string& instanceId) {
    // Tương tự hàm findFlightById
    size_t* indexPtr = instanceIdMap.find(instanceId);
    if (indexPtr == nullptr) {
        return nullptr;
    }
    int index = static_cast<int>(*indexPtr);
    if (index < 0 || index >= allInstances.size()) {
        instanceIdMap.remove(instanceId); // Xóa entry lỗi
        return nullptr;
    }
    return allInstances[index];
}

void FlightManager::findInstancesByFlightId(const std::string& flightId, DynamicArray<FlightInstance*>& results) const {
    // 1. Xóa sạch kết quả cũ trong mảng 'results'
    results.clear();

    // 2. Duyệt toàn bộ mảng 'allInstances' (O(N))
    for (int i = 0; i < allInstances.size(); ++i) {
        if (allInstances[i] && allInstances[i]->getFlightId() == flightId) {
            // 3. Nếu tìm thấy, thêm con trỏ vào mảng kết quả
            results.push_back(allInstances[i]);
        }
    }
}

// --- CÁC HÀM XÓA ---

bool FlightManager::removeFlightById(const std::string& flightId) {
    // 1. Tìm vị trí (index) của Flight cần xóa
    size_t* indexPtr = flightIdMap.find(flightId);
    if (indexPtr == nullptr) {
        return false; // Không tìm thấy
    }
    int indexToDelete = static_cast<int>(*indexPtr);

    if (indexToDelete < 0 || indexToDelete >= allFlights.size()) {
        flightIdMap.remove(flightId); 
        return false; // Lỗi dữ liệu
    }

    // 2. Xóa khỏi DynamicArray.
    // Hàm erase(index) sẽ:
    //    a. `delete allFlights[indexToDelete]` (giải phóng bộ nhớ)
    //    b. Dịch chuyển các phần tử từ [indexToDelete + 1] trở về sau lên 1 vị trí.
    bool erased = allFlights.erase(indexToDelete);
    if (!erased) {
        return false; // Lỗi khi xóa
    }

    // 3. Xóa entry cũ khỏi HashTable
    flightIdMap.remove(flightId);

    // 4. QUAN TRỌNG: Cập nhật lại index trong HashTable
    // Vì các phần tử từ [indexToDelete] trở đi đã bị dịch chuyển,
    // ta phải cập nhật lại index mới của chúng trong HashTable.
    for (int i = indexToDelete; i < allFlights.size(); ++i) {
        if(allFlights[i]) {
             // Dùng 'insert' để ghi đè (hoặc thêm mới nếu chưa có)
             // Ánh xạ ID tới vị trí `i` (là vị trí mới của nó)
             flightIdMap.insert(allFlights[i]->getFlightId(), static_cast<size_t>(i));
        }
    }

    return true;
}

bool FlightManager::removeInstanceById(const std::string& instanceId) {
    // Logic tương tự như removeFlightById
    size_t* indexPtr = instanceIdMap.find(instanceId);
    if (indexPtr == nullptr) {
        return false;
    }
    int indexToDelete = static_cast<int>(*indexPtr);

    if (indexToDelete < 0 || indexToDelete >= allInstances.size()) {
        instanceIdMap.remove(instanceId);
        return false;
    }

    // 1. Xóa khỏi mảng (DynamicArray tự 'delete' con trỏ)
    bool erased = allInstances.erase(indexToDelete);
     if (!erased) {
        return false;
    }

    // 2. Xóa khỏi HashTable
    instanceIdMap.remove(instanceId);

    // 3. Cập nhật lại index trong HashTable cho các phần tử bị dịch
    for (int i = indexToDelete; i < allInstances.size(); ++i) {
         if(allInstances[i]) {
            instanceIdMap.insert(allInstances[i]->getInstanceId(), static_cast<size_t>(i));
         }
    }

    return true;
}

// --- Chức năng Lưu trữ (Persistence) ---

bool FlightManager::saveFlightsToFiles(const std::string& flightsFilePath) const {
    std::ofstream flightsFile(flightsFilePath);
    if (!flightsFile.is_open()) return false;
    for (int i = 0; i < allFlights.size(); ++i) {
        if (allFlights[i]) {
            flightsFile << allFlights[i]->toRecordLine() << "\n";
        }
    }
    flightsFile.close();
    return true;
}

bool FlightManager::saveInstancesToFiles(const std::string& instancesFilePath) const {
    std::ofstream instancesFile(instancesFilePath);
    if (!instancesFile.is_open()) return false;
    for (int i = 0; i < allInstances.size(); ++i) {
        if (allInstances[i]) {
            instancesFile << allInstances[i]->toRecordLine() << "\n";
        }
    }
    instancesFile.close();
    return true;
}

// --- Getters (Thêm) ---
DynamicArray<Flight*> FlightManager::getAllFlights() const {
    return allFlights;
}

DynamicArray<FlightInstance*> FlightManager::getAllInstances() const {
    return allInstances;
}