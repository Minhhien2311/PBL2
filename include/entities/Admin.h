#ifndef ADMIN_H
#define ADMIN_H

#include "Account.h"
#include <vector>

class Admin : public Account {
public:
    Admin(const std::string& id, const std::string& name, const std::string& dob, 
          const std::string& email, const std::string& phone, const std::string& addr,
          const std::string& user, const std::string& pwdHash, 
          const std::string& regDate);
    
    // Admin-specific methods
    void manageFlights();
    void manageAgents();
    void generateReports();
    void configureSystemSettings();
    
    void displayInfo() const override;

private:
    // Flight management methods
    void addNewFlight();
    void editFlight();
    void deleteFlight();
    void viewAllFlights();
    void manageFlightInstances();
    
    // Agent management methods
    void addNewAgent();
    void editAgent();
    void deleteAgent();
    void viewAllAgents();
    void viewAgentReports();
    
    // Report generation methods
    void generateDailyRevenueReport();
    void generateWeeklyRevenueReport();
    void generateMonthlyRevenueReport();
    void generateTicketSalesReport();
    void generateAgentPerformanceReport();
    void viewActivityLogs();
    
    // System configuration methods
    void managePromotions();
    void configureFlightRules();
    void configurePricing();
    void backupData();
    void restoreData();
};

#endif // ADMIN_H