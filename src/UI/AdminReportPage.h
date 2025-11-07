#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QDateEdit>
#include <QLabel>

class AccountManager;
class BookingManager;
class ReportManager;

class AdminReportPage : public QWidget {
    Q_OBJECT
public:
    explicit AdminReportPage(AccountManager* am,
                             BookingManager* bm,
                             ReportManager* rm,
                             QWidget* parent = nullptr);

private:
    AccountManager* accountManager_;
    BookingManager* bookingManager_;
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
