#include <iostream>
#include "C:/PBL2/include/entities/Promotion.h"

Promotion::Promotion(const std::string& id, const std::string& code, const std::string& desc,
              const std::string& type, double value, const std::string& start,
              const std::string& end, bool active)
    : promotionId(id), code(code), description(desc), discountType(type),
      discountValue(value), startDate(start), endDate(end), isActive(active) {}

// Các hàm getter
std::string Promotion::getPromotionId() const { return promotionId; }   // get PromotionId
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
bool Promotion::isValid() const {
    // Thực hiện kiểm tra tính hợp lệ (có trong danh sách mã không kiểu thế)
    return isActive; // Placeholder
}

bool Promotion::isExpired() const {
    // Có vẻ cần hàm get thời gian thực
    // So sánh EndDate và ngày hiện tại
    return false; // Placeholder
}

// Hàm tính giá vé sau khi đã áp dụng giảm giá
double Promotion::applyDiscount(double originalPrice) const {
    if (!isValid()) {
        return originalPrice;
    }
    if (discountType == "percentage") {
        return originalPrice * (1 - discountValue / 100.0);
    } else if (discountType == "fixed") {
        return originalPrice - discountValue;
    }
    return originalPrice;
}

bool Promotion::isApplicableForFlight(const std::string& flightId) const {
    // Kiểm tra xem khuyến mãi này có apply cho một chuyến bay cụ thể không? Hình như hơi phức tạp hóa không? Trong data đâu có mục nào tham chiếu flightID?
    return true; // Placeholder
}

bool Promotion::isApplicableForClass(const std::string& seatClass) const {
    // Kiểm tra xem khuyến mãi này có apply cho một hạng bay cụ thể không? Xem lại như trên
    return true; // Placeholder
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
    return Promotion("", "", "", "", 0.0, "", "", false); // Placeholder
}

Promotion Promotion::findPromotionById(const std::string& id) {
    // Implement logic to find a promotion by its ID
    // Hay là bỏ mục này ta? Tại vì cũng toàn tìm theo code chứ ID thì thêm nó thừa, nếu là kiểu tạo để phân biệt với hệ thống thì cũng không cần lắm? Để nghiên cứu
    return Promotion("", "", "", "", 0.0, "", "", false); // Placeholder
}

bool Promotion::validatePromotionCode(const std::string& code) {
    // Implement logic to validate a promotion code format or existence
    // Đây là gì...?
    return true; // Placeholder
}