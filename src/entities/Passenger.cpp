#include "entities/Passenger.h"

Passenger::Passenger(const std::string& id,
                     const std::string& name,
                     const std::string& dob,
                     Gender gender,
                     const std::string& phone,
                     const std::string& passport,
                     const std::string& nation)
                    : id(id), fullName(name), dateOfBirth(dob), gender(gender), phoneNumber(phone), passportNumber(passport), nationality(nation) {}

// --- Getters ---
const std::string& Passenger::getId() const { return id; }
const std::string& Passenger::getFullName() const { return fullName; }
const std::string& Passenger::getDateOfBirth() const { return dateOfBirth; }
Gender Passenger::getGender() const { return gender; }
const std::string& Passenger::getPhoneNumber() const { return phoneNumber; }
const std::string& Passenger::getPassportNumber() const { return passportNumber; }
const std::string& Passenger::getNationality() const { return nationality; }

// --- Setters ---
void Passenger::setFullName(const std::string& name) { this->fullName = name; }
void Passenger::setPhoneNumber(const std::string& phone) { this->phoneNumber = phone; }
void Passenger::setPassportNumber(const std::string& passport) { this->passportNumber = passport; }
void Passenger::setNationality(const std::string& nation) { this->nationality = nation; }

// --- Đọc/Ghi file ---

// Chuyển đổi đối tượng thành một dòng string để lưu vào file.
std::string Passenger::toRecordLine() const {
    // Chuyển đổi enum Gender thành số nguyên (0: Male, 1: Female) để lưu file
    std::string genderStr = std::to_string(static_cast<int>(this->gender));

    return this->id + "|" +
           this->fullName + "|" +
           this->dateOfBirth + "|" +
           genderStr + "|" +
           this->phoneNumber + "|" +
           this->passportNumber + "|" +
           this->nationality;
}

// Tạo đối tượng Passenger từ một dòng string đọc từ file.
Passenger Passenger::fromRecordLine(const std::string& line) {
    // Dùng phương pháp phân tích chuỗi thủ công đã thống nhất
    size_t start = 0;
    size_t end = line.find('|');
    
    // Tách ID (lưu ý: ID được đọc từ file, không sinh mới)
    std::string id = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    // Tách các trường còn lại
    std::string name = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string dob = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    // Chuyển đổi chuỗi số thành enum Gender
    int genderInt = std::stoi(line.substr(start, end - start));
    Gender gender = static_cast<Gender>(genderInt);
    start = end + 1;
    end = line.find('|', start);

    std::string phone = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    std::string passport = line.substr(start, end - start);
    start = end + 1;
    end = line.length(); // Phần cuối cùng

    std::string nation = line.substr(start, end - start);

    // Tạo đối tượng Passenger bằng constructor với ID đã đọc
    Passenger p(id, name, dob, gender, phone, passport, nation);
    
    return p;
}

void Passenger::overrideIdForLoad(const std::string& existingId) {
    this->id = existingId;
}