// Thông tin chuyến bay chung
#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>
#include <vector>

class Flight {
private:
    std::string flightId;
    std::string flightNumber;
    std::string airline;
    std::string aircraftType;
    std::string departureAirport;
    std::string arrivalAirport;
    double basePrice;
    int totalSeats;
    int businessSeats;
    int economySeats;

public:
    Flight(const std::string& id, const std::string& number, const std::string& airline,
           const std::string& aircraft, const std::string& departure, const std::string& arrival,
           double price, int totalSeats, int businessSeats, int economySeats);
    
    // Getter methods
    std::string getFlightId() const;
    std::string getFlightNumber() const;
    std::string getAirline() const;
    std::string getAircraftType() const;
    std::string getDepartureAirport() const;
    std::string getArrivalAirport() const;
    double getBasePrice() const;
    int getTotalSeats() const;
    int getBusinessSeats() const;
    int getEconomySeats() const;
    
    // Setter methods
    void setBasePrice(double price);
    void setAircraftType(const std::string& aircraft);
    
    // Flight information
    std::string getFlightInfo() const;
    bool isInternational() const;
    
    static std::vector<Flight> loadAllFlights();
    static Flight findFlightById(const std::string& flightId);
    static std::vector<Flight> searchFlights(const std::string& departure, 
                                            const std::string& arrival, 
                                            const std::string& date);
};

#endif // FLIGHT_H