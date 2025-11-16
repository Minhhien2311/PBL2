#include "core/AirportManager.h"
#include "core/FlightManager.h"
#include "entities/Route.h" // <-- ĐÃ THAY ĐỔI

AirportManager::AirportManager()
    : displayToIATA_(), iataToDisplay_()
{
    loadDefaultAirports();
}

void AirportManager::loadDefaultAirports()
{
    // Việt Nam (19 sân bay - đã sắp xếp alphabet)
    addAirport("Buôn Ma Thuột (BMV)", "BMV");
    addAirport("Cà Mau (CAH)", "CAH");
    addAirport("Cần Thơ (VCA)", "VCA");
    addAirport("Côn Đảo (VCS)", "VCS");
    addAirport("Điện Biên Phủ (DIN)", "DIN");
    addAirport("Đà Lạt (DLI)", "DLI");
    addAirport("Đà Nẵng (DAD)", "DAD");
    addAirport("Hà Nội (HAN)", "HAN");
    addAirport("Hải Phòng (HPH)", "HPH");
    addAirport("Huế (HUI)", "HUI");
    addAirport("Nha Trang (CXR)", "CXR");
    addAirport("Phú Quốc (PQC)", "PQC");
    addAirport("Pleiku (PXU)", "PXU");
    addAirport("Quy Nhơn (UIH)", "UIH");
    addAirport("Rạch Giá (VKG)", "VKG");
    addAirport("Thanh Hóa (THD)", "THD");
    addAirport("TP. Hồ Chí Minh (SGN)", "SGN");
    addAirport("Vinh (VII)", "VII");
    addAirport("Vũng Tàu (VTG)", "VTG");
    
    // Quốc tế (8 sân bay - đã sắp xếp alphabet)
    // addAirport("Bangkok (BKK)", "BKK");
    // addAirport("Beijing (PEK)", "PEK");
    // addAirport("Hong Kong (HKG)", "HKG");
    // addAirport("Kuala Lumpur (KUL)", "KUL");
    // addAirport("Seoul (ICN)", "ICN");
    // addAirport("Shanghai (PVG)", "PVG");
    // addAirport("Singapore (SIN)", "SIN");
    // addAirport("Tokyo (NRT)", "NRT");
}

void AirportManager::addAirport(const std::string& displayName, const std::string& iataCode)
{
    // Thêm vào cả 2 map để tra cứu 2 chiều
    displayToIATA_.insert(displayName, iataCode);
    iataToDisplay_.insert(iataCode, displayName);
}

std::string AirportManager::getIATACode(const std::string& displayName) const
{
    const std::string* code = displayToIATA_.find(displayName);
    return code ? *code : "";
}

std::string AirportManager::getDisplayName(const std::string& iataCode) const
{
    const std::string* display = iataToDisplay_.find(iataCode);
    return display ? *display : "";
}

std::vector<std::string> AirportManager::getAllDisplayNames() const
{
    // Lấy tất cả keys (display names)
    std::vector<std::string> names = displayToIATA_.getKeys();
    
    // Sắp xếp alphabet bằng Bubble Sort
    for (int i = 0; i < names.size() - 1; ++i) {
        for (int j = 0; j < names.size() - i - 1; ++j) {
            if (names[j] > names[j + 1]) {
                std::string temp = names[j];
                names[j] = names[j + 1];
                names[j + 1] = temp;
            }
        }
    }
    
    return names;
}

void AirportManager::loadAirportsFromFlights(FlightManager* flightManager)
{
    if (!flightManager) return;
    
    // Đổi getAllFlights (trả về Flight mới) sang getAllRoutes (trả về Route)
    const std::vector<Route*>& routes = flightManager->getAllRoutes(); // <-- ĐÃ THAY ĐỔI
    
    for (int i = 0; i < routes.size(); ++i) { // <-- ĐÃ THAY ĐỔI
        Route* route = routes[i]; // <-- ĐÃ THAY ĐỔI
        if (!route) continue; // <-- ĐÃ THAY ĐỔI
        
        // Các hàm getter này vẫn tồn tại trên Route (Flight cũ)
        std::string depIATA = route->getDepartureAirport(); // <-- ĐÃ THAY ĐỔI
        std::string arrIATA = route->getArrivalAirport(); // <-- ĐÃ THAY ĐỔI
        
        // Nếu chưa có trong danh sách, tạo display name mặc định: "HAN (HAN)"
        if (!hasAirport(depIATA)) {
            addAirport(depIATA + " (" + depIATA + ")", depIATA);
        }
        if (!hasAirport(arrIATA)) {
            addAirport(arrIATA + " (" + arrIATA + ")", arrIATA);
        }
    }
}

bool AirportManager::hasAirport(const std::string& iataCode) const
{
    return iataToDisplay_.contains(iataCode);
}

int AirportManager::getAirportCount() const
{
    return iataToDisplay_.size();
}