// #ifndef BOOKING_MANAGER_H
// #define BOOKING_MANAGER_H

// // Khai báo tiền định (forward declaration)
// class FlightManager; 
// class FlightRule;

// #include "C:/PBL2/include/DSA/DynamicArray.h"
// #include "C:/PBL2/include/entities/Booking.h"
// #include <string>
// #include <chrono> 

// // BookingManager (Đã tái cấu trúc - Gộp Ticket)
// // Chịu trách nhiệm cho nghiệp vụ Bán vé và Hủy vé (bao gồm kiểm tra logic)
// class BookingManager {
// private:
//     DynamicArray<Booking*> allBookings;
//     FlightRule* currentRule; // Giữ nguyên

//     // <<< THÊM MỚI: Bảng băm >>>
//     HashTable<std::string, Booking*> bookingIdTable;

//     // --- Hàm trợ giúp nội bộ ---
//     void loadBookingsFromFile(const std::string& filePath);

//     // <<< THÊM MỚI: Hàm xây dựng bảng băm >>>
//     void buildBookingIdTable();

// public:
//     // Hàm khởi tạo 
//     BookingManager(const std::string& bookingsFilePath, FlightRule* rule);
    
//     // Hàm hủy 
//     ~BookingManager();

//     // NGHIỆP VỤ Bán vé 
//     Booking* createNewBooking( FlightManager& flightManager,
//                                const std::string& flightInstanceId,
//                                const std::string& passengerId,
//                                BookingClass bookingClass,
//                                double baseFare);

//     // NGHIỆP VỤ Hủy vé
//     bool cancelBooking(FlightManager& flightManager, const std::string& bookingId);

//     // --- Tìm kiếm  ---
//     Booking* findBookingById(const std::string& bookingId);
    
//     // --- Lấy dữ liệu  ---
//     const DynamicArray<Booking*>& getAllBookings() const;

//     // --- Lưu trữ  ---
//     bool saveDataToFiles(const std::string& bookingsFilePath) const; 
// };

// #endif
