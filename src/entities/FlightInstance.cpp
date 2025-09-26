#include "C:/PBL2/include/entities/FlightInstance.h"
#include <iostream>

// Constructor
FlightInstance::FlightInstance(const std::string& instanceId,
                               const std::string& flightId,
                               const std::string& departureIso,
                               const std::string& arrivalIso,
                               int capacity,
                               int seatsAvailable)
    : instanceId(instanceId),
      flightId(flightId),
      departureIso(departureIso),
      arrivalIso(arrivalIso),
      capacity(capacity),
      seatsAvailable(seatsAvailable) {}

// Getters
const std::string& FlightInstance::getInstanceId() const    { return instanceId; }
const std::string& FlightInstance::getFlightId() const      { return flightId; }
const std::string& FlightInstance::getDepartureIso() const  { return departureIso; }
const std::string& FlightInstance::getArrivalIso() const    { return arrivalIso; }
int  FlightInstance::getCapacity() const                    { return capacity; }
int  FlightInstance::getSeatsAvailable() const              { return seatsAvailable; }

// Updater
void FlightInstance::setSeatsAvailable(int remaining) {
    seatsAvailable = remaining;            // nếu muốn an toàn hơn có thể clamp [0, capacity]
    // if (remaining < 0) seatsAvailable = 0;
    // else if (remaining > capacity) seatsAvailable = capacity;
    // else seatsAvailable = remaining;
}

// Helpers
bool FlightInstance::hasAvailableSeats() const {
    return seatsAvailable > 0;
}

void FlightInstance::displayInfo() const {
    // Để triển sau
}
