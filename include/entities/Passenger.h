#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>

enum class Gender {
    Male,
    Female
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
    // Không cho phép tạo đối tượng rỗng.
    Passenger() = delete;

    Passenger(const std::string& id,
              const std::string& name,
              const std::string& dob,
              Gender gender,
              const std::string& phone,
              const std::string& passport,
              const std::string& nation);

    // Destructor mặc định là đủ dùng vì lớp không quản lý tài nguyên động.
    ~Passenger() = default;

    // Getters (trả const ref cho string để tránh copy) 
    const std::string& getId()            const;
    const std::string& getFullName()      const;
    const std::string& getDateOfBirth()   const;
    Gender             getGender()        const; // enum trả theo giá trị
    const std::string& getPhoneNumber()   const;
    const std::string& getPassportNumber()const;
    const std::string& getNationality()   const;

    // --- Setters ---
    // Cho phép cập nhật thông tin hành khách nếu có sai sót khi nhập liệu.
    void setFullName(const std::string& name);
    void setPhoneNumber(const std::string& phone);
    void setPassportNumber(const std::string& passport);
    void setNationality(const std::string& nation);

    void overrideIdForLoad(const std::string& existingId);
    
    // --- Đọc/Ghi file ---
    // Chuyển đổi đối tượng thành 1 dòng string để lưu vào file.
    std::string toRecordLine() const;
    
    // Tạo đối tượng Passenger từ 1 dòng string đọc từ file.
    static Passenger fromRecordLine(const std::string& line);
};

#endif