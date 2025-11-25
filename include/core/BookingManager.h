#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "entities/Booking.h"
#include "DSA/HashTable.h"
#include "core/SeatManager.h"

class FlightManager; 
class FlightRule;

class BookingManager {
public:
    // --- Constructor & Destructor ---
    BookingManager();
    BookingManager(const std::string& bookingsFilePath, FlightRule* rule);
    ~BookingManager();

    // --- Booking Operations (Nghiệp vụ Vé) ---
    // Tạo vé mới
    Booking* createNewBooking(FlightManager& flightManager, const std::string& flightId,
                              const std::string& agentId, const std::string& passengerId,
                              BookingClass bookingClass, int baseFare, SeatManager& seatManager);

    // Hủy vé
    bool cancelBooking(FlightManager& flightManager, SeatManager& seatManager, const std::string& bookingId);
    
    // Đổi vé (sang chuyến khác/ghế khác)
    bool changeBooking(FlightManager& flightManager, SeatManager& seatManager, const std::string& bookingId,
                       const std::string& newFlightId, const std::string& newSeatNumber, BookingClass newClass);

    // Cập nhật thông tin vé (Passenger, Class, SeatId - không đổi chuyến)
    bool updateBooking(const std::string& bookingId, const std::string& newPassengerId,
                       BookingClass newClass, const std::string& newSeatId);

    // --- Business Logic Validation (Kiểm tra quy định) ---
    bool canCancelBooking(const std::string& bookingId, FlightManager& flightManager) const;
    std::string getCancellationDeadline(const std::string& bookingId, FlightManager& flightManager) const;
    
    bool canChangeBooking(const std::string& bookingId, FlightManager& flightManager) const;
    std::string getChangeDeadline(const std::string& bookingId, FlightManager& flightManager) const;

    // Áp dụng khuyến mãi
    int applyPromotion(const std::string& promoCode, int baseFare);

    // --- Search & Lookup (Tìm kiếm) ---
    Booking* findBookingById(const std::string& bookingId);
    std::vector<Booking*> findBookingsByPassengerId(const std::string& passengerId);
    std::vector<Booking*> getBookingsByAgentId(const std::string& agentId) const;
    const std::vector<Booking*>& getAllBookings() const;

    // --- Persistence (Lưu trữ) ---
    bool saveDataToFiles(const std::string& bookingsFilePath) const;
    bool saveBookingToFile(Booking* booking);

private:
    // --- Data Members ---
    std::vector<Booking*> allBookings;
    FlightRule* currentRule;
    std::string bookingsFilePath_;

    // --- Indexing Tables ---
    HashTable<std::string, Booking*> bookingIdTable;
    HashTable<std::string, std::vector<Booking*>> passengerIdTable;

    // --- Internal Helpers ---
    void loadBookingsFromFile(const std::string& filePath);
    void buildBookingIdTable();
    void buildPassengerIdTable();
};