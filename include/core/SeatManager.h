#ifndef SEAT_MANAGER_H
#define SEAT_MANAGER_H

#include "entities/Seat.h"
#include "DSA/DynamicArray.h"
#include "DSA/HashTable.h"
#include <string>

class SeatManager {
private:
    std::string activeFlightInstanceId;
    DynamicArray<DynamicArray<Seat>*> activeSeatMap;  // Use pointers to avoid copy issues
    bool isDirty;
    HashTable<std::string, std::string>& seatDataCache;
    
    // Configuration from seat_config.txt
    int totalRows;
    int totalCols;
    int businessClassEndRow;

    // Helper methods
    void loadConfiguration(const std::string& configFilePath);
    void createEmptySeatMap();
    void cleanupSeatMap(); // Helper to cleanup allocated seat rows

public:
    // Constructor takes a reference to the cache
    SeatManager(HashTable<std::string, std::string>& cache);
    
    // Destructor
    ~SeatManager();

    // Load seat map for a specific flight instance
    void loadSeatMapFor(const std::string& instanceId);

    // Seat operations
    bool bookSeat(const std::string& seatId);
    bool releaseSeat(const std::string& seatId);

    // Save changes to cache and file
    void saveChanges();

    // Unload current seat map from memory
    void unload();

    // Getters
    const DynamicArray<DynamicArray<Seat>*>& getActiveSeatMap() const;
    std::string getActiveFlightInstanceId() const;
    int getTotalRows() const;
    int getTotalCols() const;
    int getBusinessClassEndRow() const;
};

#endif // SEAT_MANAGER_H
