#include "entities/FlightRule.h"
#include <string>
#include <stdexcept>

// --- Constructor (Đã đơn giản hóa) ---
FlightRule::FlightRule(bool allowCancel,
                       int cancelHours,
                       bool allowChange,
                       int changeHours)
    : allowCancel(allowCancel),
      cancelCutoffHours(cancelHours),
      allowChange(allowChange),
      changeCutoffHours(changeHours) {}

// --- Getters ---
bool   FlightRule::isCancelAllowed() const { return allowCancel; }
int    FlightRule::getCancelCutoffHours() const { return cancelCutoffHours; }
bool   FlightRule::isChangeAllowed() const { return allowChange; }
int    FlightRule::getChangeCutoffHours() const { return changeCutoffHours; }

// --- Setters ---
void FlightRule::setCancelAllowed(bool allowed) { this->allowCancel = allowed; }
void FlightRule::setCancelCutoffHours(int hours) { this->cancelCutoffHours = hours; }
void FlightRule::setChangeAllowed(bool allowed) { this->allowChange = allowed; }
void FlightRule::setChangeCutoffHours(int hours) { this->changeCutoffHours = hours; }

// --- Logic kiểm tra nghiệp vụ ---
bool FlightRule::isCancellable(int hoursUntilDeparture) const {
    if (!this->allowCancel) {
        return false;
    }
    // Logic: Phải hủy trước X giờ
    return hoursUntilDeparture >= this->cancelCutoffHours;
}

bool FlightRule::isChangeable(int hoursUntilDeparture) const {
    if (!this->allowChange) {
        return false;
    }
    // Logic: Phải đổi trước Y giờ
    return hoursUntilDeparture >= this->changeCutoffHours;
}

// --- Đọc/Ghi file cấu hình (Đã đơn giản hóa) ---

// Ghi đối tượng vào file dữ liệu (3 trường)
std::string FlightRule::toRecordLine() const {
    return (this->allowCancel ? "1" : "0") + std::string("|") +
           std::to_string(this->cancelCutoffHours) + std::string("|") +
           (this->allowChange ? "1" : "0") + std::string("|") +
           std::to_string(this->changeCutoffHours);
}

// Đọc dữ liệu từ file và tạo đối tượng (3 trường)
FlightRule FlightRule::fromRecordLine(const std::string& line) {
    size_t start = 0;
    size_t end = line.find('|');
    
    // Tách phần 1: allowCancel
    bool bAllowCancel = (line.substr(start, end - start) == "1");
    start = end + 1;
    end = line.find('|', start);

    // Tách phần 2: cancelCutoffHours
    int iCancelCutoff = std::stoi(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    // Tách phần 3: allowChange
    bool bAllowChange = (line.substr(start, end - start) == "1");
    start = end + 1;
    end = line.length(); // Phần cuối cùng

    // Tách phần 4: changeCutoffHours
    int iChangeCutoff = std::stoi(line.substr(start, end - start));

    return FlightRule(bAllowCancel, iCancelCutoff, bAllowChange, iChangeCutoff);
}