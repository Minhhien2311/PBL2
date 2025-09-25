#include <iostream>
#include "C:/PBL2/include/entities/FlightRule.h"

FlightRule::FlightRule(const std::string &id, int minChangeDays, int minCancelDays, double chgFee, double cancelFee, double upgFee, const std::string &classes)
    : ruleId(id), minDaysBeforeChange(minChangeDays), minDaysBeforeCancel(minCancelDays),
      changeFee(chgFee), cancellationFee(cancelFee), upgradeFee(upgFee),
      applicableClasses(classes) {}

// Getter methods
std::string FlightRule::getRuleId() const { return ruleId; }
int FlightRule::getMinDaysBeforeChange() const { return minDaysBeforeChange; }
int FlightRule::getMinDaysBeforeCancel() const { return minDaysBeforeCancel; }
double FlightRule::getChangeFee() const { return changeFee; }
double FlightRule::getCancellationFee() const { return cancellationFee; }
double FlightRule::getUpgradeFee() const { return upgradeFee; }
std::string FlightRule::getApplicableClasses() const { return applicableClasses; }

// Setter methods
void FlightRule::setChangeFee(double fee) { changeFee = fee; }
void FlightRule::setCancellationFee(double fee) { cancellationFee = fee; }
void FlightRule::setUpgradeFee(double fee) { upgradeFee = fee; }
void FlightRule::setApplicableClasses(const std::string& classes) { applicableClasses = classes; }
