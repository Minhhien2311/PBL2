#pragma once

#include <string>
#include <vector>
#include "entities/Passenger.h"
#include "DSA/HashTable.h"

class PassengerManager {
public:
    // --- Constructor & Destructor ---
    explicit PassengerManager(const std::string& passengersFilePath = "C:/PBL2/data/passengers.txt");
    ~PassengerManager();

    // --- Core Operations (Thêm/Sửa) ---
    // Tạo mới hoặc cập nhật nếu đã tồn tại
    Passenger* createOrUpdatePassenger(const std::string& id, const std::string& fullName,
                                       const std::string& dateOfBirth, const std::string& phoneNumber);
    
    bool updatePassenger(const std::string& id, const std::string& fullName, const std::string& phoneNumber);

    // --- Tìm kiếm & Tra cứu ---
    Passenger* findPassengerById(const std::string& id); // id = CCCD
    const std::vector<Passenger*>& getAllPassengers() const;

    // --- Lưu trữ ---
    bool savePassengersToFile(const std::string& filePath) const;
    bool saveAllData();

private:
    // --- Dữ liệu lưu trữ ---
    std::vector<Passenger*> allPassengers_;
    HashTable<std::string, Passenger*> passengerIdTable_; // Key: CCCD
    std::string passengersFilePath_;
    
    // --- Các hàm trợ giúp nội bộ ---
    void loadPassengersFromFile(const std::string& filePath);
    void buildPassengerIdTable();
};