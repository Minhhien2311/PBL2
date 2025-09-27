#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>

enum class Gender {
    Male,
    Female,
    Other
};

class Passenger {
private:
    std::string id;            // Mã định danh
    std::string fullName;      // Họ tên đầy đủ
    std::string dateOfBirth;   // Khuyến nghị: "yyyy-mm-dd"
    Gender      gender;        // Giới tính (enum class)
    std::string phoneNumber;   // Số điện thoại liên hệ

    std::string passportNumber; // Số hộ chiếu (nếu là nội địa có thể để trống/CCCD)
    std::string nationality;    // Quốc tịch

public:
    Passenger(const std::string& id,
              const std::string& name,
              const std::string& dob,
              Gender gender,
              const std::string& phone,
              const std::string& passport,
              const std::string& nation);

    // Destructor mặc định đủ dùng (không sở hữu tài nguyên động)
    ~Passenger() = default;

    // Getters (trả const ref cho string để tránh copy) 
    const std::string& getId()            const;
    const std::string& getFullName()      const;
    const std::string& getDateOfBirth()   const;
    Gender             getGender()        const; // enum trả theo giá trị
    const std::string& getPhoneNumber()   const;
    const std::string& getPassportNumber()const;
    const std::string& getNationality()   const;
};

#endif