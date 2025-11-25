#include "DashboardPage.h"
#include "core/AccountManager.h"
#include "core/ReportManager.h"
#include "entities/Account.h"
#include "PageRefresher.h"
#include "BoldItemDelegate.h" // Cần include class này (đã có trong project của bạn)

#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QHeaderView> 
#include <QList>

// Helper format tiền tệ (Copy từ SearchBookPage)
namespace {
    QString formatVietnamCurrency(long long price) {
        QString priceStr = QString::number(price);
        QString result;
        int count = 0;
        
        for (int i = priceStr.length() - 1; i >= 0; --i) {
            if (count == 3) {
                result.prepend('.');
                count = 0;
            }
            result.prepend(priceStr[i]);
            count++;
        }
        
        return result + " VNĐ";
    }
}

DashboardPage::DashboardPage(AccountManager* accManager, ReportManager* reportManager, QWidget *parent)
    : QWidget(parent), accountManager_(accManager), reportManager_(reportManager)
{
    Q_ASSERT(accountManager_ != nullptr);
    Q_ASSERT(reportManager_ != nullptr);
    
    setupUi();
    setupModel();
    refreshData(); 
}

void DashboardPage::setupUi()
{
    // --- Style chung ---
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.SectionTitle { color: #123B7A; font-weight:700; font-size: 16px; }"
        "QFrame#StatCard { background: white; border:1px solid #0E3B7C; }"
        "DashboardTitle { font-size: 18px; }"
        "TableTitle { font-size: 18px; }"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ====== Thanh top: Refresh + Stat Cards ======
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topBarLayout = new QVBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 20, 24, 16);
    topBarLayout->setSpacing(16);
    topBarLayout->addStretch();

    // Nút refresh
    QPushButton* refreshBtn = PageRefresher::createRefreshButton(topBar);
    refreshBtn->setIcon(QIcon("C:/PBL2/assets/icons/reload.png")); 
    refreshBtn->setIconSize(QSize(14, 14));
    refreshBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #133e87;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   border: none;"
        "   text-align: left;"
        "   padding: 0px;"
        "}"
        "QPushButton:hover {"
        "   background: transparent;"
        "   text-decoration: underline;"
        "}"
    );
    refreshBtn->setCursor(Qt::PointingHandCursor);
    
    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->addWidget(refreshBtn);
    titleRow->addStretch();
    topBarLayout->addLayout(titleRow);
    connect(refreshBtn, &QPushButton::clicked, this, &DashboardPage::refreshPage);

    // 4 Stat cards
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);
    statsLayout->addWidget(createStatCard("Tổng vé đã bán", "0", salesCountLabel_));
    statsLayout->addWidget(createStatCard("Tổng tiền bán vé", "0", salesTotalLabel_));
    statsLayout->addWidget(createStatCard("Vé đã hủy", "0", cancelCountLabel_));
    statsLayout->addWidget(createStatCard("Vé đã đổi", "0", doVeLabel_));

    topBarLayout->addLayout(statsLayout);
    mainLayout->addWidget(topBar);

    // ====== Tiêu đề bảng ======
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *tableHeaderLayout = new QHBoxLayout(tableHeader);
    tableHeaderLayout->setContentsMargins(24, 0, 24, 0);
    tableHeaderLayout->setSpacing(0);

    QLabel *tableTitle = new QLabel("Các chuyến bay trong ngày", tableHeader);
    tableTitle->setObjectName("TableTitle");
    tableTitle->setProperty("class", "SectionTitle");
    tableHeaderLayout->addWidget(tableTitle);
    tableHeaderLayout->addStretch();

    mainLayout->addWidget(tableHeader);

    // ====== Bảng (Cập nhật style giống SearchBookPage) ======
    QWidget *tableBox = new QWidget(this);
    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 24);

    flightsTable_ = new QTableView(this);
    
    // Style sạch sẽ, bỏ border, nền trắng
    flightsTable_->setStyleSheet(
        "QTableView { background:white; border:0px solid #133e87; }"
    );
    
    // Sử dụng Delegate để render text đẹp hơn (như bên SearchBook)
    flightsTable_->setItemDelegate(new BoldItemDelegate(this));

    // Cấu hình hành vi bảng
    flightsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    flightsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    flightsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Tắt header dọc (số thứ tự mặc định của Qt) -> ta sẽ dùng cột STT riêng
    flightsTable_->verticalHeader()->setVisible(false);
    
    // Tắt lưới (Grid)
    flightsTable_->setShowGrid(false);
    
    // Bật màu xen kẽ
    flightsTable_->setAlternatingRowColors(true);

    // Tắt scrollbar ngang
    flightsTable_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    flightsTable_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    tblWrap->addWidget(flightsTable_);
    mainLayout->addWidget(tableBox, 1);
}

