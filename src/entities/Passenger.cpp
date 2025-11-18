#include "entities/Passenger.h"

Passenger::Passenger(const std::string& id,
                     const std::string& name,
                     const std::string& dob,
                     const std::string& phone)
                    : id(id), fullName(name), dateOfBirth(dob), phoneNumber(phone) {}

// --- Getters ---
const std::string& Passenger::getId() const { return id; }
const std::string& Passenger::getFullName() const { return fullName; }
const std::string& Passenger::getDateOfBirth() const { return dateOfBirth; }
const std::string& Passenger::getPhoneNumber() const { return phoneNumber; }

// --- Setters ---
void Passenger::setFullName(const std::string& name) { this->fullName = name; }
void Passenger::setPhoneNumber(const std::string& phone) { this->phoneNumber = phone; }

// --- Đọc/Ghi file ---

// Chuyển đổi đối tượng thành một dòng string để lưu vào file.
std::string Passenger::toRecordLine() const {

    return this->id + "|" +
           this->fullName + "|" +
           this->dateOfBirth + "|" +
           this->phoneNumber;
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

    std::string phone = line.substr(start, end - start);
    start = end + 1;
    end = line.find('|', start);

    // Tạo đối tượng Passenger bằng constructor với ID đã đọc
    Passenger p(id, name, dob, phone);
    
    return p;
}