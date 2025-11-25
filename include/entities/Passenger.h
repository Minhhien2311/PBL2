#pragma once

#include <string>

class Passenger {
private:
    std::string id;             // CCCD/Passport
    std::string fullName;
    std::string dateOfBirth;    // yyyy-mm-dd
    std::string phoneNumber;

public:
    // --- Constructor & Destructor ---
    Passenger() = delete;
    Passenger(const std::string& id, const std::string& name,
              const std::string& dob, const std::string& phone);
    ~Passenger() = default;

    // --- Getters ---
    const std::string& getId() const;
    const std::string& getFullName() const;
    const std::string& getDateOfBirth() const;
    const std::string& getPhoneNumber() const;
    
    // Các hàm getter mở rộng (nếu cần cho tương lai)
    const std::string& getPassportNumber() const;
    const std::string& getNationality() const;

    // --- Setters ---
    void setFullName(const std::string& name);
    void setPhoneNumber(const std::string& phone);
    
    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static Passenger fromRecordLine(const std::string& line);
};