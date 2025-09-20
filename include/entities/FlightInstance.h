// Lịch bay cụ thể theo ngày
#ifndef FLIGHTINSTANCE_H
#define FLIGHTINSTANCE_H

#include <string>
#include "Flight.h"

class FlightInstance {
private:
    std::string instanceId;
    std::string flightId;
    std::string flightDate;
    std::string scheduledDepartureTime;
    std::string scheduledArrivalTime;
    std::string actualDepartureTime;
    std::string actualArrivalTime;
    int availableSeats;
    std::string status;
    std::string aircraftRegistration;
    std::string currentGate;

public:
    FlightInstance(const std::string& id, const std::string& fId, const std::string& date,
                   const std::string& sDept, const std::string& sArr, int seats, 
                   const std::string& stat);
    
    // Getter methods
    std::string getInstanceId() const;
    std::string getFlightId() const;
    std::string getFlightDate() const;
    std::string getScheduledDepartureTime() const;
    std::string getScheduledArrivalTime() const;
    std::string getActualDepartureTime() const;
    std::string getActualArrivalTime() const;
    int getAvailableSeats() const;
    std::string getStatus() const;
    std::string getAircraftRegistration() const;
    std::string getCurrentGate() const;
    
    // Setter methods
    void setActualDepartureTime(const std::string& time);
    void setActualArrivalTime(const std::string& time);
    void setAvailableSeats(int seats);
    void setStatus(const std::string& newStatus);
    void setAircraftRegistration(const std::string& registration);
    void setCurrentGate(const std::string& gate);
    
    // Business methods
    bool isSeatAvailable() const;
    void updateSeatAvailability(int change);
    std::string getFlightDetails() const;
    bool isDeparted() const;
    bool isCompleted() const;
    
    static std::vector<FlightInstance> loadAllInstances();
    static FlightInstance findInstanceById(const std::string& id);
    static std::vector<FlightInstance> findInstancesByDate(const std::string& date);
};

#endif // FLIGHTINSTANCE_H