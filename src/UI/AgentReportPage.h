#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QDateEdit>
#include <QLabel>
#include <string>

class ReportManager;

class AgentReportPage : public QWidget {
    Q_OBJECT
public:
    explicit AgentReportPage(const std::string& agentId,
                             ReportManager* rm,
                             QWidget* parent = nullptr);

private:
    std::string agentId_;
    ReportManager* reportManager_;

    QTableWidget* table_;
    QDateEdit* startDate_;
    QDateEdit* endDate_;
    QPushButton* refreshBtn_;

    void setupUI();
    void loadReport();

private slots:
    void onRefreshClicked();
};
