#ifndef FLIGHT_RULE_H
#define FLIGHT_RULE_H

#include <string>

class FlightRule {
private:
    //  Quyền thao tác 
    bool allowCancel;               // có cho phép hủy không
    bool allowChange;               // có cho phép đổi thông tin (ngày/giờ/ghế) không

    // Cho phép sau khi đã Issued?
    bool allowCancelAfterIssued;    // mặc định: true (hủy sau xuất vé nhưng có phí)
    bool allowChangeAfterIssued;    // mặc định: true (đổi sau xuất vé có tính chênh lệch)

    // Tham số phí 
    // Phí đổi/hủy là % tính trên base fare (hoặc total quy ước, mặc định base).
    double cancelFeePercent;
    double changeFeePercent;

public:
    // Constructors 
    FlightRule();
    FlightRule(bool allowCancel,
               bool allowChange,
               bool allowCancelAfterIssued,
               bool allowChangeAfterIssued,
               double cancelFeePercent,
               double changeFeePercent);

    // Getters 
    bool   isCancelAllowed() const;
    bool   isChangeAllowed() const;

    bool   isCancelAfterIssuedAllowed() const;
    bool   isChangeAfterIssuedAllowed() const;

    double getCancelFeePercent() const;
    double getChangeFeePercent() const;

    // Setters (kẹp biên hợp lệ) 
    void setCancelAllowed(bool v);
    void setChangeAllowed(bool v);

    void setCancelAfterIssuedAllowed(bool v);
    void setChangeAfterIssuedAllowed(bool v);

    void setCancelFeePercent(double pct);   // clamp [0..1]
    void setChangeFeePercent(double pct);   // clamp [0..1]

    // Convenience checks theo ngữ cảnh 
    bool canCancel(bool afterIssued)  const;  // xét theo cờ allow* và afterIssued
    bool canChange(bool afterIssued)  const;

    // Helpers tính phí 
    // baseAmount là số tiền làm cơ sở tính % (theo quy ước là base fare)
    double calcCancelFee(double baseAmount) const;            // = max(0, baseAmount * cancelFeePercent/100)
    double calcChangeFee(double baseAmount) const;            // = max(0, baseAmount * changeFeePercent/100)

    //  Serialize một dòng cấu hình — để sau nếu cần 
    std::string toRecordLine() const;
    static FlightRule fromRecordLine(const std::string& line);
};

#endif // FLIGHT_RULE_H
