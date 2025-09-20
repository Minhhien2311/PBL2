// Thông tin ghế ngồi
#ifndef SEAT_H
#define SEAT_H

#include <string>
#include <vector>

class Seat {
private:
    std::string seatId;
    std::string instanceId;
    std::string seatNumber;
    std::string seatClass;
    bool isAvailable;
    double extraFee;

public:
    Seat(const std::string& id, const std::string& instanceId, const std::string& number,
         const std::string& sClass, bool available, double fee);
    
    // Getter methods
    std::string getSeatId() const;
    std::string getInstanceId() const;
    std::string getSeatNumber() const;
    std::string getSeatClass() const;
    bool getIsAvailable() const;
    double getExtraFee() const;
    
    // Setter methods
    void setIsAvailable(bool available);
    void setExtraFee(double fee);
    
    // Business methods
    double calculateTotalPrice(double basePrice) const;
    bool isBusinessClass() const;
    bool isEconomyClass() const;
    
    static std::vector<Seat> loadSeatsForInstance(const std::string& instanceId);
    static Seat findSeatById(const std::string& seatId);
    static bool reserveSeat(const std::string& seatId);
    static bool releaseSeat(const std::string& seatId);
};

#endif // SEAT_H