#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>

class QTableView;
class QStandardItemModel;
class QLabel;
class AccountManager;
class ReportManager;

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(AccountManager* accManager, ReportManager* reportManager, QWidget *parent = nullptr);

public slots:
    void refreshData();

private:
    void setupUi();
    void setupModel();

    AccountManager* accountManager_;
    ReportManager* reportManager_;
    
    QWidget* createStatCard(const QString& title, const QString& defaultValue, QLabel*& outValueLabel);

    // --- UI Components ---
    QLabel *salesCountLabel_;
    QLabel *salesTotalLabel_;
    QLabel *cancelCountLabel_;
    QLabel *doVeLabel_; 

    QTableView *flightsTable_;
    QStandardItemModel *flightsModel_;
};

#endif // DASHBOARDPAGE_H

