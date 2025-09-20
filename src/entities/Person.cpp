#include <iostream>
#include "C:/PBL2/include/entities/Person.h"

Person::Person(const std::string& id, const std::string& name, const std::string& dob, 
               const std::string& email, const std::string& phone, const std::string& addr)
    : id(id), fullName(name), dateOfBirth(dob), email(email), phoneNumber(phone), address(addr) {}

// Getter methods
std::string Person::getId() const { return id; }
std::string Person::getFullName() const { return fullName; }
std::string Person::getDateOfBirth() const { return dateOfBirth; }
std::string Person::getEmail() const { return email; }
std::string Person::getPhoneNumber() const { return phoneNumber; }
std::string Person::getAddress() const { return address; }

// Setter methods
void Person::setFullName(const std::string& name) { fullName = name; }
void Person::setEmail(const std::string& email) { this->email = email; }
void Person::setPhoneNumber(const std::string& phone) { phoneNumber = phone; }
void Person::setAddress(const std::string& addr) { address = addr; }

void Person::displayInfo() const {
    std::cout << "ID: " << id << std::endl;
    std::cout << "Name: " << fullName << std::endl;
    std::cout << "Date of Birth: " << dateOfBirth << std::endl;
    std::cout << "Email: " << email << std::endl;
    std::cout << "Phone: " << phoneNumber << std::endl;
    std::cout << "Address: " << address << std::endl;
}