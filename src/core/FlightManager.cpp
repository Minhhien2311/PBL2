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
                // Sửa lại: Gọi fromRecordLine (giả sử nó trả về con trỏ) và thêm trực tiếp
                // Tạo đối tượng Flight trên stack, sau đó cấp phát trên heap và sao chép
                Flight flightOnStack = Flight::fromRecordLine(line);
                Flight* newFlight = new Flight(flightOnStack);
                if (newFlight) { // Kiểm tra con trỏ hợp lệ (cấp phát thành công)
                    // Lấy index mới nhất (chuyển đổi kiểu tường minh từ int sang size_t)
                    size_t newIndex = static_cast<size_t>(allFlights.size() - 1);
                    // Thêm vào HashTable
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
                 // Sửa lại: Gọi fromRecordLine (giả sử nó trả về con trỏ) và thêm trực tiếp
                FlightInstance instanceOnStack = FlightInstance::fromRecordLine(line);
                FlightInstance* newInstance = new FlightInstance(instanceOnStack);
                 if (newInstance) { // Kiểm tra con trỏ hợp lệ (cấp phát thành công)
                    allInstances.push_back(newInstance);
                     // Lấy index mới nhất (chuyển đổi kiểu tường minh)
                    size_t newIndex = static_cast<size_t>(allInstances.size() - 1);
                     // Thêm vào HashTable
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
    if (number.empty() || airline.empty() || departureIATA.empty() || arrivalIATA.empty()) {
        return false;
    }
    // Kiểm tra trùng lặp số hiệu chuyến bay (Flight Number)
    // Cách này chưa tối ưu, nếu cần có thể tạo thêm HashTable cho flightNumber
    for (int i = 0; i < allFlights.size(); ++i) { // Dùng int vì DynamicArray::size() trả về int
        if (allFlights[i] && allFlights[i]->getFlightNumber() == number) { // Kiểm tra nullptr
            return false;
        }
    }

    // Tạo ID mới
    std::string newId = IdGenerator::generateFlightId(); // Đảm bảo bạn đã có hàm này

    // Tạo đối tượng trên heap
    // *** GIẢ ĐỊNH: Constructor Flight đã được sửa để nhận ID ***
    Flight* newFlight = new Flight(/*newId,*/ number, airline, departureIATA, arrivalIATA);
    if (!newFlight) return false; // Kiểm tra cấp phát bộ nhớ

    // Thêm vào DynamicArray
    allFlights.push_back(newFlight);

    // Thêm vào HashTable
    size_t newIndex = static_cast<size_t>(allFlights.size() - 1);
    flightIdMap.insert(newFlight->getFlightId(), newIndex); // Lấy ID thực từ đối tượng vừa tạo

    return true;
}

bool FlightManager::createNewInstance(const std::string& flightId,
                                      const std::string& departureIso,
                                      const std::string& arrivalIso,
                                      int totalEconomySeats,
                                      int totalBusinessSeats,
                                      double fareEconomy,
                                      double fareBusiness) {
    // Kiểm tra Flight gốc có tồn tại không (dùng hàm find đã tối ưu)
    if (findFlightById(flightId) == nullptr) { // Đã dùng hàm find tối ưu
        return false;
    }
    if (departureIso.empty() || arrivalIso.empty() || totalEconomySeats < 0 || totalBusinessSeats < 0 || fareEconomy < 0.0 || fareBusiness < 0.0) {
        return false;
    }

     // Tạo ID mới
    std::string newId = IdGenerator::generateInstanceId(); // Đảm bảo bạn đã có hàm này

    // Tạo đối tượng trên heap
    // *** GIẢ ĐỊNH: Constructor FlightInstance đã được sửa để nhận ID ***
    FlightInstance* newInstance = new FlightInstance(/*newId,*/ flightId, departureIso, arrivalIso, totalEconomySeats, totalBusinessSeats, fareEconomy, fareBusiness);
    if (!newInstance) return false; // Kiểm tra cấp phát

    // Thêm vào DynamicArray
    allInstances.push_back(newInstance);

    // Thêm vào HashTable
    size_t newIndex = static_cast<size_t>(allInstances.size() - 1);
    instanceIdMap.insert(newInstance->getInstanceId(), newIndex); // Lấy ID thực

    return true;
}

// --- Chức năng Đọc/Tìm kiếm (Read) ---

// Bỏ const để cho phép sửa đổi đối tượng trả về nếu cần
Flight* FlightManager::findFlightById(const std::string& flightId) {
    size_t* indexPtr = flightIdMap.find(flightId);
    if (indexPtr == nullptr) {
        return nullptr;
    }
    // Chuyển đổi kiểu và kiểm tra index hợp lệ
    int index = static_cast<int>(*indexPtr);
    if (index < 0 || index >= allFlights.size()) {
         // Dữ liệu không nhất quán, xóa entry lỗi khỏi map
        flightIdMap.remove(flightId);
        return nullptr;
    }
    return allFlights[index];
}

// Bỏ const để cho phép sửa đổi
FlightInstance* FlightManager::findInstanceById(const std::string& instanceId) {
    size_t* indexPtr = instanceIdMap.find(instanceId);
    if (indexPtr == nullptr) {
        return nullptr;
    }
    // Chuyển đổi kiểu và kiểm tra index hợp lệ
    int index = static_cast<int>(*indexPtr);
    if (index < 0 || index >= allInstances.size()) {
        instanceIdMap.remove(instanceId); // Xóa entry lỗi
        return nullptr;
    }
    return allInstances[index];
}

// Tìm tất cả các chuyến bay cụ thể của một tuyến bay gốc.
// Sửa lại để nhận DynamicArray qua tham chiếu (đã làm ở lần trước)
void FlightManager::findInstancesByFlightId(const std::string& flightId, DynamicArray<FlightInstance*>& results) const {
    // Xóa sạch kết quả cũ trước khi tìm kiếm mới
    // *** GIẢ ĐỊNH: DynamicArray có hàm clear() ***
    results.clear();

    // Duyệt qua allInstances
    for (int i = 0; i < allInstances.size(); ++i) { // Dùng int vì size() trả về int
        if (allInstances[i] && allInstances[i]->getFlightId() == flightId) { // Kiểm tra nullptr
            results.push_back(allInstances[i]);
        }
    }
}

// --- CÁC HÀM XÓA MỚI (Logic cập nhật index) ---

bool FlightManager::removeFlightById(const std::string& flightId) {
    size_t* indexPtr = flightIdMap.find(flightId);
    if (indexPtr == nullptr) {
        return false; // Không tìm thấy
    }
    int indexToDelete = static_cast<int>(*indexPtr); // Chuyển sang int

    // Kiểm tra index hợp lệ trước khi xóa
     if (indexToDelete < 0 || indexToDelete >= allFlights.size()) {
        flightIdMap.remove(flightId); // Xóa khỏi map nếu index không hợp lệ
        return false; // Lỗi dữ liệu
    }

    // Bước 1: Xóa khỏi DynamicArray (hàm erase tự động gọi delete đối tượng)
    bool erased = allFlights.erase(indexToDelete);
    if (!erased) {
        return false; // Lỗi khi xóa khỏi DynamicArray
    }

    // Bước 2: Xóa khỏi HashTable
    flightIdMap.remove(flightId);

    // Bước 3: Cập nhật lại index trong HashTable cho các phần tử bị dịch chuyển
    for (int i = indexToDelete; i < allFlights.size(); ++i) { // Dùng int
        // Cập nhật lại index mới (kiểu size_t) cho ID tương ứng trong Map
        if(allFlights[i]) { // Kiểm tra nullptr
             flightIdMap.insert(allFlights[i]->getFlightId(), static_cast<size_t>(i));
        }
    }

    return true;
}

bool FlightManager::removeInstanceById(const std::string& instanceId) {
    size_t* indexPtr = instanceIdMap.find(instanceId);
    if (indexPtr == nullptr) {
        return false;
    }
    int indexToDelete = static_cast<int>(*indexPtr); // Chuyển sang int

    // Kiểm tra index hợp lệ
    if (indexToDelete < 0 || indexToDelete >= allInstances.size()) {
        instanceIdMap.remove(instanceId);
        return false; // Lỗi dữ liệu
    }

    // Bước 1: Xóa khỏi DynamicArray
    bool erased = allInstances.erase(indexToDelete);
     if (!erased) {
        return false;
    }

    // Bước 2: Xóa khỏi HashTable
    instanceIdMap.remove(instanceId);

    // Bước 3: Cập nhật index trong HashTable
    for (int i = indexToDelete; i < allInstances.size(); ++i) { // Dùng int
         if(allInstances[i]) { // Kiểm tra nullptr
            instanceIdMap.insert(allInstances[i]->getInstanceId(), static_cast<size_t>(i));
         }
    }

    return true;
}

// --- Chức năng Lưu trữ (Persistence) ---

bool FlightManager::saveFlightsToFiles(const std::string& flightsFilePath) const {
    std::ofstream flightsFile(flightsFilePath);
    if (!flightsFile.is_open()) return false;
    for (int i = 0; i < allFlights.size(); ++i) { // Dùng int
        // Thêm kiểm tra con trỏ trước khi ghi
        if (allFlights[i]) {
            flightsFile << allFlights[i]->toRecordLine() << "\n";
        }
    }
    flightsFile.close(); // Đặt close() ra ngoài vòng lặp
    return true;
}

bool FlightManager::saveInstancesToFiles(const std::string& instancesFilePath) const {
    std::ofstream instancesFile(instancesFilePath);
    if (!instancesFile.is_open()) return false;
    for (int i = 0; i < allInstances.size(); ++i) { // Dùng int
         // Thêm kiểm tra con trỏ trước khi ghi
        if (allInstances[i]) {
            instancesFile << allInstances[i]->toRecordLine() << "\n";
        }
    }
    instancesFile.close();
    return true;
}