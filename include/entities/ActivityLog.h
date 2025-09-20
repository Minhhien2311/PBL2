// Nhật ký hoạt động
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

public:
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
    
    // Logging methods
    static void logLogin(const std::string& userId, const std::string& ip);
    static void logBookingCreation(const std::string& userId, const std::string& pnr);
    static void logTicketIssuance(const std::string& userId, const std::string& ticketNumber);
    static void logCancellation(const std::string& userId, const std::string& pnr);
    
    static std::vector<ActivityLog> loadAllLogs();
    static std::vector<ActivityLog> findLogsByUser(const std::string& userId);
    static std::vector<ActivityLog> findLogsByDate(const std::string& date);
};

#endif // ACTIVITYLOG_H