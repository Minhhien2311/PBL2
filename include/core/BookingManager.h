#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

class FlightManager; 
#include "C:/PBL2/include/DSA/DynamicArray.h"
#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/entities/Ticket.h"
#include <string>

class BookingManager {
private:
    // Lưu trữ một mảng các con trỏ thay vì đối tượng
    DynamicArray<Booking*> allBookings;
    DynamicArray<Ticket*> allTickets;

    

    void loadBookingsFromFile(const std::string& filePath);
    void loadTicketsFromFile(const std::string& filePath);

public:
    BookingManager(const std::string& bookingsFilePath, const std::string& ticketsFilePath);
    ~BookingManager() = default;

    Booking* createNewBooking( FlightManager& flightManager,
                               const std::string& pnr,
                               const std::string& agentId,
                               const std::string& flightInstanceId,
                               const std::string& passengerId,
                               BookingClass bookingClass,
                               double baseFare);

    Ticket* issueTicketForBooking(const std::string& bookingId);
    bool cancelBooking(const std::string& bookingId, const std::string& reason);

    Booking* findBookingById(const std::string& bookingId);
    Booking* findBookingByPNR(const std::string& pnr);
    Ticket* findTicketById(const std::string& ticketId);
    
    bool saveDataToFiles(const std::string& bookingsFilePath, const std::string& ticketsFilePath) const;
};

#endif
