#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H

#include "DSA/DynamicArray.h"           // CTDL nền tảng để lưu trữ
#include "entities/Flight.h"            // Đối tượng tuyến bay gốc
#include "entities/FlightInstance.h"    // Đối tượng chuyến bay cụ thể
#include "DSA/HashTable.h"
#include <string>

class SeatManager;

class FlightManager {
private:
    // Lưu trữ một mảng các con trỏ thay vì đối tượng
    DynamicArray<Flight*> allFlights;         
    DynamicArray<FlightInstance*> allInstances;

    // Thêm bảng băm để tra cứu bằng ID
    HashTable<std::string, Flight*> flightIdTable;
    HashTable<std::string, FlightInstance*> instanceIdTable;
    
    // Lưu đường dẫn file để dùng trong saveAllData() và destructor
    std::string flightsFilePath_;
    std::string instancesFilePath_;

    // SeatManager để quản lý sơ đồ chỗ ngồi
    SeatManager* seatManager_;

    // Hàm xây dựng bảng băm
    void buildFlightIdTable();
    void buildInstanceIdTable();

    void loadFlightsFromFile(const std::string& filePath);
    void loadInstancesFromFile(const std::string& filePath);

public:
    FlightManager();
    FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath);
    ~FlightManager();

    // --- Các hàm tạo mới ---
    bool createNewFlight(const std::string& airline,
                         const std::string& departureIATA,
                         const std::string& arrivalIATA);

    // Tạo mới chuyến bay cụ thể dựa trên
    bool createNewInstance(const std::string& flightId,
                           const std::string& flightNumber,
                           const std::string& departureDate,  
                           const std::string& departureTime,  
                           const std::string& arrivalDate,    
                           const std::string& arrivalTime,    
                           int totalCapacity,
                           int fareEconomy,
                           int fareBusiness);
    
    // --- Các hàm Update và Delete ---
    bool updateFlight(const std::string& flightId, const std::string& newAirline, 
                      const std::string& newDeparture, const std::string& newDestination);
    bool deleteFlight(const std::string& flightId);
    bool updateInstance(const std::string& instanceId, const FlightInstance& updatedInstance);
    bool deleteInstance(const std::string& instanceId);

    // --- Các hàm tìm kiếm ---
    Flight* findFlightById(const std::string& flightId);
    FlightInstance* findInstanceById(const std::string& instanceId);
    DynamicArray<FlightInstance*> findInstancesByFlightId(const std::string& flightId);
    DynamicArray<Flight*> findFlightByRoute(const std::string& fromIATA, const std::string& toIATA);

    // --- Các hàm lưu ---
    bool saveFlightsToFiles(const std::string& flightsFilePath) const;
    bool saveInstancesToFiles(const std::string& instancesFilePath) const;
    // Lưu tất cả dữ liệu (flights, instances)
    bool saveAllData();

    // --- Các hàm Getters ---
    const DynamicArray<Flight*>& getAllFlights() const;
    const DynamicArray<FlightInstance*>& getAllInstances() const;
    SeatManager* getSeatManager() const;
};

#endif