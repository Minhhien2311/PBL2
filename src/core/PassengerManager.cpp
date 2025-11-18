#include "core/PassengerManager.h"
#include <fstream>
#include <iostream>

PassengerManager::PassengerManager(const std::string& passengersFilePath)
    : passengersFilePath_(passengersFilePath)
{
    
    loadPassengersFromFile(passengersFilePath_);
    buildPassengerIdTable();
    
    std::cout << "[PassengerManager] Initialized with " 
              << allPassengers_.size() << " passengers" << std::endl;
}

PassengerManager::~PassengerManager() {
    // Save before destruction
    saveAllData();
    
    // Cleanup
    for (Passenger* p : allPassengers_) {
        delete p;
    }
}

void PassengerManager::loadPassengersFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "[INFO] Passenger file not found, will create new" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            try {
                Passenger p = Passenger::fromRecordLine(line);
                allPassengers_.push_back(new Passenger(p));
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to parse passenger line: " << line << std::endl;
            }
        }
    }
    file.close();
    
    std::cout << "[INFO] Loaded " << allPassengers_.size() << " passengers" << std::endl;
}

void PassengerManager::buildPassengerIdTable() {
    for (Passenger* p : allPassengers_) {
        if (p) {
            passengerIdTable_.insert(p->getId(), p);
        }
    }
}

Passenger* PassengerManager::createOrUpdatePassenger(
    const std::string& id,
    const std::string& fullName,
    const std::string& dateOfBirth,
    const std::string& phoneNumber)
{
    // Check if passenger already exists
    Passenger* existing = findPassengerById(id);
    
    if (existing) {
        // Update existing passenger
        existing->setFullName(fullName);
        existing->setPhoneNumber(phoneNumber);
        
        std::cout << "[INFO] Updated passenger " << id << std::endl;
        return existing;
    }
    
    // Create new passenger
    Passenger* newPassenger = new Passenger( id, fullName, dateOfBirth, phoneNumber );
    
    allPassengers_.push_back(newPassenger);
    passengerIdTable_.insert(id, newPassenger);
    
    std::cout << "[INFO] Created new passenger " << id << std::endl;
    return newPassenger;
}

Passenger* PassengerManager::findPassengerById(const std::string& id) {
    Passenger** passengerPtr = passengerIdTable_.find(id);
    return (passengerPtr != nullptr) ? *passengerPtr : nullptr;
}

bool PassengerManager::updatePassenger(
    const std::string& id,
    const std::string& fullName,
    const std::string& phoneNumber)
{
    Passenger* p = findPassengerById(id);
    if (!p) return false;
    
    p->setFullName(fullName);
    p->setPhoneNumber(phoneNumber);
    
    return true;
}

bool PassengerManager::savePassengersToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot open " << filePath << " for writing" << std::endl;
        return false;
    }
    
    for (const Passenger* p : allPassengers_) {
        if (p) {
            file << p->toRecordLine() << "\n";
        }
    }
    
    file.close();
    std::cout << "[INFO] Saved " << allPassengers_.size() 
              << " passengers to " << filePath << std::endl;
    return true;
}

bool PassengerManager::saveAllData() {
    return savePassengersToFile(passengersFilePath_);
}