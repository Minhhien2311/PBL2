#include "C:/PBL2/include/entities/Passenger.h"
#include "C:/PBL2/include/utils/GenID.h"

Passenger::Passenger(const std::string& name,
                     const std::string& dob,
                     Gender gender,
                     const std::string& phone,
                     const std::string& passport,
                     const std::string& nation)
                    : id(IdGenerator::generatePassengerId()), fullName(name), dateOfBirth(dob), gender(gender), phoneNumber(phone), passportNumber(passport), nationality(nation) {}

const std::string& Passenger::getId() const { return id; }
const std::string& Passenger::getFullName() const { return fullName; }
const std::string& Passenger::getDateOfBirth() const { return dateOfBirth; }
Gender Passenger::getGender() const { return gender; }
const std::string& Passenger::getPhoneNumber() const { return phoneNumber; }
const std::string& Passenger::getPassportNumber() const { return passportNumber; }
const std::string& Passenger::getNationality() const { return nationality; }