#ifndef FLIGHT_INSTANCE_H
#define FLIGHT_INSTANCE_H

#include <string>

// Dùng enum để định nghĩa rõ các hạng ghế, giúp các hàm nghiệp vụ an toàn hơn.
enum class SeatClass {
    Economy,
    Business
};

/*
    FlightInstance: Đại diện cho MỘT LẦN BAY CỤ THỂ của một Flight.
    - Đối tượng này tự quản lý số ghế trống để đảm bảo dữ liệu luôn nhất quán.
    - Ví dụ: Chuyến bay VN123 vào lúc 08:00 ngày 12/10/2025.
 */
class FlightInstance {
private:
    std::string instanceId;     // ID nội bộ duy nhất, được sinh tự động (VD: "FI-12345")
    std::string flightId;       // Khóa ngoại, tham chiếu tới Flight gốc
    
    std::string departureDate;  // Định dạng "DD/MM/YYYY"
    std::string departureTime;  // Định dạng "HH:MM"
    std::string arrivalDate;    // Định dạng "DD/MM/YYYY"
    std::string arrivalTime;    // Định dạng "HH:MM"

    // --- Quản lý ghế ---
    // Chỉ lưu trữ các thông tin gốc, các thông tin khác sẽ được tính toán.
    int economyTotal;           // Tổng số ghế hạng phổ thông
    int economyAvailable;       // Số ghế phổ thông còn trống
    int businessTotal;          // Tổng số ghế hạng thương gia
    int businessAvailable;      // Số ghế thương gia còn trống

    // --- Giá vé ---
    int fareEconomy;
    int fareBusiness;

public:
    // Không cho phép tạo đối tượng rỗng
    FlightInstance() = delete;

    // Constructor chính: Tự động sinh ID và tính toán số ghế ban đầu.
    // Chỉ cần cung cấp thông tin gốc, không cần truyền vào số ghế trống.
    explicit FlightInstance(const std::string& flightId,
                            const std::string& departureDate,
                            const std::string& departureTime,
                            const std::string& arrivalDate,
                            const std::string& arrivalTime,
                            int totalEconomySeats,
                            int totalBusinessSeats,
                            int fareEconomy,
                            int fareBusiness);

    // --- Getters ---
    const std::string& getInstanceId() const;
    const std::string& getFlightId() const;
    const std::string& getDepartureDate() const;
    const std::string& getDepartureTime() const;
    const std::string& getArrivalDate() const;
    const std::string& getArrivalTime() const;

    // Getters cho ghế: Các giá trị tổng được tính toán tự động, đảm bảo luôn đúng.
    int getTotalCapacity() const;       // Trả về: economyTotal + businessTotal
    int getTotalSeatsAvailable() const; // Trả về: economyAvailable + businessAvailable
    
    int getEconomyTotal() const;
    int getEconomyAvailable() const;
    int getBusinessTotal() const;
    int getBusinessAvailable() const;
    
    double getFareEconomy() const;
    double getFareBusiness() const;

    // --- Hàm nghiệp vụ ---
    // Giảm số ghế trống khi có người đặt vé. Trả về true nếu thành công.
    bool bookSeats(SeatClass seatClass, int count = 1);

    // Tăng lại số ghế trống khi có người hủy vé.
    void releaseSeats(SeatClass seatClass, int count = 1);

    // --- Setters cho các thông tin có thể thay đổi (giá vé) ---
    void setFareEconomy(double fare);
    void setFareBusiness(double fare);

    // --- Hàm tiện ích ---
    bool hasAvailableSeats() const;     // Kiểm tra xem còn ghế trống hay không
    void displayInfo() const;           // Hiển thị thông tin chi tiết

    // --- Đọc/Ghi file cấu hình ---
    // Chuyển đổi đối tượng thành 1 dòng string để lưu vào file.
    std::string toRecordLine() const;
    
    // Tạo đối tượng FlightInstance từ 1 dòng string đọc từ file.
    static FlightInstance fromRecordLine(const std::string& line);

    // --- Helper cho việc nạp dữ liệu ---
    // Chỉ dùng cho hàm fromRecordLine để ghi đè ID tự sinh bằng ID đã lưu.
    void overrideIdForLoad(const std::string& existingId);
};

#endif