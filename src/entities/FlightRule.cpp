#include "C:/PBL2/include/entities/FlightRule.h"
#include <string>

// --- Constructor ---
FlightRule::FlightRule(bool allowCancel,
                       bool allowChange,
                       int cancelHours,
                       int changeHours,
                       double cancelFeePercent,
                       double changeFeePercent)
    : allowCancel(allowCancel),
      allowChange(allowChange),
      cancelCutoffHours(cancelHours),
      changeCutoffHours(changeHours),
      cancelFeePercent(cancelFeePercent),
      changeFeePercent(changeFeePercent) {}

// --- Getters ---
bool   FlightRule::isCancelAllowed() const { return allowCancel; }
bool   FlightRule::isChangeAllowed() const { return allowChange; }
int    FlightRule::getCancelCutoffHours() const { return cancelCutoffHours; }
int    FlightRule::getChangeCutoffHours() const { return changeCutoffHours; }
double FlightRule::getCancelFeePercent() const { return cancelFeePercent; }
double FlightRule::getChangeFeePercent() const { return changeFeePercent; }

// --- Setters ---
void FlightRule::setCancelAllowed(bool allowed) { this->allowCancel = allowed; }
void FlightRule::setChangeAllowed(bool allowed) { this->allowChange = allowed; }
void FlightRule::setCancelCutoffHours(int hours) { this->cancelCutoffHours = hours; }
void FlightRule::setChangeCutoffHours(int hours) { this->changeCutoffHours = hours; }
void FlightRule::setCancelFeePercent(double percent) { this->cancelFeePercent = percent; }
void FlightRule::setChangeFeePercent(double percent) { this->changeFeePercent = percent; }

// --- Logic kiểm tra nghiệp vụ ---
bool FlightRule::isCancellable(int hoursUntilDeparture) const {
    if (!this->allowCancel) {
        return false;
    }
    return hoursUntilDeparture >= this->cancelCutoffHours;
}

bool FlightRule::isChangeable(int hoursUntilDeparture) const {
    if (!this->allowChange) {
        return false;
    }
    return hoursUntilDeparture >= this->changeCutoffHours;
}

// --- Helpers tính phí ---
double FlightRule::calculateCancelFee(double baseAmount, int hoursUntilDeparture) const {
    return baseAmount * this->cancelFeePercent;
}

double FlightRule::calculateChangeFee(double baseAmount, int hoursUntilDeparture) const {
    return baseAmount * this->changeFeePercent;
}

// --- Đọc/Ghi file cấu hình ---

// Ghi đối tượng vào file dữ liệu
std::string FlightRule::toRecordLine() const {
    return (this->allowCancel ? "1" : "0") + std::string("|") +
           (this->allowChange ? "1" : "0") + std::string("|") +
           std::to_string(this->cancelCutoffHours) + std::string("|") +
           std::to_string(this->changeCutoffHours) + std::string("|") +
           std::to_string(this->cancelFeePercent) + std::string("|") +
           std::to_string(this->changeFeePercent);
}

// Đọc dữ liệu từ file và tạo đối tượng
FlightRule FlightRule::fromRecordLine(const std::string& line) {
    // Giả định rằng dữ liệu đầu vào luôn đúng định dạng.
    
    size_t start = 0;
    size_t end = line.find('|');
    
    // Tách phần 1: allowCancel
    bool bAllowCancel = (line.substr(start, end - start) == "1");
    start = end + 1;
    end = line.find('|', start);

    // Tách phần 2: allowChange
    bool bAllowChange = (line.substr(start, end - start) == "1");
    start = end + 1;
    end = line.find('|', start);

    // Tách phần 3: cancelCutoffHours
    int iCancelCutoff = std::stoi(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    // Tách phần 4: changeCutoffHours
    int iChangeCutoff = std::stoi(line.substr(start, end - start));
    start = end + 1;
    end = line.find('|', start);

    // Tách phần 5: cancelFeePercent
    double dCancelFee = std::stod(line.substr(start, end - start));
    start = end + 1;
    // Phần cuối cùng không có dấu | ở sau
    end = line.length();

    // Tách phần 6: changeFeePercent
    double dChangeFee = std::stod(line.substr(start, end - start));

    return FlightRule(bAllowCancel, bAllowChange, iCancelCutoff, iChangeCutoff, dCancelFee, dChangeFee);
}