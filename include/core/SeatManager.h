#ifndef SEAT_MANAGER_H
#define SEAT_MANAGER_H

#include "DSA/HashTable.h"
#include "DSA/DynamicArray.h"
#include "entities/Seat.h"
#include "entities/FlightInstance.h"
#include <string>

/**
 * @brief Quản lý sơ đồ ghế cho các chuyến bay
 * 
 * Sử dụng lazy-loading và cache để tối ưu hiệu năng:
 * - Chỉ load sơ đồ ghế khi cần
 * - Cache dữ liệu để tránh đọc file nhiều lần
 * - Dirty tracking để chỉ save khi có thay đổi
 */
class SeatManager {
private:
    // Cache chứa dữ liệu thô từ file seat_maps.txt
    // Key: flightInstanceId, Value: chuỗi dữ liệu ghế
    HashTable<std::string, std::string>& seatDataCache;
    
    // Sơ đồ ghế đang được load vào bộ nhớ
    DynamicArray<Seat*>* activeSeatMap;
    
    // ID của FlightInstance hiện tại đang load
    std::string activeInstanceId;
    
    // Đánh dấu có thay đổi hay không
    bool isDirty;

    // Số cột ghế mặc định (A-F = 6 cột)
    static const int DEFAULT_SEAT_COLS = 6;

    // Hàm helper: Tạo sơ đồ ghế mới cho một FlightInstance
    DynamicArray<Seat*>* generateDefaultSeatMap(FlightInstance* instance);
    
    // Hàm helper: Parse chuỗi dữ liệu thành DynamicArray<Seat*>
    DynamicArray<Seat*>* parseSeatData(const std::string& data);
    
    // Hàm helper: Chuyển DynamicArray<Seat*> thành chuỗi để lưu
    std::string serializeSeatMap(DynamicArray<Seat*>* seatMap);

public:
    // Constructor nhận tham chiếu tới cache (được quản lý bởi FlightManager)
    explicit SeatManager(HashTable<std::string, std::string>& cache);
    
    // Destructor
    ~SeatManager();

    // --- Chức năng chính ---
    
    /**
     * @brief Load sơ đồ ghế cho một FlightInstance
     * 
     * Nếu có cache -> parse cache
     * Nếu không -> tạo sơ đồ mặc định và lưu vào cache
     * 
     * @param instance Con trỏ tới FlightInstance cần load
     * @return true nếu thành công, false nếu thất bại
     */
    bool loadSeatMapFor(FlightInstance* instance);
    
    /**
     * @brief Đặt ghế (đổi trạng thái từ Available -> Booked)
     * 
     * @param seatId ID ghế cần đặt (ví dụ: "1A")
     * @return true nếu đặt thành công, false nếu ghế không khả dụng
     */
    bool bookSeat(const std::string& seatId);
    
    /**
     * @brief Hủy đặt ghế (đổi trạng thái từ Booked -> Available)
     * 
     * @param seatId ID ghế cần hủy
     * @return true nếu hủy thành công
     */
    bool releaseSeat(const std::string& seatId);
    
    /**
     * @brief Lưu các thay đổi vào cache
     * 
     * Chỉ lưu nếu isDirty = true
     */
    void saveChanges();
    
    /**
     * @brief Giải phóng sơ đồ ghế hiện tại khỏi bộ nhớ
     * 
     * Tự động save nếu có thay đổi trước khi unload
     */
    void unload();

    // --- Getters ---
    
    /**
     * @brief Lấy sơ đồ ghế hiện tại
     * 
     * @return Con trỏ tới DynamicArray chứa các Seat*
     */
    DynamicArray<Seat*>* getActiveSeatMap() const;
    
    /**
     * @brief Kiểm tra có sơ đồ ghế nào đang được load không
     * 
     * @return true nếu có sơ đồ ghế đang được load
     */
    bool isLoaded() const;

    /**
     * @brief Lấy số cột ghế mặc định
     * 
     * @return Số cột ghế (thường là 6: A-F)
     */
    int getSeatColumns() const;
};

#endif // SEAT_MANAGER_H
