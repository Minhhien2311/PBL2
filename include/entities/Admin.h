#ifndef ADMIN_H
#define ADMIN_H

#include "Account.h"

class Admin : public User {
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
};

#endif // ADMIN_H