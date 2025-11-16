#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <vector>

class Helpers {
public:
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    
    static std::string formatCurrency(double amount);

    static std::vector<std::string> loadAirlinesFromFile(const std::string& filePath);
};

#endif // HELPERS_H