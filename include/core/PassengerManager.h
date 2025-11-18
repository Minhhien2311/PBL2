#ifndef PASSENGER_MANAGER_H
#define PASSENGER_MANAGER_H

#include <string>
#include <vector>
#include "entities/Passenger.h"
#include "DSA/HashTable.h"

class PassengerManager {
private:
    std::vector<Passenger*> allPassengers_;
    HashTable<std::string, Passenger*> passengerIdTable_;  // Key = CCCD
    std::string passengersFilePath_;
    
    void loadPassengersFromFile(const std::string& filePath);
    void buildPassengerIdTable();

public:
    explicit PassengerManager(const std::string& passengersFilePath = "C:/PBL2/data/passengers.txt");
    ~PassengerManager();
    
    // CRUD Operations
    Passenger* createOrUpdatePassenger(
        const std::string& id,           // CCCD
        const std::string& fullName,
        const std::string& dateOfBirth,
        const std::string& phoneNumber
    );
    
    Passenger* findPassengerById(const std::string& id);
    
    bool updatePassenger(
        const std::string& id,
        const std::string& fullName,
        const std::string& phoneNumber
    );
    
    // Persistence
    bool savePassengersToFile(const std::string& filePath) const;
    bool saveAllData();
    
    // Getters
    const std::vector<Passenger*>& getAllPassengers() const { 
        return allPassengers_; 
    }
};

#endif // PASSENGER_MANAGER_H