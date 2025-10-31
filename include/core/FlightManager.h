#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H

#include "DSA/DynamicArray.h"           // CTDL nền tảng để lưu trữ
#include "entities/Flight.h"            // Đối tượng tuyến bay gốc
#include "entities/FlightInstance.h"    // Đối tượng chuyến bay cụ thể
#include "DSA/HashTable.h"
#include <string>

class FlightManager {
private:
    // Lưu trữ một mảng các con trỏ thay vì đối tượng
    DynamicArray<Flight*> allFlights;         
    DynamicArray<FlightInstance*> allInstances;

    // Thêm bảng băm để tra cứu bằng ID
    HashTable<std::string, Flight*> flightIdTable;
    HashTable<std::string, FlightInstance*> instanceIdTable;

    // Hàm xây dựng bảng băm
    void buildFlightIdTable();
    void buildInstanceIdTable();

    void loadFlightsFromFile(const std::string& filePath);
    void loadInstancesFromFile(const std::string& filePath);

public:
    FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath);
    ~FlightManager(); // xóa default, viết lại hàm hủy

    // createNewFlight 
    bool createNewFlight(const std::string& number,
                         const std::string& airline,
                         const std::string& departureIATA,
                         const std::string& arrivalIATA);

    // Đã đổi tham số từ Iso sang Date/Time
    bool createNewInstance(const std::string& flightId,
                           const std::string& departureDate,  // Thay đổi
                           const std::string& departureTime,  // Thay đổi
                           const std::string& arrivalDate,    // Thay đổi
                           const std::string& arrivalTime,    // Thay đổi
                           int totalEconomySeats,
                           int totalBusinessSeats,
                           int fareEconomy,
                           int fareBusiness);

    // Các hàm tìm kiếm 
    Flight* findFlightById(const std::string& flightId);
    FlightInstance* findInstanceById(const std::string& instanceId);
    DynamicArray<FlightInstance*> findInstancesByFlightId(const std::string& flightId);

    // Các hàm lưu 
    bool saveFlightsToFiles(const std::string& flightsFilePath) const;
    bool saveInstancesToFiles(const std::string& instancesFilePath) const;

    // Các hàm Getters 
    const DynamicArray<Flight*>& getAllFlights() const;       
    const DynamicArray<FlightInstance*>& getAllInstances() const;
};

#endif // FLIGHT_MANAGER_H