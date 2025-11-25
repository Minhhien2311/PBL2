#include "utils/Helpers.h"
#include <fstream>
#include <string>
#include <sstream>

std::string Helpers::formatCurrency(double amount) {
    std::string s = std::to_string(amount);
    int decimalPoint = s.find('.');
    if (decimalPoint == std::string::npos) {
        decimalPoint = s.length();
    }
    
    std::string formatted;
    int count = 0;
    for (int i = decimalPoint - 1; i >= 0; --i) {
        formatted.insert(0, 1, s[i]);
        count++;
        if (count % 3 == 0 && i > 0) {
            formatted.insert(0, ",");
        }
    }
    
    if (decimalPoint < s.length()) {
        formatted += s.substr(decimalPoint);
    }
    
    return formatted;
}

std::vector<std::string> Helpers::loadAirlinesFromFile(const std::string& filePath) {
    std::vector<std::string> airlines;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        return airlines; // Trả về vector rỗng nếu không đọc được file
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Bỏ qua dòng trống và comment
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Loại bỏ khoảng trắng đầu/cuối
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        
        if (start != std::string::npos && end != std::string::npos) {
            airlines.push_back(line.substr(start, end - start + 1));
        }
    }
    
    file.close();
    return airlines;
}