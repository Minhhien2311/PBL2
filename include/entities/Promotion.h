#pragma once

#include <string>
#include <vector>

class Promotion {
private:
    std::string code;
    std::string description;
    std::string discountType;   // "FIXED" hoặc "PERCENTAGE"
    double discountValue;
    std::string startDate;
    std::string endDate;
    bool isActive;

public:
    // --- Constructor ---
    Promotion(const std::string& code, const std::string& desc,
              const std::string& type, double value, const std::string& start,
              const std::string& end, bool active);

    // --- Getters ---
    std::string getCode() const;
    std::string getDescription() const;
    std::string getDiscountType() const;
    double getDiscountValue() const;
    std::string getStartDate() const;
    std::string getEndDate() const;
    bool getIsActive() const;

    // --- Setters ---
    void setIsActive(bool active);
    void setEndDate(const std::string& endDate);

    // --- Logic nghiệp vụ ---
    bool isExpired() const;
    
    // --- Helpers (Static) ---
    static std::vector<Promotion> loadAllPromotions();
    static Promotion findPromotionByCode(const std::string& code);
    static bool validatePromotionCode(const std::string& code);
};