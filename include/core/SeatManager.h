#ifndef SEATMANAGER_H
#define SEATMANAGER_H

#include <string>
#include "DSA/DynamicArray.h"
#include "entities/Seat.h"

enum class SeatClass {
    Economy,
    Business
};

class FlightInstance;

class SeatManager {
private:
    // === State Management ===
    std::string currentInstanceId_;
    DynamicArray<Seat*> activeSeatMap_;
    Seat* selectedSeat_;
    
    int seatRows_;
    int seatCols_;
    
    // === File Paths ===
    std::string seatStatusFilePath_;
    std::string seatConfigFilePath_;
    
    // === Helpers ===
    void clearCurrentMap();
    void loadConfiguration();
    std::string seatIdToString(int row, int col) const;
    SeatType determineSeatType(int row) const;
    
public:
    // Constructor - Initialize at app startup
    explicit SeatManager(const std::string& seatStatusPath = "data/seat_maps.txt",
                         const std::string& seatConfigPath = "data/seat_config.txt");
    
    ~SeatManager();
    
    // === Core Functions ===
    bool loadForFlight(const std::string& instanceId);
    bool loadSeatMapFor(FlightInstance* instance);
    
    // Seat operations
    bool selectSeat(const std::string& seatId);
    void cancelSelection();
    bool confirmSelection();
    bool bookSeat(const std::string& seatId);
    bool releaseSeat(const std::string& seatId);
    
    // Save/Load
    bool saveChanges();
    bool updateAndSaveChanges();
    
    // === Getters ===
    DynamicArray<Seat*>* getActiveSeatMap();
    const DynamicArray<Seat*>& getAllSeats() const;
    Seat* getSelectedSeat() const;
    const std::string& getCurrentInstanceId() const;
    int getSeatRows() const;
    int getSeatColumns() const;
    int getAvailableSeats(SeatClass seatClass) const;
};

#endif // SEATMANAGER_H
