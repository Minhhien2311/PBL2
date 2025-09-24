#include "ActivityLog.h"
#include "../utils/DatabaseManager.h"
#include "../utils/Helpers.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Constructor implementation
ActivityLog::ActivityLog(const std::string& id, const std::string& userId, const std::string& type,
                         const std::string& timestamp, const std::string& desc,
                         const std::string& ip, const std::string& entity, const std::string& eId)
    : logId(id), userId(userId), activityType(type), timestamp(timestamp),
      description(desc), ipAddress(ip), affectedEntity(entity), entityId(eId) {}

// Getter methods implementation
std::string ActivityLog::getLogId() const { return logId; }
std::string ActivityLog::getUserId() const { return userId; }
std::string ActivityLog::getActivityType() const { return activityType; }
std::string ActivityLog::getTimestamp() const { return timestamp; }
std::string ActivityLog::getDescription() const { return description; }
std::string ActivityLog::getIpAddress() const { return ipAddress; }
std::string ActivityLog::getAffectedEntity() const { return affectedEntity; }
std::string ActivityLog::getEntityId() const { return entityId; }

// Static method to log user login
void ActivityLog::logLogin(const std::string& userId, const std::string& ip) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = "User logged in successfully";
    
    ActivityLog log(logId, userId, "LOGIN", timestamp, description, ip, "USER", userId);
    saveLogToFile(log);
}

// Static method to log booking creation
void ActivityLog::logBookingCreation(const std::string& userId, const std::string& pnr) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = "Created booking with PNR: " + pnr;
    
    ActivityLog log(logId, userId, "CREATE_BOOKING", timestamp, description, "", "BOOKING", pnr);
    saveLogToFile(log);
}

// Static method to log ticket issuance
void ActivityLog::logTicketIssuance(const std::string& userId, const std::string& ticketNumber) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = "Issued ticket: " + ticketNumber;
    
    ActivityLog log(logId, userId, "ISSUE_TICKET", timestamp, description, "", "TICKET", ticketNumber);
    saveLogToFile(log);
}

// Static method to log cancellation
void ActivityLog::logCancellation(const std::string& userId, const std::string& pnr) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = "Cancelled booking with PNR: " + pnr;
    
    ActivityLog log(logId, userId, "CANCEL_BOOKING", timestamp, description, "", "BOOKING", pnr);
    saveLogToFile(log);
}

// Static method to log flight search
void ActivityLog::logFlightSearch(const std::string& userId, const std::string& searchCriteria) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = "Searched for flights: " + searchCriteria;
    
    ActivityLog log(logId, userId, "SEARCH_FLIGHT", timestamp, description, "", "FLIGHT", "");
    saveLogToFile(log);
}

// Static method to log user logout
void ActivityLog::logLogout(const std::string& userId, const std::string& ip) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = "User logged out";
    
    ActivityLog log(logId, userId, "LOGOUT", timestamp, description, ip, "USER", userId);
    saveLogToFile(log);
}

// Static method to log password change
void ActivityLog::logPasswordChange(const std::string& userId) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = "User changed password";
    
    ActivityLog log(logId, userId, "CHANGE_PASSWORD", timestamp, description, "", "USER", userId);
    saveLogToFile(log);
}

// Static method to log flight management actions
void ActivityLog::logFlightManagement(const std::string& userId, const std::string& action, const std::string& flightId) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = action + " for flight: " + flightId;
    
    ActivityLog log(logId, userId, "MANAGE_FLIGHT", timestamp, description, "", "FLIGHT", flightId);
    saveLogToFile(log);
}

// Static method to log report generation
void ActivityLog::logReportGeneration(const std::string& userId, const std::string& reportType) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = "Generated report: " + reportType;
    
    ActivityLog log(logId, userId, "GENERATE_REPORT", timestamp, description, "", "REPORT", reportType);
    saveLogToFile(log);
}

// Static method to log promotion management
void ActivityLog::logPromotionManagement(const std::string& userId, const std::string& action, const std::string& promoCode) {
    std::string logId = std::to_string(DatabaseManager::getNextId("activity_logs.txt"));
    std::string timestamp = Helpers::getCurrentDateTime();
    std::string description = action + " promotion: " + promoCode;
    
    ActivityLog log(logId, userId, "MANAGE_PROMOTION", timestamp, description, "", "PROMOTION", promoCode);
    saveLogToFile(log);
}

// Private method to save log to file
void ActivityLog::saveLogToFile(const ActivityLog& log) {
    std::vector<std::string> row;
    row.push_back(log.getLogId());
    row.push_back(log.getUserId());
    row.push_back(log.getActivityType());
    row.push_back(log.getTimestamp());
    row.push_back(log.getDescription());
    row.push_back(log.getIpAddress());
    row.push_back(log.getAffectedEntity());
    row.push_back(log.getEntityId());
    
    DatabaseManager::appendData("activity_logs.txt", row);
}

