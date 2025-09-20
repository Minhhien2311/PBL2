// Báo cáo doanh thu
#ifndef SALESREPORT_H
#define SALESREPORT_H

#include <string>
#include <vector>

class SalesReport {
private:
    std::string reportId;
    std::string agentId;
    std::string periodType; // "DAILY", "WEEKLY", "MONTHLY"
    std::string startDate;
    std::string endDate;
    double totalSales;
    int ticketCount;

public:
    SalesReport(const std::string& id, const std::string& agentId, const std::string& period,
                const std::string& start, const std::string& end,
                double sales, int count);
    
    // Getter methods
    std::string getReportId() const;
    std::string getAgentId() const;
    std::string getPeriodType() const;
    std::string getStartDate() const;
    std::string getEndDate() const;
    double getTotalSales() const;
    int getTicketCount() const;
    
    // Reporting methods
    static SalesReport generateDailyReport(const std::string& agentId, const std::string& date);
    static SalesReport generateWeeklyReport(const std::string& agentId, const std::string& startDate);
    static SalesReport generateMonthlyReport(const std::string& agentId, const std::string& month);
    static std::vector<SalesReport> generateAgentReport(const std::string& agentId, 
                                                       const std::string& startDate,
                                                       const std::string& endDate);
    
    static std::vector<SalesReport> loadAllReports();
    static std::vector<SalesReport> findReportsByAgent(const std::string& agentId);
    static std::vector<SalesReport> findReportsByPeriod(const std::string& periodType);
};

#endif // SALESREPORT_H