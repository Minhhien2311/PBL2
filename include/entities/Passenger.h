#ifndef PASSENGER_H
#define PASSENGER_H

#include "Person.h"
#include <string>

class Passenger : public Person {
private:
    std::string passportNumber;
    std::string nationality;

public:
    Passenger(const std::string& id,
              const std::string& name,
              const std::string& dob,
              Gender gender,
              const std::string& phone,
              const std::string& passport,
              const std::string& nation);
    
    // Getter
    const std::string& getPassportNumber() const;
    const std::string& getNationality() const;
        
    // Override displayInfo để in thêm field riêng
    void displayInfo() const override;
};

#endif // PASSENGER_H