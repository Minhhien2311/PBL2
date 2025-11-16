#include "core/AirportManager.h"
#include "core/FlightManager.h"
#include "entities/Route.h"
#include <fstream> // Required for std::ifstream
#include <stdexcept> // Required for std::runtime_error

AirportManager::AirportManager(const std::string& airportsFilePath)
    : displayToIATA_(), iataToDisplay_()
{
    loadAirportsFromFile(airportsFilePath);
}
void AirportManager::loadAirportsFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open airports file: " + filePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Bỏ qua comment
        
        size_t pos = line.find('|');
        if (pos == std::string::npos) continue;
        
        std::string iataCode = line.substr(0, pos);
        std::string cityName = line.substr(pos + 1);
        
        // Loại bỏ khoảng trắng thừa
        iataCode.erase(0, iataCode.find_first_not_of(" \t\r\n"));
        iataCode.erase(iataCode.find_last_not_of(" \t\r\n") + 1);
        cityName.erase(0, cityName.find_first_not_of(" \t\r\n"));
        cityName.erase(cityName.find_last_not_of(" \t\r\n") + 1);
        
        addAirport(cityName, iataCode);
    }
    
    file.close();
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

void AirportManager::loadAirportsFromRoutes(FlightManager* flightManager)
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
        
        // Nếu chưa có trong danh sách, tạo display name mặc định: "HAN"
        if (!hasAirport(depIATA)) {
            addAirport(depIATA, depIATA);
        }
        if (!hasAirport(arrIATA)) {
            addAirport(arrIATA, arrIATA);
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