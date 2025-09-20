#ifndef USER_H
#define USER_H

#include "Person.h"
#include <string>

class User : public Person {
protected:
    std::string username;
    std::string passwordHash;
    std::string registrationDate;
    std::string lastLogin;
    std::string role; // "ADMIN" or "AGENT"

public:
    User(const std::string& id, const std::string& name, const std::string& dob, 
         const std::string& email, const std::string& phone, const std::string& addr,
         const std::string& user, const std::string& pwdHash, 
         const std::string& regDate, const std::string& role);
    
    // Gette 
    std::string getUsername() const;
    std::string getPasswordHash() const;
    std::string getRegistrationDate() const;
    std::string getLastLogin() const;
    std::string getRole() const;
    
    // Setter methods
    void setPasswordHash(const std::string& pwdHash);
    void setLastLogin(const std::string& loginTime);
    
    // Authentication methods
    bool authenticate(const std::string& password) const;
    void changePassword(const std::string& newPassword);
    
    virtual void displayInfo() const override;
};

#endif // USER_H