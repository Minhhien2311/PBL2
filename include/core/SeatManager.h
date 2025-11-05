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
    // Dữ liệu chính
    std::string currentInstanceId_;
    DynamicArray<Seat*> activeSeatMap_;
    Seat* selectedSeat_;
    
    int seatRows_;
    int seatCols_;
    
    // Đường dẫn file
    std::string seatStatusFilePath_;
    std::string seatConfigFilePath_;
    
    // Hàm trợ giúp nội bộ
    void clearCurrentMap();
    void loadConfiguration();
    std::string seatIdToString(int row, int col) const;
    SeatType determineSeatType(int row) const;
    
public:
    // Constructor và Destructor
    explicit SeatManager(const std::string& seatStatusPath = "data/seat_maps.txt",
                         const std::string& seatConfigPath = "data/seat_config.txt");
    
    ~SeatManager();
    
    // -- Chức năng chính --
    bool loadForFlight(const std::string& instanceId);
    bool loadSeatMapFor(FlightInstance* instance);
    
    // --- Chức năng thao tác ghế ---
    bool selectSeat(const std::string& seatId);
    void cancelSelection();
    bool confirmSelection();
    bool bookSeat(const std::string& seatId);
    bool releaseSeat(const std::string& seatId);
    
    // --- Chức năng lưu ---
    bool saveChanges();
    bool updateAndSaveChanges();
    
    // --- Getters ---
    DynamicArray<Seat*>* getActiveSeatMap();
    const DynamicArray<Seat*>& getAllSeats() const;
    Seat* getSelectedSeat() const;
    const std::string& getCurrentInstanceId() const;
    int getSeatRows() const;
    int getSeatColumns() const;
    int getAvailableSeats(SeatClass seatClass) const;
};

#endif