QWidget* DashboardPage::createStatCard(const QString& title, const QString& defaultValue, QLabel*& outValueLabel)
{
    QFrame *card = new QFrame(this);
    card->setObjectName("StatCard");
    card->setFixedHeight(90);
    card->setStyleSheet(
        "QFrame#StatCard {"
        "   background: white;"
        "   border: 1px solid #0E3B7C;"
        "}"
    );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0,0,0,0);
    cardLayout->setSpacing(0);

    QWidget *head = new QWidget(card);
    head->setStyleSheet("background:#0E3B7C; color:white;");
    head->setFixedHeight(28);
    QHBoxLayout *headLayout = new QHBoxLayout(head);
    headLayout->setContentsMargins(10, 0, 10, 0);
    QLabel *titleLabel = new QLabel(title, head);
    titleLabel->setStyleSheet("font-size:13px; font-weight:600;");
    titleLabel->setAlignment(Qt::AlignCenter); 
    headLayout->addWidget(titleLabel);

    QWidget *body = new QWidget(card);
    QHBoxLayout *bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(10, 6, 10, 6);

    outValueLabel = new QLabel(defaultValue, body);
    outValueLabel->setStyleSheet("color:#0E3B7C; font-size:20px; font-weight:700;");
    outValueLabel->setAlignment(Qt::AlignCenter);
    bodyLayout->addWidget(outValueLabel, 0, Qt::AlignCenter);

    cardLayout->addWidget(head);
    cardLayout->addWidget(body);
    return card;
}

void DashboardPage::setupModel()
{
    // Định nghĩa cột: STT | Mã chuyến | Tuyến | Giờ đi | Khách (Booked/Total) | Doanh thu dự kiến
    flightsModel_ = new QStandardItemModel(0, 6, this);
    flightsModel_->setHorizontalHeaderLabels({
        "STT", "Mã chuyến", "Tuyến bay", "Giờ khởi hành", 
        "Khách đặt", "Doanh thu (Est)"
    });
    
    flightsTable_->setModel(flightsModel_);
    QHeaderView *h = flightsTable_->horizontalHeader();

    // Mặc định co theo nội dung
    h->setSectionResizeMode(QHeaderView::Stretch);
    
    // Cột STT cố định nhỏ
    h->setSectionResizeMode(0, QHeaderView::Fixed);
    flightsTable_->setColumnWidth(0, 50);
}

void DashboardPage::refreshData()
{
    // 1. Cập nhật Stat Cards
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        salesCountLabel_->setText("0");
        salesTotalLabel_->setText("0 VNĐ");
        cancelCountLabel_->setText("0");
        doVeLabel_->setText("0");
        return;
    }
    
    std::string agentId = currentUser->getId();
    int dailySales = reportManager_->getDailyTicketsSold(agentId);
    double dailyRevenue = reportManager_->getDailyRevenue(agentId);
    int dailyCancellations = reportManager_->getDailyCancellations(agentId);
    
    salesCountLabel_->setText(QString::number(dailySales));
    salesTotalLabel_->setText(formatVietnamCurrency(static_cast<long long>(dailyRevenue))); // Dùng hàm helper
    cancelCountLabel_->setText(QString::number(dailyCancellations));
    // doVeLabel_->setText(QString::number(dailyChanges)); // Chưa có API

    // 2. Cập nhật Bảng (Mock data nhưng format đẹp)
    flightsModel_->removeRows(0, flightsModel_->rowCount());
    
    // Mock data structure
    struct MockFlight {
        QString id;
        QString route;
        QString time;
        QString seats;     // "Booked/Total"
        long long revenue; // Mock revenue for this flight
    };

    QList<MockFlight> mockData = {
        {"VN123", "SGN - HAN", "08:00", "100/150", 150000000},
        {"VJ456", "DAD - SGN", "10:30", "120/180", 180000000},
        {"QH789", "HAN - DAD", "13:15", "80/120", 95000000},
        {"VN234", "SGN - DAD", "15:45", "140/160", 210000000},
        {"VJ567", "HAN - SGN", "18:00", "170/200", 255000000},
        {"QH890", "DAD - HAN", "20:30", "90/120", 110000000}
    };

    int stt = 1;
    for (const auto& f : mockData) {
        QList<QStandardItem *> row;
        row << new QStandardItem(QString::number(stt++))
            << new QStandardItem(f.id)
            << new QStandardItem(f.route)
            << new QStandardItem(f.time)
            << new QStandardItem(f.seats)
            << new QStandardItem(formatVietnamCurrency(f.revenue));

        // Căn giữa tất cả các cột
        for (QStandardItem *item : row) {
            item->setTextAlignment(Qt::AlignCenter);
        }
        flightsModel_->appendRow(row);
    }
}

void DashboardPage::refreshPage() {
    PageRefresher::executeRefresh([this]() {
        refreshData();
    });
}