#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>

// Flight: Đại diện cho MỘT LẦN BAY CỤ THỂ của một Flight.
// Ví dụ: Chuyến bay VN123 vào lúc 08:00 ngày 12/10/2025.
class Flight {
private:
    std::string flightId;       // ID nội bộ duy nhất, tạo từ flightNumber-YYYYMMDD
    std::string routeId;        // Khóa ngoại, tham chiếu tới Route gốc
    std::string airline;        // Tên hãng hàng không (VD: "Vietnam Airlines")
    std::string flightNumber;   // Mã tuyến bay công khai (VD: "VN123")
    
    std::string departureDate;  // Định dạng "DD/MM/YYYY"
    std::string departureTime;  // Định dạng "HH:MM"
    std::string arrivalDate;    // Định dạng "DD/MM/YYYY"
    std::string arrivalTime;    // Định dạng "HH:MM"

    // --- Capacity info ---
    int availableSeats;     // Số ghế còn trống
    int totalCapacity;          // Tổng số ghế

    // --- Giá vé ---
    int fareEconomy;
    int fareBusiness;

public:
    // Không cho phép tạo đối tượng rỗng
    Flight() = delete;

    // Constructor
    explicit Flight(const std::string& routeId,
                    const std::string& airline,
                    const std::string& flightNumber,
                    const std::string& departureDate,
                    const std::string& departureTime,
                    const std::string& arrivalDate,
                    const std::string& arrivalTime,
                    int totalCapacity,
                    int availableSeats,
                    int fareEconomy,
                    int fareBusiness);

    // --- Getters ---
    const std::string& getFlightId  () const;
    const std::string& getRouteId() const;
    const std::string& getAirline() const;
    const std::string& getFlightNumber() const;
    const std::string& getDepartureDate() const;
    const std::string& getDepartureTime() const;
    const std::string& getArrivalDate() const;
    const std::string& getArrivalTime() const;
    int getAvailableSeats() const;
    int getTotalCapacity() const;
    
    double getFareEconomy() const;
    double getFareBusiness() const;

    // --- Setters cho các thông tin có thể thay đổi ---
    void setFareEconomy(double fare);
    void setFareBusiness(double fare);
    void setAirline(const std::string& airline);
    void setAvailableSeats(int seats);

    // --- Đọc/Ghi file cấu hình ---
    // Chuyển đổi đối tượng thành 1 dòng string để lưu vào file.
    std::string toRecordLine() const;
    
    // Tạo đối tượng Flight từ 1 dòng string đọc từ file.
    static Flight fromRecordLine(const std::string& line);

    // --- Helper cho việc nạp dữ liệu ---
    // Chỉ dùng cho hàm fromRecordLine để ghi đè ID tự sinh bằng ID đã lưu.
    void overrideIdForLoad(const std::string& existingId);
};

#endif
