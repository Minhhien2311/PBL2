#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <vector>
#include "C:/PBL2/include/entities/Account.h"
#include "C:/PBL2/include/entities/Flight.h"
#include "../entities/Booking.h"
// Include other entity headers as needed

class DatabaseManager {
public:
    // User management
    static std::vector<User> loadAllUsers();
    static bool saveUser(const User& user);
    static bool updateUser(const User& user);
    static bool deleteUser(const std::string& userId);
    static User* findUserByUsername(const std::string& username);
    static User* findUserById(const std::string& userId);
    
    // Flight management
    static std::vector<Flight> loadAllFlights();
    static bool saveFlight(const Flight& flight);
    static bool updateFlight(const Flight& flight);
    static bool deleteFlight(const std::string& flightId);
    
    // Booking management
    static std::vector<Booking> loadAllBookings();
    static bool saveBooking(const Booking& booking);
    static bool updateBooking(const Booking& booking);
    static bool deleteBooking(const std::string& bookingId);
    
    // Generic methods
    static std::vector<std::vector<std::string>> readData(const std::string& filename);
    static bool writeData(const std::string& filename, const std::vector<std::vector<std::string>>& data);
    static bool appendData(const std::string& filename, const std::vector<std::string>& row);
    static int getNextId(const std::string& filename);
    static bool backupDatabase();
    static bool restoreDatabase(const std::string& backupPath);
};

#endif // DATABASEMANAGER_H