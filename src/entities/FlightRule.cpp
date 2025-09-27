#include "C:/PBL2/include/core/FlightRule.h"
#include <algorithm> // For std::clamp
#include <sstream> // For std::stringstream

FlightRule::FlightRule() {}
FlightRule::FlightRule(bool allowCancel,
               bool allowChange,
               bool allowCancelAfterIssued,
               bool allowChangeAfterIssued,
               double cancelFeePercent,
               double changeFeePercent)

    : allowCancel(allowCancel),
      allowChange(allowChange),
      allowCancelAfterIssued(allowCancelAfterIssued),
      allowChangeAfterIssued(allowChangeAfterIssued),
      cancelFeePercent(cancelFeePercent),
      changeFeePercent(changeFeePercent) {}

// Getters
bool FlightRule::isCancelAllowed() const { return allowCancel; }
bool FlightRule::isChangeAllowed() const { return allowChange; }

bool FlightRule::isCancelAfterIssuedAllowed() const { return allowCancelAfterIssued; }
bool FlightRule::isChangeAfterIssuedAllowed() const { return allowChangeAfterIssued; }

double FlightRule::getCancelFeePercent() const { return cancelFeePercent; }
double FlightRule::getChangeFeePercent() const { return changeFeePercent; }

// Setters (kẹp biên hợp lệ)
void FlightRule::setCancelAllowed(bool v) { allowCancel = v; }
void FlightRule::setChangeAllowed(bool v) { allowChange = v; }

void FlightRule::setCancelAfterIssuedAllowed(bool v) { allowCancelAfterIssued = v; }
void FlightRule::setChangeAfterIssuedAllowed(bool v) { allowChangeAfterIssued = v; }

void FlightRule::setCancelFeePercent(double pct) { cancelFeePercent = pct; }
void FlightRule::setChangeFeePercent(double pct) { changeFeePercent = pct; }

// Convenience checks theo ngữ cảnh
bool FlightRule::canCancel(bool afterIssued) const {
    if (!allowCancel) return false;
    if (afterIssued && !allowCancelAfterIssued) return false;
    return true;
}

bool FlightRule::canChange(bool afterIssued) const {
    if (!allowChange) return false;
    if (afterIssued && !allowChangeAfterIssued) return false;
    return true;
}

// Helpers tính phí
// baseAmount là số tiền làm cơ sở tính % (theo quy ước là base fare)
double FlightRule::calcCancelFee(double baseAmount) const {
    if (!allowCancel) return 0.0; // No cancellation allowed, no fee
    return std::max(0.0, baseAmount * cancelFeePercent);
}

double FlightRule::calcChangeFee(double baseAmount) const {
    if (!allowChange) return 0.0; // No change allowed, no fee
    return std::max(0.0, baseAmount * changeFeePercent);
}

// Serialize một dòng cấu hình
std::string FlightRule::toRecordLine() const { }

FlightRule FlightRule::fromRecordLine(const std::string& line) { }