// Thông tin khuyến mãi
#ifndef PROMOTION_H
#define PROMOTION_H

#include <string>
#include <vector>

class Promotion {
private:
    std::string promotionId;
    std::string code;
    std::string description;
    std::string discountType; // "FIXED" or "PERCENTAGE"
    double discountValue;
    std::string startDate;
    std::string endDate;
    bool isActive;

public:
    Promotion(const std::string& id, const std::string& code, const std::string& desc,
              const std::string& type, double value, const std::string& start,
              const std::string& end, bool active);
    
    // Getter methods
    std::string getPromotionId() const;
    std::string getCode() const;
    std::string getDescription() const;
    std::string getDiscountType() const;
    double getDiscountValue() const;
    std::string getStartDate() const;
    std::string getEndDate() const;
    bool getIsActive() const;
    
    // Setter methods
    void setIsActive(bool active);
    void setEndDate(const std::string& endDate);
    
    // Business methods
    bool isValid() const;
    bool isExpired() const;
    double applyDiscount(double originalPrice) const;
    bool isApplicableForFlight(const std::string& flightId) const;
    bool isApplicableForClass(const std::string& seatClass) const;
    
    static std::vector<Promotion> loadAllPromotions();
    static Promotion findPromotionByCode(const std::string& code);
    static Promotion findPromotionById(const std::string& id);
    static bool validatePromotionCode(const std::string& code);
};

#endif // PROMOTION_H