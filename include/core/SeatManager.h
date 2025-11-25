#pragma once

#include <string>
#include <vector>
#include "entities/Seat.h"

enum class SeatClass { Economy, Business };
class Flight;

class SeatManager {
public:
    // --- Constructor & Destructor ---
    explicit SeatManager(const std::string& seatStatusPath = "C:/PBL2/data/seat_maps.txt",
                         const std::string& seatConfigPath = "C:/PBL2/data/seat_config.txt");
    ~SeatManager();

    // --- Initialization (Khởi tạo Map) ---
    // Load map cho một chuyến bay cụ thể để chuẩn bị thao tác
    bool loadForFlight(const std::string& flightId);
    bool loadSeatMapFor(Flight* flight);

    // --- Seat Operations (Thao tác ghế) ---
    bool selectSeat(const std::string& seatId);  // Chọn tạm thời (UI highlight)
    void cancelSelection();                      // Hủy chọn
    bool confirmSelection();                     // Xác nhận chọn
    
    bool bookSeat(const std::string& seatId);    // Đặt cứng (lưu vào data)
    bool releaseSeat(const std::string& seatId); // Trả ghế (hủy vé)

    // --- Persistence (Lưu trữ) ---
    bool saveChanges();          // Lưu trạng thái hiện tại
    bool updateAndSaveChanges(); // Cập nhật sync và lưu

    // --- Getters & Status ---
    std::vector<Seat*>* getActiveSeatMap();
    const std::vector<Seat*>& getAllSeats() const;
    Seat* getSelectedSeat() const;
    
    const std::string& getCurrentFlightId() const;
    int getSeatRows() const;
    int getSeatColumns() const;
    
    // Thống kê ghế trống
    int getAvailableSeats(SeatClass seatClass) const;
    int getAvailableSeats() const;

private:
    // --- Data State ---
    std::string currentFlightId_;
    std::vector<Seat*> activeSeatMap_;
    Seat* selectedSeat_; // Ghế đang được highlight
    
    int seatRows_;
    int seatCols_;
    
    // --- File Paths ---
    std::string seatStatusFilePath_;
    std::string seatConfigFilePath_;
    
    // --- Internal Helpers ---
    void clearCurrentMap();
    void loadConfiguration(); // Load số hàng/cột
    std::string seatIdToString(int row, int col) const;
    SeatType determineSeatType(int row) const;
};