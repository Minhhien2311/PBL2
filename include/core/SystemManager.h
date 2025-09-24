#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <string>
#include "../entities/Account.h"
#include "../entities/Flight.h"
#include "../entities/Booking.h"

class SystemManager {
private:
    static SystemManager* instance;
    Account* currentUser;
    
    SystemManager(); // Private constructor for singleton

public:
    static SystemManager* getInstance();
    
    // User management
    bool initializeSystem();
    bool shutdownSystem();
    bool backupSystem();
    bool restoreSystem(const std::string& backupPath);
    
    // Session management
    bool startSession(Account* account);
    bool endSession();
    Account* getCurrentSessionUser();
    
    // System status
    std::string getSystemStatus() const;
    int getActiveUsersCount() const;
    int getTotalBookingsToday() const;
    double getTotalRevenueToday() const;
    
    // Notification system
    static void sendNotification(const std::string& userId, const std::string& message);
    static void broadcastNotification(const std::string& message);
};

#endif // SYSTEMMANAGER_H