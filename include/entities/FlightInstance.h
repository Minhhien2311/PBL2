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
    std::string instanceId;     // ID nội bộ duy nhất, tạo từ flightNumber-YYYYMMDD
    std::string flightId;       // Khóa ngoại, tham chiếu tới Flight gốc
    std::string flightNumber;   // Mã tuyến bay công khai (VD: "VN123")
    
    std::string departureDate;  // Định dạng "DD/MM/YYYY"
    std::string departureTime;  // Định dạng "HH:MM"
    std::string arrivalDate;    // Định dạng "DD/MM/YYYY"
    std::string arrivalTime;    // Định dạng "HH:MM"

    // --- Capacity info ---
    int totalCapacity;          // Tổng số ghế (chỉ dùng để tính layout)

    // --- Giá vé ---
    int fareEconomy;
    int fareBusiness;

public:
    // Không cho phép tạo đối tượng rỗng
    FlightInstance() = delete;

    // Constructor chính: chỉ cần totalCapacity thay vì economy/business riêng
    explicit FlightInstance(const std::string& flightId,
                            const std::string& flightNumber,
                            const std::string& departureDate,
                            const std::string& departureTime,
                            const std::string& arrivalDate,
                            const std::string& arrivalTime,
                            int totalCapacity,
                            int fareEconomy,
                            int fareBusiness);

    // --- Getters ---
    const std::string& getInstanceId() const;
    const std::string& getFlightId() const;
    const std::string& getFlightNumber() const;
    const std::string& getDepartureDate() const;
    const std::string& getDepartureTime() const;
    const std::string& getArrivalDate() const;
    const std::string& getArrivalTime() const;

    // Only total capacity getter needed
    int getTotalCapacity() const;
    
    double getFareEconomy() const;
    double getFareBusiness() const;

    // --- Setters cho các thông tin có thể thay đổi (giá vé) ---
    void setFareEconomy(double fare);
    void setFareBusiness(double fare);

    // --- Hàm tiện ích ---
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