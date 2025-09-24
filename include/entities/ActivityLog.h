#ifndef ACTIVITYLOG_H
#define ACTIVITYLOG_H

#include <string>
#include <vector>

class ActivityLog {
private:
    std::string logId;
    std::string userId;
    std::string activityType;
    std::string timestamp;
    std::string description;
    std::string ipAddress;
    std::string affectedEntity;
    std::string entityId;

    // Private method to save log to file
    static void saveLogToFile(const ActivityLog& log);

public:
    // Constructor
    ActivityLog(const std::string& id, const std::string& userId, const std::string& type,
                const std::string& timestamp, const std::string& desc,
                const std::string& ip = "", const std::string& entity = "",
                const std::string& eId = "");
    
    // Getter methods
    std::string getLogId() const;
    std::string getUserId() const;
    std::string getActivityType() const;
    std::string getTimestamp() const;
    std::string getDescription() const;
    std::string getIpAddress() const;
    std::string getAffectedEntity() const;
    std::string getEntityId() const;
    
    // Static logging methods
    static void logLogin(const std::string& userId, const std::string& ip);
    static void logBookingCreation(const std::string& userId, const std::string& pnr);
    static void logTicketIssuance(const std::string& userId, const std::string& ticketNumber);
    static void logCancellation(const std::string& userId, const std::string& pnr);
    static void logFlightSearch(const std::string& userId, const std::string& searchCriteria);
    static void logLogout(const std::string& userId, const std::string& ip);
    static void logPasswordChange(const std::string& userId);
    static void logFlightManagement(const std::string& userId, const std::string& action, const std::string& flightId);
    static void logReportGeneration(const std::string& userId, const std::string& reportType);
    static void logPromotionManagement(const std::string& userId, const std::string& action, const std::string& promoCode);
    
    // Static query methods
    static std::vector<ActivityLog> loadAllLogs();
    static std::vector<ActivityLog> findLogsByUser(const std::string& userId);
    static std::vector<ActivityLog> findLogsByDate(const std::string& date);
    static std::vector<ActivityLog> findLogsByType(const std::string& activityType);
    static std::vector<ActivityLog> getRecentActivities(int count);
    
    // Display methods
    void displayInfo() const;
    static void displayLogsTable(const std::vector<ActivityLog>& logs);
};

#endif // ACTIVITYLOG_H