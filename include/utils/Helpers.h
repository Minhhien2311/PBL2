#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <vector>

class Helpers {
public:
    static std::string hashPassword(const std::string& password);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static std::string generatePNR();
    // static std::string getCurrentDateTime();
    static double calculateTicketPrice(double basePrice, const std::string& seatClass, 
                                       int passengerAge, bool hasPromotion);
    static std::string formatCurrency(double amount);
    static bool isValidPhoneNumber(const std::string& phone);
    static std::string generateRandomString(int length);
};

#endif // HELPERS_H