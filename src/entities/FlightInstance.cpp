#include "C:/PBL2/include/entities/FlightInstance.h"
#include <iostream>

// Constructor
FlightInstance::FlightInstance(const std::string& instanceId,
                               const std::string& flightId,
                               const std::string& departureIso,
                               const std::string& arrivalIso,
                               int capacity,
                               int seatsAvailable,
                               int economyTotal,
                               int economyAvailable,
                               int businessTotal,
                               int businessAvailable,
                               double fareEconomy,
                               double fareBusiness)
    : instanceId(instanceId),
      flightId(flightId),
      departureIso(departureIso),
      arrivalIso(arrivalIso),
      capacity(capacity),
      seatsAvailable(seatsAvailable),
      economyTotal(economyTotal),
      economyAvailable(economyAvailable),
      businessTotal(businessTotal),
      businessAvailable(businessAvailable),
      fareEconomy(fareEconomy),
      fareBusiness(fareBusiness)
{}

// GETTERS 
const std::string& FlightInstance::getInstanceId() const      { return instanceId; }
const std::string& FlightInstance::getFlightId() const        { return flightId; }
const std::string& FlightInstance::getDepartureIso() const    { return departureIso; }
const std::string& FlightInstance::getArrivalIso() const      { return arrivalIso; }

int  FlightInstance::getCapacity() const                      { return capacity; }
int  FlightInstance::getSeatsAvailable() const                { return seatsAvailable; }

int  FlightInstance::getEconomyTotal() const                  { return economyTotal; }
int  FlightInstance::getEconomyAvail() const                  { return economyAvailable; }
int  FlightInstance::getBusinessTotal() const                 { return businessTotal; }
int  FlightInstance::getBusinessAvail() const                 { return businessAvailable; }

double FlightInstance::getFareEconomy() const                 { return fareEconomy; }
double FlightInstance::getFareBusiness() const                { return fareBusiness; }

// SETTERS 
void FlightInstance::setSeatsAvailable(int remaining) {
    seatsAvailable = remaining;
}

void FlightInstance::setEconomyTotal(int ecoTotal) {
    economyTotal = ecoTotal;
}

void FlightInstance::setEconomyAvail(int ecoAvail) {
    economyAvailable = ecoAvail;
}

void FlightInstance::setBusinessTotal(int busTotal) {
    businessTotal = busTotal;
}

void FlightInstance::setBusinessAvail(int busAvail) {
    businessAvailable = busAvail;
}

void FlightInstance::setFareEconomy(double fare) {
    fareEconomy = fare;
}

void FlightInstance::setFareBusiness(double fare) {
    fareBusiness = fare;
}