// Static method to load all activity logs
std::vector<ActivityLog> ActivityLog::loadAllLogs() {
    std::vector<ActivityLog> logs;
    auto data = DatabaseManager::readData("activity_logs.txt");
    
    // Skip header row
    for (size_t i = 1; i < data.size(); i++) {
        if (data[i].size() >= 8) {
            ActivityLog log(data[i][0], data[i][1], data[i][2], data[i][3], 
                           data[i][4], data[i][5], data[i][6], data[i][7]);
            logs.push_back(log);
        }
    }
    
    return logs;
}

// Static method to find logs by user ID
std::vector<ActivityLog> ActivityLog::findLogsByUser(const std::string& userId) {
    std::vector<ActivityLog> allLogs = loadAllLogs();
    std::vector<ActivityLog> userLogs;
    
    for (const auto& log : allLogs) {
        if (log.getUserId() == userId) {
            userLogs.push_back(log);
        }
    }
    
    return userLogs;
}

// Static method to find logs by date
std::vector<ActivityLog> ActivityLog::findLogsByDate(const std::string& date) {
    std::vector<ActivityLog> allLogs = loadAllLogs();
    std::vector<ActivityLog> dateLogs;
    
    for (const auto& log : allLogs) {
        if (log.getTimestamp().substr(0, 10) == date) { // Compare YYYY-MM-DD part
            dateLogs.push_back(log);
        }
    }
    
    return dateLogs;
}

// Static method to find logs by activity type
std::vector<ActivityLog> ActivityLog::findLogsByType(const std::string& activityType) {
    std::vector<ActivityLog> allLogs = loadAllLogs();
    std::vector<ActivityLog> typeLogs;
    
    for (const auto& log : allLogs) {
        if (log.getActivityType() == activityType) {
            typeLogs.push_back(log);
        }
    }
    
    return typeLogs;
}

// Static method to get recent activities (last N records)
std::vector<ActivityLog> ActivityLog::getRecentActivities(int count) {
    std::vector<ActivityLog> allLogs = loadAllLogs();
    
    // Sort by timestamp descending (most recent first)
    std::sort(allLogs.begin(), allLogs.end(), [](const ActivityLog& a, const ActivityLog& b) {
        return a.getTimestamp() > b.getTimestamp();
    });
    
    // Return the first 'count' records
    if (count > 0 && count < static_cast<int>(allLogs.size())) {
        return std::vector<ActivityLog>(allLogs.begin(), allLogs.begin() + count);
    }
    
    return allLogs;
}

// Method to display log information
void ActivityLog::displayInfo() const {
    std::cout << "Log ID: " << this->logId << std::endl;
    std::cout << "User ID: " << this->userId << std::endl;
    std::cout << "Activity Type: " << this->activityType << std::endl;
    std::cout << "Timestamp: " << this->timestamp << std::endl;
    std::cout << "Description: " << this->description << std::endl;
    
    if (!this->ipAddress.empty()) {
        std::cout << "IP Address: " << this->ipAddress << std::endl;
    }
    
    if (!this->affectedEntity.empty()) {
        std::cout << "Affected Entity: " << this->affectedEntity << std::endl;
    }
    
    if (!this->entityId.empty()) {
        std::cout << "Entity ID: " << this->entityId << std::endl;
    }
    
    std::cout << "----------------------------------------" << std::endl;
}

// Static method to display logs in a formatted table
void ActivityLog::displayLogsTable(const std::vector<ActivityLog>& logs) {
    if (logs.empty()) {
        std::cout << "No activity logs found." << std::endl;
        return;
    }
    
    std::cout << "======================================================================================================================" << std::endl;
    std::cout << "| Log ID | User ID | Activity Type       | Timestamp           | Description" << std::endl;
    std::cout << "======================================================================================================================" << std::endl;
    
    for (const auto& log : logs) {
        std::string logId = log.getLogId().substr(0, 6);
        std::string userId = log.getUserId().substr(0, 6);
        std::string activityType = log.getActivityType().substr(0, 18);
        std::string timestamp = log.getTimestamp().substr(0, 16);
        std::string description = log.getDescription().substr(0, 40);
        
        if (log.getDescription().length() > 40) {
            description += "...";
        }
        
        printf("| %-6s | %-6s | %-18s | %-16s | %-40s |\n", 
               logId.c_str(), userId.c_str(), activityType.c_str(), 
               timestamp.c_str(), description.c_str());
    }
    
    std::cout << "======================================================================================================================" << std::endl;
}