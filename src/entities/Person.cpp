#include <iostream>
#include "C:/PBL2/include/entities/Person.h"

// Constructor
Person::Person(const std::string& id,
               const std::string& name,
               const std::string& dob,
               Gender gender,
               const std::string& phone)
    : id(id), fullName(name), dateOfBirth(dob), gender(gender), phoneNumber(phone) {
    // Nếu muốn validate thì gọi hàm từ Validation.cpp ở đây
}

// Getter methods
const std::string& Person::getId() const { return id; }

const std::string& Person::getFullName() const { return fullName; }

const std::string& Person::getDateOfBirth() const { return dateOfBirth; }

Gender Person::getGender() const { return gender; }

const std::string& Person::getPhoneNumber() const { return phoneNumber; }

// Hàm tiện ích nội bộ: chuyển enum Gender sang string để hiển thị
static std::string genderToString(Gender g) {
    switch (g) {
        case Gender::Male:   return "Male";
        case Gender::Female: return "Female";
        case Gender::Other:  return "Other";
    }
    return "Unknown";
}

// Hiển thị thông tin (có thể override ở lớp con)
void Person::displayInfo() const {
    std::cout << "ID: " << id << "\n"
              << "Name: " << fullName << "\n"
              << "DOB: " << dateOfBirth << "\n"
              << "Gender: " << genderToString(gender) << "\n"
              << "Phone: " << phoneNumber << "\n";
}
