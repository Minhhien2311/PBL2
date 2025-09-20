#ifndef PASSENGER_H
#define PASSENGER_H

#include "Person.h"
#include <string>

class Passenger : public Person {
private:
    std::string passportNumber;
    std::string nationality;
    std::string frequentFlyerNumber;

public:
    Passenger(const std::string& id, const std::string& name, const std::string& dob, 
              const std::string& email, const std::string& phone, const std::string& addr,
              const std::string& passport, const std::string& nationality, 
              const std::string& ffNumber = "");
    
    // Getter methods
    std::string getPassportNumber() const;
    std::string getNationality() const;
    std::string getFrequentFlyerNumber() const;
    
    // Setter methods
    void setPassportNumber(const std::string& passport);
    void setNationality(const std::string& country);
    void setFrequentFlyerNumber(const std::string& ffNumber);
    
    // Passenger-specific methods
    int calculateAge() const;
    bool isEligibleForDiscount() const;
    
    void displayInfo() const override;
};

#endif // PASSENGER_H