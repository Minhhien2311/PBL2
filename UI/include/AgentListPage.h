#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

class AccountManager;
class QTableView;
class QStandardItemModel;

class AgentListPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit AgentListPage(AccountManager* am, QWidget* parent = nullptr);

signals:
    void agentAdded();

private slots:
    void onAddAgentClicked();
    void onViewDetailClicked();
    void refreshPage();
    void onSelectionChanged();

private:
    void setupUI();
    void setupModel();
    void setupConnections();
    void loadData();
    void showAddAgentDialog();
    void showAgentDetailsDialog();
    void displayAgent(const class AccountAgent* agent);
    
    AccountManager* accountManager_;
    
    // UI Components
    QTableView* tableView_;
    QStandardItemModel* model_;
    QPushButton* viewDetailsBtn_;
    QPushButton* addAgentBtn_;
    QPushButton* refreshButton_;
};