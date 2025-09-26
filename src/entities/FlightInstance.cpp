#include "C:/PBL2/include/entities/FlightInstance.h"
#include <iostream>

// Constructor
FlightInstance::FlightInstance(const std::string& instanceId,
                               const std::string& flightId,
                               const std::string& departureIso,
                               const std::string& arrivalIso,
                               int capacity,
                               int seatsAvailable,
                               double fareEconomy, 
                               double fareBusiness)
    : instanceId(instanceId),
      flightId(flightId),
      departureIso(departureIso),
      arrivalIso(arrivalIso),
      capacity(capacity),
      seatsAvailable(seatsAvailable),
      fareEconomy(fareEconomy), 
      fareBusiness(fareBusiness) {}

// Getters
const std::string& FlightInstance::getInstanceId() const    { return instanceId; }
const std::string& FlightInstance::getFlightId() const      { return flightId; }
const std::string& FlightInstance::getDepartureIso() const  { return departureIso; }
const std::string& FlightInstance::getArrivalIso() const    { return arrivalIso; }
int  FlightInstance::getCapacity() const                    { return capacity; }
int  FlightInstance::getSeatsAvailable() const              { return seatsAvailable; }
double FlightInstance::getFareEconomy() const { return fareEconomy; }
double FlightInstance::getFareBusiness() const { return fareBusiness; }

// Updater
void FlightInstance::setSeatsAvailable(int remaining) {
    seatsAvailable = remaining;            // nếu muốn an toàn hơn có thể clamp [0, capacity]
    // if (remaining < 0) seatsAvailable = 0;
    // else if (remaining > capacity) seatsAvailable = capacity;
    // else seatsAvailable = remaining;
}
void FlightInstance::setFareEconomy(double fare)
{
    fareEconomy = fare;
}
void FlightInstance::setFareBusiness(double fare)
{
    fareBusiness = fare;
}

// Helpers
bool FlightInstance::hasAvailableSeats() const {
    return seatsAvailable > 0;
}

void FlightInstance::displayInfo() const {
    // Để triển sau
}
