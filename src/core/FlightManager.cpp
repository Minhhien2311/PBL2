#include "C:/PBL2/include/core/FlightManager.h" 
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
                // <<< THAY ĐỔI: Tạo đối tượng trên heap bằng 'new'
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
                // <<< THAY ĐỔI: Tạo đối tượng trên heap bằng 'new'
                FlightInstance instanceOnStack = FlightInstance::fromRecordLine(line);
                this->allInstances.push_back(new FlightInstance(instanceOnStack));
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
    // "Người Gác Cổng" kiểm tra dữ liệu đầu vào.
    if (number.empty() || airline.empty() || departureIATA.empty() || arrivalIATA.empty()) {
        return false; // Các thông tin cơ bản không được rỗng.
    }

    // Kiểm tra xem flightNumber đã tồn tại chưa để tránh trùng lặp.
    for (size_t i = 0; i < allFlights.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        if (allFlights[i]->getFlightNumber() == number) {
            return false; // Đã có tuyến bay với số hiệu này.
        }
    }

    // <<< THAY ĐỔI: Tạo đối tượng trên heap bằng 'new'
    this->allFlights.push_back(new Flight(number, airline, departureIATA, arrivalIATA));
    return true;
}

bool FlightManager::createNewInstance(const std::string& flightId,
                                      const std::string& departureIso,
                                      const std::string& arrivalIso,
                                      int totalEconomySeats,
                                      int totalBusinessSeats,
                                      double fareEconomy,
                                      double fareBusiness) {
    // "Người Gác Cổng" kiểm tra dữ liệu.
    // 1. Kiểm tra tính hợp lệ của flightId (phải tồn tại trong danh sách allFlights).
    if (findFlightById(flightId) == nullptr) {
        return false; // Không thể tạo chuyến bay cho một tuyến bay không tồn tại.
    }
    // 2. Kiểm tra các giá trị số.
    if (totalEconomySeats < 0 || totalBusinessSeats < 0 || fareEconomy < 0.0 || fareBusiness < 0.0) {
        return false; // Số ghế và giá vé không được âm.
    }
    // 3. Kiểm tra dữ liệu rỗng. //Thêm cái này
    if (flightId.empty() || departureIso.empty() || arrivalIso.empty() || totalEconomySeats == 0 || totalBusinessSeats == 0 || fareEconomy == 0 || fareBusiness == 0)
        return false;

    // <<< THAY ĐỔI: Tạo đối tượng trên heap bằng 'new'
    this->allInstances.push_back(new FlightInstance(flightId, departureIso, arrivalIso, totalEconomySeats, totalBusinessSeats, fareEconomy, fareBusiness));
    return true;
}

// --- Chức năng Đọc/Tìm kiếm (Read) ---

Flight* FlightManager::findFlightById(const std::string& flightId) {
    // Tìm kiếm tuyến tính (chưa tối ưu, sẽ nâng cấp bằng Bảng Băm sau).
    for (size_t i = 0; i < allFlights.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        if (allFlights[i]->getFlightId() == flightId) {
            // <<< THAY ĐỔI: Trả về con trỏ trực tiếp
            return allFlights[i]; 
        }
    }
    return nullptr; // Không tìm thấy.
}

FlightInstance* FlightManager::findInstanceById(const std::string& instanceId) {
    // Tìm kiếm tuyến tính.
    for (size_t i = 0; i < allInstances.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        if (allInstances[i]->getInstanceId() == instanceId) {
            // <<< THAY ĐỔI: Trả về con trỏ trực tiếp
            return allInstances[i];
        }
    }
    return nullptr;
}

// Tìm tất cả các chuyến bay cụ thể của một tuyến bay gốc.
DynamicArray<FlightInstance*> FlightManager::findInstancesByFlightId(const std::string& flightId) {
    DynamicArray<FlightInstance*> results;
    for (size_t i = 0; i < allInstances.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        if (allInstances[i]->getFlightId() == flightId) {
            // <<< THAY ĐỔI: Thêm con trỏ trực tiếp vào mảng kết quả.
            results.push_back(allInstances[i]);
        }
    }
    return results;
}

// --- Chức năng Lưu trữ (Persistence) ---

bool FlightManager::saveFlightsToFiles(const std::string& flightsFilePath) const {
    // Lưu danh sách Flight
    std::ofstream flightsFile(flightsFilePath);
    if (!flightsFile.is_open()) return false;
    for (size_t i = 0; i < allFlights.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        flightsFile << allFlights[i]->toRecordLine() << "\n";
    }
    flightsFile.close();
    return true;
}

bool FlightManager::saveInstancesToFiles(const std::string& instancesFilePath) const {
    // Lưu danh sách FlightInstance
    std::ofstream instancesFile(instancesFilePath);
    if (!instancesFile.is_open()) return false;
    for (size_t i = 0; i < allInstances.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        instancesFile << allInstances[i]->toRecordLine() << "\n";
    }
    instancesFile.close();

    return true;
}

DynamicArray<Flight*> FlightManager::getAllFlights() const{
    return this->allFlights;
}     

DynamicArray<FlightInstance*> FlightManager::getAllInstances() const{
    return this->allInstances;
}