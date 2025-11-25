#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

class FlightManager; 
class FlightRule;

#include <vector>
#include "entities/Booking.h"
#include "DSA/HashTable.h"
#include <string>
#include <chrono> 
#include "core/SeatManager.h"

// Chịu trách nhiệm cho nghiệp vụ Bán vé và Hủy vé (bao gồm kiểm tra logic)
class BookingManager {
private:
    std::vector<Booking*> allBookings;
    FlightRule* currentRule;

    HashTable<std::string, Booking*> bookingIdTable;
    HashTable<std::string, std::vector<Booking*>> passengerIdTable;
    
    std::string bookingsFilePath_; // Lưu đường dẫn file để dùng trong destructor

    // --- Hàm trợ giúp nội bộ ---
    void loadBookingsFromFile(const std::string& filePath);

    void buildBookingIdTable();
    void buildPassengerIdTable();

public:
    // Constructor
    BookingManager();
    BookingManager(const std::string& bookingsFilePath, FlightRule* rule);

    // Destructor
    ~BookingManager();

    // NGHIỆP VỤ ĐẶT VÉ 
    Booking* createNewBooking( FlightManager& flightManager,
                               const std::string& flightId, // <-- Đã đổi tên
                               const std::string& agentId,
                               const std::string& passengerId,
                               BookingClass bookingClass,
                               int baseFare,
                               SeatManager& seatManager);

    // NGHIỆP VỤ HỦY VÉ
    bool cancelBooking(FlightManager& flightManager, SeatManager& seatManager, const std::string& bookingId);
    
    // Check if booking can be cancelled (considering time constraints)
    bool canCancelBooking(const std::string& bookingId, 
                          FlightManager& flightManager) const;
    
    // Get cancellation deadline for a booking
    std::string getCancellationDeadline(const std::string& bookingId,
                                        FlightManager& flightManager) const;
    
    // NGHIỆP VỤ Cập nhật vé
    bool updateBooking(const std::string& bookingId, 
                       const std::string& newPassengerId,
                       BookingClass newClass, 
                       const std::string& newSeatId);
    
    // NGHIỆP VỤ ĐỔI VÉ
    bool changeBooking(FlightManager& flightManager,
                       SeatManager& seatManager,
                       const std::string& bookingId,
                       const std::string& newFlightId, // <-- Đã đổi tên
                       const std::string& newSeatNumber,
                       BookingClass newClass);

    bool canChangeBooking(const std::string& bookingId, FlightManager& flightManager) const;
    std::string getChangeDeadline(const std::string& bookingId, FlightManager& flightManager) const;

    // --- Tìm kiếm  ---
    Booking* findBookingById(const std::string& bookingId);
    std::vector<Booking*> findBookingsByPassengerId(const std::string& passengerId);
    
    // --- Lấy dữ liệu ---
    const std::vector<Booking*>& getAllBookings() const;

    // Lấy danh sách booking theo Agent ID
    std::vector<Booking*> getBookingsByAgentId(const std::string& agentId) const;
    
    // Lấy danh sách booking theo Flight ID (sẽ triển khai sau nếu cần) // <-- Đã đổi comment
    // std::vector<Booking*> getBookingsByFlightId(const std::string& flightId) const; // <-- Đã đổi tên hàm

    int applyPromotion(const std::string& promoCode, int baseFare);

    // --- Lưu trữ ---
    bool saveDataToFiles(const std::string& bookingsFilePath) const;
    
    // Lưu một booking riêng lẻ ngay lập tức vào file
    bool saveBookingToFile(Booking* booking);
};

#endif // BOOKING_MANAGER_H