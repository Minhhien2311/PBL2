#include <iostream>
#include "entities/Promotion.h"

Promotion::Promotion(const std::string& code, const std::string& desc,
                     const std::string& type, double value, const std::string& start,
                     const std::string& end, bool active)
                :    code(code), description(desc), discountType(type),
                     discountValue(value), startDate(start), endDate(end), isActive(active) {}

// Các hàm getter
std::string Promotion::getCode() const { return code; }                 // get Code
std::string Promotion::getDescription() const { return description; }   // get Description
std::string Promotion::getDiscountType() const { return discountType; } // get DiscountType
double Promotion::getDiscountValue() const { return discountValue; }    // get DiscountValue - Cân nhắn bỏ khúc này
std::string Promotion::getStartDate() const { return startDate; }       // get StartDate
std::string Promotion::getEndDate() const { return endDate; }           // get EndDate
bool Promotion::getIsActive() const { return isActive; }                // get IsActive

// Các hàm setter
void Promotion::setIsActive(bool active) { isActive = active; }
void Promotion::setEndDate(const std::string& endDate) { this->endDate = endDate; } // Tại sao input ở đây lại là const string& ?

// Hàm check logic
bool Promotion::isExpired() const {
    // Có vẻ cần hàm get thời gian thực
    // So sánh EndDate và ngày hiện tại
    return false; // Placeholder
}

std::vector<Promotion> Promotion::loadAllPromotions() {
    // Implement logic to load all promotions from a data source
    // Tạm thời chưa biết cách triển khai, có phải tự viết class vector không nè trời...
    return {}; // Placeholder
}

Promotion Promotion::findPromotionByCode(const std::string& code) {
    // Implement logic to find a promotion by its code
    // Này là kiểu tìm data rồi tạo đối tượng đúng không? Ta nghĩ mình nên sắp xếp theo thứ tự quy trình rồi comment cho dễ xài
    // Thuật toán: ....(đang làm)
    return Promotion("", "", "", 0.0, "", "", false); // Placeholder
}

bool Promotion::validatePromotionCode(const std::string& code) {
    // Implement logic to validate a promotion code format or existence
    // Đây là gì...?
    return true; // Placeholder
}