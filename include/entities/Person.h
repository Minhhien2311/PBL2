#ifndef PERSON_H
#define PERSON_H

#include <string>

// Định nghĩa enum class Gender ngay trong header này
enum class Gender {
    Male,
    Female,
    Other
};

class Person {
private:
    std::string id;
    std::string fullName;
    std::string dateOfBirth;  // Định dạng khuyến nghị: yyyy-mm-dd
    Gender gender;            // dùng enum class thay vì string
    std::string phoneNumber;

public:
    // Constructor: chỉ khai báo, validate sẽ được triển khai ở .cpp
    Person(const std::string& id,
           const std::string& name,
           const std::string& dob,
           Gender gender,
           const std::string& phone);

    // Đảm bảo hủy đúng khi kế thừa
    virtual ~Person() = default;

    // Getter methods (trả const ref để tránh copy)
    const std::string& getId()          const;
    const std::string& getFullName()    const;
    const std::string& getDateOfBirth() const;
    Gender             getGender()      const;   // trả trực tiếp enum
    const std::string& getPhoneNumber() const;

    // Cho phép override ở lớp con
    virtual void displayInfo() const;
};

#endif // PERSON_H
