#ifndef FLIGHT_MANAGER_H
#define FLIGHT_MANAGER_H

#include "C:/PBL2/include/DSA/DynamicArray.h"           // CTDL nền tảng để lưu trữ
#include "C:/PBL2/include/entities/Flight.h"            // Đối tượng tuyến bay gốc
#include "C:/PBL2/include/entities/FlightInstance.h"    // Đối tượng chuyến bay cụ thể
#include <string>

class FlightManager {
private:
    // <<< THAY ĐỔI: Lưu trữ một mảng các con trỏ thay vì đối tượng
    DynamicArray<Flight*> allFlights;         
    DynamicArray<FlightInstance*> allInstances; 

    void loadFlightsFromFile(const std::string& filePath);
    void loadInstancesFromFile(const std::string& filePath);

public:
    FlightManager(const std::string& flightsFilePath, const std::string& instancesFilePath);
    ~FlightManager() = default;

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

    Flight* findFlightById(const std::string& flightId);
    FlightInstance* findInstanceById(const std::string& instanceId);
    
    DynamicArray<FlightInstance*> findInstancesByFlightId(const std::string& flightId);

    bool saveFlightsToFiles(const std::string& flightsFilePath) const;
    bool saveInstancesToFiles(const std::string& instancesFilePath) const;
};

#endif // FLIGHT_MANAGER_H
