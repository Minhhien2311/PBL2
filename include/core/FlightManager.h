#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H

#include "C:/PBL2/include/DSA/DynamicArray.h"           // CTDL nền tảng để lưu trữ
#include "C:/PBL2/include/entities/Flight.h"            // Đối tượng tuyến bay gốc
#include "C:/PBL2/include/entities/FlightInstance.h"    // Đối tượng chuyến bay cụ thể
#include <string>

/*
    FlightManager: Lớp quản lý trung tâm cho mọi nghiệp vụ liên quan đến chuyến bay.
    - Chịu trách nhiệm nạp, lưu, tạo, tìm kiếm, và quản lý các đối tượng Flight và FlightInstance.
    - Đóng vai trò là "Người Gác Cổng", đảm bảo mọi dữ liệu đầu vào đều hợp lệ trước khi tạo ra các đối tượng.
 */
class FlightManager {
private:
    DynamicArray<Flight> allFlights;            // "Cơ sở dữ liệu" trong bộ nhớ cho các tuyến bay
    DynamicArray<FlightInstance> allInstances;  // "Cơ sở dữ liệu" trong bộ nhớ cho các chuyến bay cụ thể

    // --- Hàm trợ giúp nội bộ ---
    // Các hàm này được gọi bởi constructor để nạp dữ liệu từ file khi chương trình khởi động.
    void loadFlightsFromFile(const std::string& filePath);
    void loadInstancesFromFile(const std::string& filePath);

public:
    // --- Constructor & Destructor ---
    // Constructor sẽ tự động nạp toàn bộ dữ liệu từ các file được chỉ định.
    FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath);

    // Destructor mặc định là đủ, vì DynamicArray đã tự quản lý bộ nhớ của nó.
    ~FlightManager() = default;

    // --- Chức năng Tạo mới (Create) ---
    // Các hàm này là "Người Gác Cổng". Chúng kiểm tra dữ liệu trước khi tạo đối tượng.
    // Trả về true nếu tạo thành công, false nếu dữ liệu không hợp lệ.
    bool createNewFlight(const std::string& number,
                         const std::string& airline,
                         const std::string& departureIATA,
                         const std::string& arrivalIATA);

    bool createNewInstance(const std::string& flightId,
                           const std::string& departureIso,
                           const std::string& arrivalIso,
                           int totalEconomySeats,
                           int totalBusinessSeats,
                           double fareEconomy,
                           double fareBusiness);

    // --- Chức năng Đọc/Tìm kiếm (Read) ---
    // Trả về con trỏ để có thể:
    // 1. Báo hiệu "không tìm thấy" bằng nullptr.
    // 2. Cho phép người gọi sửa đổi trực tiếp đối tượng đã tìm thấy.
    Flight* findFlightById(const std::string& flightId);
    FlightInstance* findInstanceById(const std::string& instanceId);

    // Tìm tất cả các chuyến bay cụ thể thuộc về một tuyến bay gốc.
    DynamicArray<FlightInstance*> findInstancesByFlightId(const std::string& flightId);

    // --- Chức năng Lưu trữ (Persistence) ---
    // Lưu lại toàn bộ dữ liệu hiện tại trong bộ nhớ vào file.
    // Cần được gọi trước khi chương trình kết thúc để đảm bảo không mất dữ liệu.
    bool saveDataToFiles(const std::string& flightsFilePath, const std::string& instancesFilePath) const;
};

#endif // FLIGHT_MANAGER_H