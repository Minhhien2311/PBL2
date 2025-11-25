#ifndef FLIGHT_RULE_H
#define FLIGHT_RULE_H

#include <string>

// Đối tượng chứa bộ luật chung.
// Chỉ có một đối tượng này được tạo khi chương trình khởi động, đọc từ file.
// Admin có thể thay đổi các quy tắc này trong lúc chạy và lưu lại.
class FlightRule {
private:
    // --- Công tắc tổng quyết định tính năng có được BẬT hay không ---
    bool allowCancel;               // true: Về nguyên tắc, hệ thống cho phép hủy vé.
    bool allowChange;               // true: Về nguyên tắc, hệ thống cho phép đổi vé.

    // --- Điều kiện chi tiết ---
    int cancelCutoffHours;          // Số giờ tối thiểu phải hủy trước giờ bay (VD: 24).
    int changeCutoffHours;          // Số giờ tối thiểu phải đổi trước giờ bay (VD: 12).

public:
    // Không cho phép tạo đối tượng rỗng, phải có đầy đủ quy tắc.
    FlightRule() = delete;

    // Constructor: Khởi tạo bộ luật từ dữ liệu
    explicit FlightRule(bool allowCancel,
                        int cancelCutoffHours,
                        bool allowChange,
                        int changeCutoffHours);

    // --- Getters ---
    bool   isCancelAllowed() const;
    int    getCancelCutoffHours() const;
    bool   isChangeAllowed() const;
    int    getChangeCutoffHours() const;

    // --- Setters ---
    void setCancelAllowed(bool allowed);
    void setCancelCutoffHours(int hours);
    void setChangeAllowed(bool allowed);
    void setChangeCutoffHours(int hours);

    // --- Logic kiểm tra nghiệp vụ ---
    // Kiểm tra xem một booking CÓ THỂ HỦY tại một thời điểm cụ thể không.
    bool isCancellable(int hoursUntilDeparture) const;
    // Kiểm tra xem một booking CÓ THỂ ĐỔI tại một thời điểm cụ thể không.
    bool isChangeable(int hoursUntilDeparture) const;

    // --- Đọc/Ghi file cấu hình ---
    // Chuyển đổi đối tượng thành 1 dòng string để lưu vào file.
    std::string toRecordLine() const;
    
    // Tạo đối tượng FlightRule từ 1 dòng string đọc từ file.
    static FlightRule fromRecordLine(const std::string& line);

    static FlightRule* loadFromFile(const std::string& filePath);
};

#endif