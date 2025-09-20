// Thông tin đặt chỗ (PNR)
#ifndef BOOKING_H
#define BOOKING_H

#include <string>
#include <vector>
#include "Passenger.h"

class Booking {
private:
    std::string bookingId;
    std::string pnr;
    std::string agentId;
    std::string instanceId;
    std::string bookingDate;
    double totalAmount;
    std::string status;
    std::string promotionId;
    std::vector<Passenger> passengers;

public:
    Booking(const std::string& id, const std::string& pnr, const std::string& agentId,
            const std::string& instanceId, const std::string& date, double amount,
            const std::string& status, const std::string& promotionId);
    
    // Getter methods
    std::string getBookingId() const;
    std::string getPNR() const;
    std::string getAgentId() const;
    std::string getInstanceId() const;
    std::string getBookingDate() const;
    double getTotalAmount() const;
    std::string getStatus() const;
    std::string getPromotionId() const;
    std::vector<Passenger> getPassengers() const;
    
    // Setter methods
    void setStatus(const std::string& newStatus);
    void setPromotionId(const std::string& promoId);
    void setTotalAmount(double amount);
    
    // Passenger management
    void addPassenger(const Passenger& passenger);
    void removePassenger(const std::string& passengerId);
    int getPassengerCount() const;
    
    // Business methods
    bool canBeCancelled() const;
    bool canBeChanged() const;
    double calculateCancellationFee() const;
    double calculateChangeFee() const;
    
    static std::vector<Booking> loadAllBookings();
    static Booking findBookingByPNR(const std::string& pnr);
    static std::vector<Booking> findBookingsByAgent(const std::string& agentId);
    static std::vector<Booking> findBookingsByDate(const std::string& date);
};

#endif // BOOKING_H