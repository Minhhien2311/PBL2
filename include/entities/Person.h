#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person {
protected:
    std::string id;
    std::string fullName;
    std::string dateOfBirth;
    std::string email;
    std::string phoneNumber;
    std::string address;

public:
    Person(const std::string& id, const std::string& name, const std::string& dob, 
           const std::string& email, const std::string& phone, const std::string& addr);
    
    virtual ~Person() = default;
    
    // Getter methods
    std::string getId() const;
    std::string getFullName() const;
    std::string getDateOfBirth() const;
    std::string getEmail() const;
    std::string getPhoneNumber() const;
    std::string getAddress() const;
    
    // Setter methods
    void setFullName(const std::string& name);
    void setEmail(const std::string& email);
    void setPhoneNumber(const std::string& phone);
    void setAddress(const std::string& addr);
    
    virtual void displayInfo() const;
};

#endif // PERSON_H