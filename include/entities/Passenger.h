#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>

class Passenger {
private:
    std::string id;             // Mã định danh
    std::string fullName;       // Họ tên đầy đủ
    std::string dateOfBirth;    // Khuyến nghị: "yyyy-mm-dd"
    std::string phoneNumber;    // Số điện thoại liên hệ
public:
    // Không cho phép tạo đối tượng rỗng.
    Passenger() = delete;

    Passenger(const std::string& id,
              const std::string& name,
              const std::string& dob,
              const std::string& phone);

    // Destructor mặc định là đủ dùng vì lớp không quản lý tài nguyên động.
    ~Passenger() = default;

    // --- Getters ---
    const std::string& getId()            const;
    const std::string& getFullName()      const;
    const std::string& getDateOfBirth()   const;
    const std::string& getPhoneNumber()   const;
    const std::string& getPassportNumber()const;
    const std::string& getNationality()   const;

    // --- Setters ---
    // Cho phép cập nhật thông tin hành khách nếu có sai sót khi nhập liệu.
    void setFullName(const std::string& name);
    void setPhoneNumber(const std::string& phone);
    
    // --- Đọc/Ghi file ---
    // Chuyển đổi đối tượng thành 1 dòng string để lưu vào file.
    std::string toRecordLine() const;
    
    // Tạo đối tượng Passenger từ 1 dòng string đọc từ file.
    static Passenger fromRecordLine(const std::string& line);
};

#endif