#include <iostream>
#include "C:/PBL2/include/entities/Passenger.h"

Passenger::Passenger(const std::string& id, const std::string& name, const std::string& dob, 
              const std::string& email, const std::string& phone, const std::string& addr,
              const std::string& passport, const std::string& nationality, 
              const std::string& ffNumber)
    : Person(id, name, dob, email, phone, addr),
      passportNumber(passport), nationality(nationality), frequentFlyerNumber(ffNumber) {}

// Getter methods
std::string Passenger::getPassportNumber() const { return passportNumber; }
std::string Passenger::getNationality() const { return nationality; }
std::string Passenger::getFrequentFlyerNumber() const { return frequentFlyerNumber; }

// Setter methods
void Passenger::setPassportNumber(const std::string& passport) { passportNumber = passport; }
void Passenger::setNationality(const std::string& country) { nationality = country; }
void Passenger::setFrequentFlyerNumber(const std::string& ffNumber) { frequentFlyerNumber = ffNumber; }