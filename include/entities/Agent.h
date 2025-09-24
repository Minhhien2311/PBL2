#ifndef AGENT_H
#define AGENT_H

#include "Account.h"

class Agent : public Account {
private:
    // double commissionRate;
    double totalSales;
    bool isActive;

public:
    Agent(const std::string& id, const std::string& name, const std::string& dob, 
          const std::string& email, const std::string& phone, const std::string& addr,
          const std::string& user, const std::string& pwdHash, 
          const std::string& regDate, double commission, double sales, bool active);
    
    // Getter methods
    double getCommissionRate() const;
    double getTotalSales() const;
    bool getIsActive() const;
    
    // Setter methods
    void setCommissionRate(double rate);
    void updateTotalSales(double amount);
    void setActiveStatus(bool status);
    
    // Agent-specific methods
    void createBooking();
    void issueTicket();
    void cancelBooking();
    double calculateCommission() const;
    
    void displayInfo() const override;
};

#endif // AGENT_H