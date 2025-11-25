#pragma once

#include <string>

// Quy định vé (Hủy/Đổi)
class FlightRule {
private:
    bool allowCancel;           // Cho phép hủy?
    bool allowChange;           // Cho phép đổi?
    int cancelCutoffHours;      // Hạn chót hủy (trước giờ bay)
    int changeCutoffHours;      // Hạn chót đổi (trước giờ bay)

public:
    // --- Constructor ---
    FlightRule() = delete;
    explicit FlightRule(bool allowCancel, int cancelCutoffHours,
                        bool allowChange, int changeCutoffHours);

    // --- Getters ---
    bool isCancelAllowed() const;
    int getCancelCutoffHours() const;
    bool isChangeAllowed() const;
    int getChangeCutoffHours() const;

    // --- Setters ---
    void setCancelAllowed(bool allowed);
    void setCancelCutoffHours(int hours);
    void setChangeAllowed(bool allowed);
    void setChangeCutoffHours(int hours);

    // --- Kiểm tra nghiệp vụ ---
    bool isCancellable(int hoursUntilDeparture) const;
    bool isChangeable(int hoursUntilDeparture) const;

    // --- Đọc/Ghi file ---
    std::string toRecordLine() const;
    static FlightRule fromRecordLine(const std::string& line);
    static FlightRule* loadFromFile(const std::string& filePath);
};