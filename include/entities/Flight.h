#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>

/*
    Flight: Đại diện cho một tuyến bay (route) gốc, không thay đổi.
    - Ví dụ: Tuyến bay "VN123" từ Hà Nội (HAN) đến TP.HCM (SGN).
    - Không chứa thông tin về ngày/giờ, giá vé, hay số ghế.
 */
class Flight {
private:
    std::string flightId;           // ID nội bộ duy nhất, định dạng: <Airline>-<DepartureIATA>-<ArrivalIATA>
    std::string airline;            // Tên hãng hàng không (VD: "Vietnam Airlines")
    std::string departureAirport;   // Sân bay đi (Mã IATA, VD: "HAN")
    std::string arrivalAirport;     // Sân bay đến (Mã IATA, VD: "SGN")

public:
    // Không cho phép tạo đối tượng rỗng
    Flight() = delete;

    // Constructor chính: ID được tự động sinh từ airline-departureIATA-arrivalIATA
    explicit Flight(const std::string& airline,
                    const std::string& departureIATA,
                    const std::string& arrivalIATA);

    // --- Getters ---
    // Trả về tham chiếu hằng để hiệu quả, tránh sao chép không cần thiết.
    const std::string& getFlightId()        const;
    const std::string& getAirline()         const;
    const std::string& getDepartureAirport()const;
    const std::string& getArrivalAirport()  const;

    // --- Đọc/Ghi file cấu hình ---
    // Chuyển đổi đối tượng thành 1 dòng string để lưu vào file.
    std::string toRecordLine() const;
    
    // Tạo đối tượng Flight từ 1 dòng string đọc từ file.
    static Flight fromRecordLine(const std::string& line);

    // --- Helper cho việc nạp dữ liệu ---
    // Chỉ dùng cho hàm fromRecordLine để ghi đè ID tự sinh bằng ID đã lưu.
    void overrideIdForLoad(const std::string& existingId);

    // --- Hàm tiện ích ---
    // Cung cấp một chuỗi tóm tắt thông tin chuyến bay để hiển thị nhanh.
    std::string getFlightInfo() const;
};

#endif // FLIGHT_H