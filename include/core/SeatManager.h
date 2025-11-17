#ifndef SEATMANAGER_H
#define SEATMANAGER_H

#include <string>
#include <vector>
#include "entities/Seat.h"

enum class SeatClass {
    Economy,
    Business
};

class Flight; // <-- Đã đổi tên

class SeatManager {
private:
    // Dữ liệu chính
    std::string currentFlightId_; // <-- Đã đổi tên
    std::vector<Seat*> activeSeatMap_;
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
    explicit SeatManager(const std::string& seatStatusPath = "C:/PBL2/data/seat_maps.txt",
                         const std::string& seatConfigPath = "C:/PBL2/data/seat_config.txt");
    
    ~SeatManager();
    
    // -- Chức năng chính --
    bool loadForFlight(const std::string& flightId); // <-- Đã đổi tên
    bool loadSeatMapFor(Flight* flight); // <-- Đã đổi tên

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
    std::vector<Seat*>* getActiveSeatMap();
    const std::vector<Seat*>& getAllSeats() const;
    Seat* getSelectedSeat() const;
    const std::string& getCurrentFlightId() const; // <-- Đã đổi tên
    int getSeatRows() const;
    int getSeatColumns() const;
    int getAvailableSeats(SeatClass seatClass) const;
    int getAvailableSeats() const;
};

#endif // SEATMANAGER_H