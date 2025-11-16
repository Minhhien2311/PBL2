#ifndef ROUTE_H
#define ROUTE_H

#include <string>

// Route: Đại diện cho một tuyến bay (route) gốc, không thay đổi.
// Ví dụ: Tuyến bay từ Hà Nội (HAN) đến TP.HCM (SGN) của Vietnam Airlines.
// Không chứa thông tin về ngày/giờ, giá vé, hay số ghế.
// Code được tự động tạo từ: "<DepartureIATA>-<ArrivalIATA>" để dễ tìm kiếm.
class Route {
private:
    std::string routeId;
    std::string departureAirport;   // Sân bay đi (Mã IATA, VD: "HAN")
    std::string arrivalAirport;     // Sân bay đến (Mã IATA, VD: "SGN")

public:
    // Không cho phép tạo đối tượng rỗng
    Route() = delete;

    // Constructor: Tự động tạo ID từ departure-arrival.
    explicit Route( const std::string& departureIATA,
                    const std::string& arrivalIATA);

    // --- Getters ---
    // Trả về tham chiếu hằng để hiệu quả, tránh sao chép không cần thiết.
    const std::string& getRouteId()         const;
    const std::string& getDepartureAirport()const;
    const std::string& getArrivalAirport()  const;

    // --- Đọc/Ghi file ---
    // Chuyển đổi đối tượng thành 1 dòng string để lưu vào file.
    std::string toRecordLine() const;
    
    // Tạo đối tượng Route từ 1 dòng string đọc từ file.
    static Route fromRecordLine(const std::string& line);

    // --- Helper cho việc nạp dữ liệu ---
    // Chỉ dùng cho hàm fromRecordLine để ghi đè ID tự sinh bằng ID đã lưu.
    void overrideIdForLoad(const std::string& existingId);
};

#endif // ROUTE_H