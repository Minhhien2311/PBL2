#pragma once

#include <string>
#include <vector>

class Helpers {
public:
    // --- Định dạng dữ liệu ---
    static std::string formatCurrency(double amount);

    // --- Đọc dữ liệu bổ trợ ---
    static std::vector<std::string> loadAirlinesFromFile(const std::string& filePath);
};