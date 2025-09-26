#ifndef AGENT_H
#define AGENT_H

#include <string>
#include "Person.h"
#include "Account.h"

// Agent: đại lý bán vé, là một người (Person) có một tài khoản đăng nhập (Account).
// Ngoài thông tin cá nhân, Agent còn có các chỉ số kinh doanh như doanh số và trạng thái hoạt động (active/inactive).
class Agent : public Person {
private:
    Account account;       // thông tin đăng nhập (Role::Agent)
    double totalSales;     // tổng số tiền bán vé (để tính hoa hồng)
    bool isActive;         // đại lý còn hoạt động hay đã bị khóa

public:
    // Khởi tạo Agent: gồm thông tin Person + Account + thông tin kinh doanh
    Agent(const std::string& id,
          const std::string& name,
          const std::string& dob,
          Gender gender,
          const std::string& phone,
          const Account& acc,
          double totalSales = 0,
          bool active);

    // Truy cập account
    const Account& getAccount() const;  // chỉ đọc
    Account&       getAccount();        // cho phép chỉnh sửa khi là chính Agent

    // Thông tin kinh doanh
    double getTotalSales() const;
    bool   getIsActive() const;

    // Các hàm set
    void updateTotalSales(double amount); // Cộng thêm vào doanh số. Cái này không phải Agent tự nhập tay mà được cập nhật từ Booking/Ticket Service.
    void setActive(bool status);

    // Hiển thị thông tin (Person + Account + số liệu kinh doanh)
    void displayInfo() const override;
};

#endif // AGENT_H
