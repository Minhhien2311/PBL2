#ifndef SEAT_MANAGER_H
#define SEAT_MANAGER_H

#include "entities/seat.h"
#include "DSA/DynamicArray.h"

class FlightInstance;

class SeatManager {
private:
    std::string flightInstanceId;
    DynamicArray<DynamicArray<Seat>> seatMap; 
    const std::string seatStatusFilePath = "data/seat_status.txt"; 
    static const int DEFAULT_ROWS = 35;

    // THÊM MỚI: Con trỏ lưu lại ghế đang được chọn
    Seat* selectedSeat; 

public:
    SeatManager();
    bool loadForFlight(const FlightInstance& flight);
    void release();

    // CÁC HÀM CŨ ĐỂ CẬP NHẬT TRỰC TIẾP
    bool updateAndSaveChanges(const std::string& seatId, SeatStatus newStatus);
    
    // === CÁC HÀM MỚI CHO QUY TRÌNH CHỌN GHẾ ===

    /**
     * @brief Chọn một ghế dựa trên ID người dùng nhập vào.
     * @param seatId ID của ghế (ví dụ: "A5").
     * @return true nếu chọn thành công (ghế tồn tại và còn trống), ngược lại false.
     */
    bool selectSeat(const std::string& seatId);

    /**
     * @brief Hủy lựa chọn ghế hiện tại.
     */
    void cancelSelection();

    /**
     * @brief Xác nhận ghế đã chọn, đổi trạng thái và lưu vào file.
     * @return true nếu xác nhận thành công, false nếu chưa có ghế nào được chọn.
     */
    bool confirmSelection();

    /**
     * @brief Getter để các lớp khác (như BookingManager) lấy thông tin ghế đã chọn.
     * @return Con trỏ tới đối tượng Seat đã chọn, hoặc nullptr nếu chưa có.
     */
    const Seat* getSelectedSeat() const;
    

    // Các getter khác không đổi
    const DynamicArray<DynamicArray<Seat>>& getSeatMap() const; 
    const std::string& getCurrentFlightId() const;

private:
    void initializeSeatMap(int rows, int cols);
    bool loadBookedSeatsFromFile();
    bool saveAllFlightsStatusToFile();
};

#endif // SEAT_MANAGER_H