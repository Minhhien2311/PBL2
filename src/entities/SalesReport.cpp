#include <iostream>
#include "C:/PBL2/include/entities/SalesReport.h"

SalesReport::SalesReport(const std::string& id, const std::string& agentId, const std::string& period,
 const std::string& start, const std::string& end,
 double sales, int count)
 : reportId(id), agentId(agentId), periodType(period), startDate(start), endDate(end),
 totalSales(sales), ticketCount(count) {}

// Các hàm get
std::string SalesReport::getReportId() const { return reportId; }       // get ReportId
std::string SalesReport::getAgentId() const { return agentId; }         // get AgentId
std::string SalesReport::getPeriodType() const { return periodType; }   // get PeriodType
std::string SalesReport::getStartDate() const { return startDate; }     // get StartDate
std::string SalesReport::getEndDate() const { return endDate; }         // get EndDate
double SalesReport::getTotalSales() const { return totalSales; }        // get TotalSales
int SalesReport::getTicketCount() const { return ticketCount; }         // get TicketCount

// Daily report
SalesReport SalesReport::generateDailyReport(const std::string& agentId, const std::string& date) {
 // Tại sao cần có agentID? Nếu là cần chức năng thống kê cho agent thì mình viết hàm thống kê cho Admin rồi Override sau
 return SalesReport("", agentId, "DAILY", date, date, 0.0, 0); // Placeholder
}

// WeeklyReport
SalesReport SalesReport::generateWeeklyReport(const std::string& agentId, const std::string& startDate) {
 // Tương tự như trên
 return SalesReport("", agentId, "WEEKLY", startDate, "", 0.0, 0); // Placeholder
}

SalesReport SalesReport::generateMonthlyReport(const std::string& agentId, const std::string& month) {
 // Tương tự như trên
 return SalesReport("", agentId, "MONTHLY", month, month, 0.0, 0); // Placeholder
}

std::vector<SalesReport> SalesReport::generateAgentReport(const std::string& agentId,
 const std::string& startDate,
 const std::string& endDate) {
 // Implement logic to generate reports for a specific agent within a date range
 // Đây chắc là hàm chuyên biệt để xuất ra màn hình cho Agent?
 return {}; // Placeholder
}

std::vector<SalesReport> SalesReport::loadAllReports() {
 // Implement logic to load all sales reports from a data source
 // Để coi thử, lỡ report nhiều quá thì xem có nên làm gì không
 return {}; // Placeholder
}

std::vector<SalesReport> SalesReport::findReportsByAgent(const std::string& agentId) {
 // Implement logic to find reports by agent ID
 // Thống kê theo đại lý
 return {}; // Placeholder
}

std::vector<SalesReport> SalesReport::findReportsByPeriod(const std::string& periodType) {
 // Implement logic to find reports by period type
 // Cái này là tìm lại các report đã xuất hay là sao?
 return {}; // Placeholder
}