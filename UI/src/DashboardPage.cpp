#include "DashboardPage.h"

#include "BoldItemDelegate.h"
#include "PageRefresher.h"
#include "core/AccountManager.h"
#include "core/ReportManager.h"
#include "core/FlightManager.h"
#include "core/BookingManager.h"
#include "core/AirportManager.h"
#include "entities/Account.h"
#include "entities/Flight.h"
#include "entities/Booking.h"
#include "utils/DateTime.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QList>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

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

DashboardPage::DashboardPage(AccountManager* accManager, ReportManager* reportManager, 
                             FlightManager* flightManager, BookingManager* bookingManager,
                             AirportManager* airportManager, QWidget *parent)
    : QWidget(parent), accountManager_(accManager), reportManager_(reportManager),
      flightManager_(flightManager), bookingManager_(bookingManager),
      airportManager_(airportManager)
{
    Q_ASSERT(accountManager_ != nullptr);
    Q_ASSERT(reportManager_ != nullptr);
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);
    Q_ASSERT(airportManager_ != nullptr);
    
    setupUi();
    setupModel();
    refreshData(); 
}

void DashboardPage::setupUi()
{
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

    // ========== TOP BAR ==========
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topBarLayout = new QVBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 20, 24, 16);
    topBarLayout->setSpacing(16);
    topBarLayout->addStretch();

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

    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);
    statsLayout->addWidget(createStatCard("Tổng vé đã bán", "0", salesCountLabel_));
    statsLayout->addWidget(createStatCard("Tổng tiền bán vé", "0", salesTotalLabel_));
    statsLayout->addWidget(createStatCard("Vé đã hủy", "0", cancelCountLabel_));
    statsLayout->addWidget(createStatCard("Vé đã đổi", "0", doVeLabel_));

    topBarLayout->addLayout(statsLayout);
    mainLayout->addWidget(topBar);

    // ========== TABLE TITLE ==========
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

    // ========== TABLE ==========
    QWidget *tableBox = new QWidget(this);
    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 24);

    flightsTable_ = new QTableView(this);
    
    flightsTable_->setStyleSheet(
        "QTableView { background:white; border:0px solid #133e87; }"
    );
    
    flightsTable_->setItemDelegate(new BoldItemDelegate(this));

    flightsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    flightsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    flightsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    flightsTable_->verticalHeader()->setVisible(false);
    
    flightsTable_->setShowGrid(false);
    
    flightsTable_->setAlternatingRowColors(true);

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
    flightsModel_ = new QStandardItemModel(0, 6, this);
    flightsModel_->setHorizontalHeaderLabels({
        "STT", "Mã chuyến", "Tuyến bay", "Giờ khởi hành", 
        "Khách đặt", "Doanh thu"
    });
    
    flightsTable_->setModel(flightsModel_);
    QHeaderView *h = flightsTable_->horizontalHeader();

    h->setSectionResizeMode(QHeaderView::Stretch);
    
    h->setSectionResizeMode(0, QHeaderView::Fixed);
    flightsTable_->setColumnWidth(0, 50);
}

void DashboardPage::refreshData()
{
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        salesCountLabel_->setText("0");
        salesTotalLabel_->setText("0 VNĐ");
        cancelCountLabel_->setText("0");
        doVeLabel_->setText("0");
        return;
    }
    
    // Lấy ngày hôm nay (format DD/MM/YYYY)
    auto now = utils::DateTime::nowUtc();
    std::string today = utils::DateTime::formatDmY(now);
    
    int dailySales = 0;
    double dailyRevenue = 0.0;
    int dailyCancellations = 0;
    
    // Phân biệt Admin và Agent
    if (currentUser->getRole() == Role::Admin) {
        // Admin: Hiển thị thống kê TRONG NGÀY toàn hệ thống
        dailySales = reportManager_->getTicketsSoldInRange(today, today);
        dailyRevenue = reportManager_->getRevenueInRange(today, today);
        dailyCancellations = reportManager_->getCancelledTicketsInRange(today, today);
    } else {
        // Agent: Hiển thị thống kê TRONG NGÀY cá nhân
        std::string agentId = currentUser->getId();
        dailySales = reportManager_->getDailyTicketsSold(agentId);
        dailyRevenue = reportManager_->getDailyRevenue(agentId);
        dailyCancellations = reportManager_->getDailyCancellations(agentId);
    }
    
    salesCountLabel_->setText(QString::number(dailySales));
    salesTotalLabel_->setText(formatVietnamCurrency(static_cast<long long>(dailyRevenue)));
    cancelCountLabel_->setText(QString::number(dailyCancellations));

    flightsModel_->removeRows(0, flightsModel_->rowCount());
    
    // Lấy tất cả chuyến bay
    std::vector<Flight*> allFlights = flightManager_->getAllFlights();
    
    // Lọc chuyến bay trong ngày hôm nay
    std::vector<Flight*> todayFlights;
    for (Flight* flight : allFlights) {
        if (flight) {
            if (flight->getDepartureDate() == today) {
                todayFlights.push_back(flight);
            }
        }
    }
    
    // Lấy tất cả bookings để tính số khách và doanh thu
    const std::vector<Booking*>& allBookings = bookingManager_->getAllBookings();
    
    int stt = 1;
    for (Flight* flight : todayFlights) {
        // Đếm số booking cho chuyến bay này
        int bookedCount = 0;
        long long flightRevenue = 0;
        
        for (Booking* booking : allBookings) {
            if (booking && booking->getFlightId() == flight->getFlightId()) {
                if (booking->getStatus() == BookingStatus::Issued) {
                    bookedCount++;
                    flightRevenue += booking->getBaseFare();
                }
            }
        }
        
        // Tạo chuỗi tuyến bay từ routeId (VD: SGN-HAN -> Hồ Chí Minh - Hà Nội)
        std::string routeId = flight->getRouteId();
        std::string routeInfo = routeId;
        
        size_t pos = routeId.find('-');
        if (pos != std::string::npos) {
            std::string fromIATA = routeId.substr(0, pos);
            std::string toIATA = routeId.substr(pos + 1);
            std::string fromName = airportManager_->getDisplayName(fromIATA);
            std::string toName = airportManager_->getDisplayName(toIATA);
            
            if (!fromName.empty() && !toName.empty()) {
                routeInfo = fromName + " → " + toName;
            } else {
                routeInfo = fromIATA + " → " + toIATA;
            }
        }
        
        QList<QStandardItem *> row;
        row << new QStandardItem(QString::number(stt++))
            << new QStandardItem(QString::fromStdString(flight->getFlightNumber()))
            << new QStandardItem(QString::fromStdString(routeInfo))
            << new QStandardItem(QString::fromStdString(flight->getDepartureTime()))
            << new QStandardItem(QString("%1/%2").arg(bookedCount).arg(flight->getTotalCapacity()))
            << new QStandardItem(formatVietnamCurrency(flightRevenue));

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