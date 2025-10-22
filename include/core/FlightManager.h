#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H

#include "C:/PBL2/include/DSA/DynamicArray.h"           // CTDL nền tảng để lưu trữ
#include "C:/PBL2/include/DSA/HashTable.h"
#include "C:/PBL2/include/entities/Flight.h"            // Đối tượng tuyến bay gốc
#include "C:/PBL2/include/entities/FlightInstance.h"    // Đối tượng chuyến bay cụ thể
#include <string>

/*
FlightManager quản lý toàn bộ logic liên quan đến Tuyến bay (Flight) và Chuyến bay (FlightInstance).
1. Tải danh sách tuyến bay và chuyến bay từ file.
2. Cung cấp hàm tạo mới, tìm kiếm, và xóa cho cả Flight và FlightInstance.
3. Sử dụng HashTable để tối ưu hóa tốc độ tìm kiếm theo ID.
4. Lưu lại các thay đổi ra file.
 */
class FlightManager {
private:
    // Mảng động lưu trữ con trỏ tới các tuyến bay gốc (vd: VN123, VJ234)
    DynamicArray<Flight*> allFlights;
    // Mảng động lưu trữ con trỏ tới các chuyến bay cụ thể (vd: VN123 ngày 20/10)
    DynamicArray<FlightInstance*> allInstances; 

    // --- Các bảng băm để tra cứu nhanh ---

    /*
    Bảng băm ánh xạ Flight ID (vd: "FL-001") tới VỊ TRÍ (index)
    của nó trong mảng 'allFlights'.
    Giúp tìm kiếm O(1) thay vì O(N).
     */
    HashTable<std::string, size_t> flightIdMap;

    /*
    Bảng băm ánh xạ Instance ID (vd: "FI-10001") tới VỊ TRÍ (index)
    của nó trong mảng 'allInstances'.
     */
    HashTable<std::string, size_t> instanceIdMap;

    // --- Các hàm trợ giúp nội bộ ---

    /*
    Tải danh sách tuyến bay (Flight) từ file.
    Đồng thời xây dựng 'flightIdMap'.
     */
    void loadFlightsFromFile(const std::string& filePath);

    /*
    Tải danh sách chuyến bay (FlightInstance) từ file.
    Đồng thời xây dựng 'instanceIdMap'.
     */
    void loadInstancesFromFile(const std::string& filePath);

public:
    /*
    Hàm khởi tạo, tự động gọi các hàm load...FromFile.
     */
    FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath);
    
    /*
    Hàm hủy mặc định.
    (DynamicArray<T*> sẽ tự động 'delete' các con trỏ bên trong).
     */
    ~FlightManager() = default;

    // --- Thêm tuyến và chuyến bay ---

    /*
    Tạo một tuyến bay (Flight) MỚI.
    true nếu tạo thành công, false nếu dữ liệu rỗng hoặc 'number' đã tồn tại.
     */
    bool createNewFlight(const std::string& number,
                         const std::string& airline,
                         const std::string& departureIATA,
                         const std::string& arrivalIATA);

    /*
    Tạo một chuyến bay (FlightInstance) MỚI dựa trên một tuyến bay (Flight) đã có.
    true nếu tạo thành công, false nếu 'flightId' không tồn tại hoặc dữ liệu không hợp lệ.
     */
    bool createNewInstance(const std::string& flightId,
                           const std::string& departureIso,
                           const std::string& arrivalIso,
                           int totalEconomySeats,
                           int totalBusinessSeats,
                           double fareEconomy,
                           double fareBusiness);

    // --- Tìm kiếm ---

    /*
    Tìm tuyến bay (Flight) bằng ID, sử dụng HashTable.
    flightId ID cần tìm (vd: "FL-001").
    Con trỏ tới Flight nếu tìm thấy, ngược lại là nullptr.
     */
    Flight* findFlightById(const std::string& flightId);

    /*
    Tìm chuyến bay (FlightInstance) bằng ID, sử dụng HashTable.
    instanceId ID cần tìm (vd: "FI-10001").
    Con trỏ tới FlightInstance nếu tìm thấy, ngược lại là nullptr.
     */
    FlightInstance* findInstanceById(const std::string& instanceId);
    
    /*
    Tìm tất cả các FlightInstance thuộc về một Flight gốc.
    (Hàm này phải duyệt toàn bộ mảng 'allInstances' - O(N)).
    Nhập vào flightId ID của tuyến bay gốc (vd: "FL-001").
    results là Một mảng động (truyền vào qua tham chiếu) để chứa kết quả.
    Mảng này sẽ bị xóa sạch trước khi thêm kết quả mới.
     */
    void findInstancesByFlightId(const std::string& flightId, DynamicArray<FlightInstance*>& results) const;

    // --- Các hàm xóa ---

    /*
    Xóa một tuyến bay (Flight) bằng ID.
    Sẽ tự động xóa khỏi mảng 'allFlights' và 'flightIdMap', đồng thời cập nhật lại index trong 'flightIdMap' cho các phần tử bị dịch chuyển.
    Nhập vào flightId ID cần xóa.
    true nếu xóa thành công.
     */
    bool removeFlightById(const std::string& flightId);

    /*
    Xóa một chuyến bay (FlightInstance) bằng ID.
    Tương tự 'removeFlightById', sẽ cập nhật lại 'instanceIdMap'.
    Nhập vào instanceId ID cần xóa.
    true nếu xóa thành công.
     */
    bool removeInstanceById(const std::string& instanceId);

    // --- Các hàm lưu ---
    bool saveFlightsToFiles(const std::string& flightsFilePath) const;
    bool saveInstancesToFiles(const std::string& instancesFilePath) const;

    // --- Getters (Thêm vào để hỗ trợ giao diện Admin) ---
    DynamicArray<Flight*> getAllFlights() const;
    DynamicArray<FlightInstance*> getAllInstances() const;
};

#endif // FLIGHT_MANAGER_H