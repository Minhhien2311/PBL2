#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <string>
#include "../entities/Account.h"

class MenuManager {
public:
    static void showMainMenu();
    static void showAdminMenu();
    static void showAgentMenu();
    static void showLoginMenu();
    static void showFlightManagementMenu();
    static void showBookingManagementMenu();
    static void showReportMenu();
    static void showUserManagementMenu();
    
    static void clearScreen();
    static void showBanner();
    static void showErrorMessage(const std::string& message);
    static void showSuccessMessage(const std::string& message);
    static void showInfoMessage(const std::string& message);
    
    static int getChoice(int min, int max);
    static std::string getInput(const std::string& prompt);
    static double getDoubleInput(const std::string& prompt);
    static int getIntegerInput(const std::string& prompt);
};

#endif // MENUMANAGER_H