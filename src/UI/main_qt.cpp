#include <QApplication>
#include "Application.h" 

// Include các manager từ core logic
#include "core/AccountManager.h" 
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "core/AirportManager.h"
#include "core/PassengerManager.h"  // ✅ THÊM: PassengerManager
#include "entities/FlightRule.h"   // ✅ THÊM: FlightRule

#include <iostream>
#include <exception>

int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);

        FlightRule* flightRule = FlightRule::loadFromFile("C:/PBL2/data/flight_rules.txt");

        // --- TẠO CÁC MANAGER ---
        AccountManager accountManager("C:/PBL2/data/admins.txt", "C:/PBL2/data/agents.txt");
        FlightManager flightManager("C:/PBL2/data/routes.txt", "C:/PBL2/data/flights.txt");
        BookingManager bookingManager("C:/PBL2/data/bookings.txt", flightRule); 
        ReportManager reportManager(accountManager, bookingManager);
        AirportManager airportManager("C:/PBL2/data/airports.txt");
        // ✅ THÊM: Khởi tạo PassengerManager
        PassengerManager passengerManager("C:/PBL2/data/passengers.txt");

        // --- KHỞI ĐỘNG ỨNG DỤNG ---
        Application w(&accountManager, &flightManager, &bookingManager,
                      &reportManager, &airportManager, &passengerManager);
        w.show();

        return app.exec();
    } 
    catch (const std::invalid_argument& e) {
        std::cerr << "[EXCEPTION] Invalid argument (stoi failed): " << e.what() << std::endl;
    } 
    catch (const std::out_of_range& e) {
        std::cerr << "[EXCEPTION] Out of range: " << e.what() << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "[EXCEPTION] " << e.what() << std::endl;
    } 
    catch (...) {
        std::cerr << "[EXCEPTION] Unknown exception!" << std::endl;
    }
    return -1;
}
