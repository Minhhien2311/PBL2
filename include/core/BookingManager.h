#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

// Khai báo tiền định (forward declaration)
class FlightManager; 
class FlightRule;

#include "DSA/DynamicArray.h"
#include "entities/Booking.h"
#include "DSA/HashTable.h"
#include <string>
#include <chrono> 

// Chịu trách nhiệm cho nghiệp vụ Bán vé và Hủy vé (bao gồm kiểm tra logic)
class BookingManager {
private:
    DynamicArray<Booking*> allBookings;
    FlightRule* currentRule; // Giữ nguyên

    HashTable<std::string, Booking*> bookingIdTable;

    // --- Hàm trợ giúp nội bộ ---
    void loadBookingsFromFile(const std::string& filePath);

    void buildBookingIdTable();

public:
    BookingManager();
    // Hàm khởi tạo 
    BookingManager(const std::string& bookingsFilePath, FlightRule* rule);
    
    // Hàm hủy 
    ~BookingManager();

    // NGHIỆP VỤ Bán vé 
    Booking* createNewBooking( FlightManager& flightManager,
                               const std::string& flightInstanceId,
                               const std::string& passengerId,
                               BookingClass bookingClass,
                               int baseFare);

    // NGHIỆP VỤ Hủy vé
    bool cancelBooking(FlightManager& flightManager, const std::string& bookingId);

    // --- Tìm kiếm  ---
    Booking* findBookingById(const std::string& bookingId);
    
    // --- Lấy dữ liệu  ---
    const DynamicArray<Booking*>& getAllBookings() const;

    // --- Lưu trữ  ---
    bool saveDataToFiles(const std::string& bookingsFilePath) const; 
};

#endif
