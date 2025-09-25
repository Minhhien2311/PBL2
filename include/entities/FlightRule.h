// Quy định chuyến bay
#ifndef FLIGHTRULE_H
#define FLIGHTRULE_H

#include <string>

class FlightRule {
private:
    // Định danh & mô tả
    std::string ruleId;
    std::string description;

    // Cờ cho phép
    bool changeAllowed;
    bool cancelAllowed;
    bool upgradeAllowed;

    // Cửa sổ thời gian (tính theo GIỜ trước giờ bay)
    int  minChangeHoursBeforeDeparture;
    int  minCancelHoursBeforeDeparture;

    double changeFee;
    double cancellationFee;
    double upgradeFee; // áp trên chênh lệch giá

public:
    // Ctor đầy đủ
    FlightRule(const std::string& id,
               const std::string& description,
               bool changeAllowed,  int minChangeHours,  double changeFee,
               bool cancelAllowed,  int minCancelHours,  double cancelFee,
               bool upgradeAllowed, double upgFee);

    // Getters
    std::string getRuleId() const;
    std::string getDescription() const;

    bool  isChangeEnabled() const;     // trả về changeAllowed
    bool  isCancelEnabled() const;     // trả về cancelAllowed
    bool  isUpgradeEnabled() const;    // trả về upgradeAllowed

    int   getMinChangeHoursBeforeDeparture() const;
    double getChangeFee() const;

    int   getMinCancelHoursBeforeDeparture() const;
    double getCancellationFee() const;

    double getUpgradeFee() const;

    // Setters cơ bản
    void setChangeFee(double fee);
    void setCancellationFee(double fee);
    void setUpgradeFee(double fee);
    void setMinChangeHoursBeforeDeparture(int hours);
    void setMinCancelHoursBeforeDeparture(int hours);

    // Business (đơn vị GIỜ)
    bool   isChangeAllowed(int hoursBeforeDeparture) const;
    bool   isCancellationAllowed(int hoursBeforeDeparture) const;
    double calculateChangePenalty(double ticketPrice) const;
    double calculateCancellationPenalty(double ticketPrice) const;

    // Rule mặc định (fallback)
    static FlightRule defaultRule();

    // GỢI Ý: loại bỏ hàm này khỏi entity, chuyển sang RuleRepository trong infrastructure
    // static FlightRule findRuleById(const std::string& ruleId);
};

#endif // FLIGHTRULE_H
