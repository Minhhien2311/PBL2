// Quy định chuyến bay
#ifndef FLIGHTRULE_H
#define FLIGHTRULE_H

#include <string>

class FlightRule {
private:
    std::string ruleId;
    int minDaysBeforeChange;
    int minDaysBeforeCancel;
    double changeFee;
    double cancellationFee;
    double upgradeFee;
    std::string applicableClasses;

public:
    FlightRule(const std::string& id, int minChangeDays, int minCancelDays,
               double chgFee, double cancelFee, double upgFee,
               const std::string& classes);
    
    // Getter methods
    std::string getRuleId() const;
    int getMinDaysBeforeChange() const;
    int getMinDaysBeforeCancel() const;
    double getChangeFee() const;
    double getCancellationFee() const;
    double getUpgradeFee() const;
    std::string getApplicableClasses() const;
    
    // Setter methods
    void setChangeFee(double fee);
    void setCancellationFee(double fee);
    void setUpgradeFee(double fee);
    void setApplicableClasses(const std::string& classes);
    
    // Business methods
    bool isChangeAllowed(int daysBeforeFlight) const;
    bool isCancellationAllowed(int daysBeforeFlight) const;
    double calculateChangePenalty(double ticketPrice) const;
    double calculateCancellationRefund(double ticketPrice) const;
    bool isApplicableForClass(const std::string& seatClass) const;
    
    static FlightRule getDefaultRule();
    static FlightRule findRuleById(const std::string& ruleId);
    static FlightRule findRuleForFlight(const std::string& flightId);
};

#endif // FLIGHTRULE_H