#include <iostream>
#include <exception>
#include <QApplication>
#include "Application.h" 

// Include các manager từ core logic
#include "core/AccountManager.h" 
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/ReportManager.h"
#include "core/AirportManager.h"
#include "core/PassengerManager.h" 
#include "entities/FlightRule.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    FlightRule* flightRule = FlightRule::loadFromFile("C:/PBL2/data/flight_rules.txt");

    // --- TẠO CÁC MANAGER ---
    AccountManager accountManager("C:/PBL2/data/admins.txt", "C:/PBL2/data/agents.txt");
    FlightManager flightManager("C:/PBL2/data/routes.txt", "C:/PBL2/data/flights.txt");
    BookingManager bookingManager("C:/PBL2/data/bookings.txt", flightRule); 
    ReportManager reportManager(accountManager, bookingManager);
    AirportManager airportManager("C:/PBL2/data/airports.txt");
    PassengerManager passengerManager("C:/PBL2/data/passengers.txt");

    Application w(&accountManager, &flightManager, &bookingManager,
                  &reportManager, &airportManager, &passengerManager);
    w.show();

    return app.exec();
}
