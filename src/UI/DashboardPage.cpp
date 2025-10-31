#include "DashboardPage.h"
#include "core/AccountManager.h"
#include "core/ReportManager.h"
#include "entities/Account.h"

#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QHeaderView> // Để style header của bảng

DashboardPage::DashboardPage(AccountManager* accManager, ReportManager* reportManager, QWidget *parent)
    : QWidget(parent), accountManager_(accManager), reportManager_(reportManager)
{
    Q_ASSERT(accountManager_ != nullptr);
    Q_ASSERT(reportManager_ != nullptr);
    
    setupUi();
    setupModel();
    refreshData(); // Tải dữ liệu (giả lập)
}

void DashboardPage::setupUi()
{
    // nền xám xanh nhạt giống login
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.SectionTitle { color: #123B7A; font-weight:700; font-size: 16px; }"
        "QFrame#StatCard { background: white; border:1px solid #0E3B7C; }"
        "QTableView { background:white; border:0px solid #d4dce5; }"
        "QHeaderView::section { background:#d5e2f2; padding:6px; border:1px solid #c2cfe2; }"
        "DashboardTitle { font-size: 18px; }"
        "TableTitle { font-size: 18px; }"
    );

    // layout gốc: cách lề trái của sidebar 0 vì trang này nằm trong khu vực content
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);   // giống ảnh 2: dính vào top
    mainLayout->setSpacing(0);

    // ====== Thanh top: "Hoạt động hôm nay" + 4 card nhỏ ======
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topBarLayout = new QVBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 20, 24, 16); // lề trong
    topBarLayout->setSpacing(16);

    // tiêu đề
    QLabel *title = new QLabel("Hoạt động hôm nay", topBar);
    title->setObjectName("DashboardTitle");
    title->setProperty("class", "SectionTitle");
    topBarLayout->addWidget(title);

    // 4 card nằm ngang, sát nhau
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    // card giống hình: nền trắng, viền xanh đậm, header xanh đậm
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

    // ====== Bảng ======
    flightsTable_ = new QTableView(this);

    flightsTable_->verticalHeader()->setVisible(true);

    
    flightsTable_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    flightsTable_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    flightsTable_->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    flightsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);

    // sau khi tạo flightsTable_ và trước addWidget(flightsTable_, 1);
    flightsTable_->verticalHeader()->setVisible(true);               // bật cột STT
    flightsTable_->verticalHeader()->setMinimumWidth(32);            // cho nó có bề rộng
    flightsTable_->verticalHeader()->setDefaultSectionSize(32);      // cao từng dòng
    flightsTable_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    flightsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    flightsTable_->horizontalHeader()->setStretchLastSection(true);
    flightsTable_->setAlternatingRowColors(false);
    flightsTable_->setShowGrid(true);
    flightsTable_->setStyleSheet(
        "QTableView { background:white; border-left:none; border-right:none; }"
        "QTableView::item { padding:0px; }"
    );

    // bọc bảng trong 1 widget để tạo lề 24px 2 bên như hình
    QWidget *tableWrapper = new QWidget(this);
    QVBoxLayout *tableWrapLayout = new QVBoxLayout(tableWrapper);
    tableWrapLayout->setContentsMargins(24, 10, 24, 24);
    tableWrapLayout->addWidget(flightsTable_);

    mainLayout->addWidget(tableWrapper, 1);    // bảng chiếm phần còn lại
}


QWidget* DashboardPage::createStatCard(const QString& title, const QString& defaultValue, QLabel*& outValueLabel)
{
    // khung ngoài
    QFrame *card = new QFrame(this);
    card->setObjectName("StatCard");
    card->setFixedHeight(90);              // thấp như hình
    card->setStyleSheet(
        "QFrame#StatCard {"
        "   background: white;"
        "   border: 1px solid #0E3B7C;"
        "}"
    );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0,0,0,0);
    cardLayout->setSpacing(0);

    // thanh đầu màu xanh
    QWidget *head = new QWidget(card);
    head->setStyleSheet("background:#0E3B7C; color:white;");
    head->setFixedHeight(28);
    QHBoxLayout *headLayout = new QHBoxLayout(head);
    headLayout->setContentsMargins(10, 0, 10, 0);
    QLabel *titleLabel = new QLabel(title, head);
    titleLabel->setStyleSheet("font-size:13px; font-weight:600;");
    titleLabel->setAlignment(Qt::AlignCenter); 
    headLayout->addWidget(titleLabel);

    // phần giá trị
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


/**
 * @brief Khởi tạo model cho bảng
 */
void DashboardPage::setupModel()
{
    flightsModel_ = new QStandardItemModel(0, 8, this);
    flightsModel_->setHorizontalHeaderLabels({
        "ID chuyến", "ID tuyến", "Thời gian khởi hành", "Thời gian cất cánh",
        "Hạng phổ thông", "Hạng thương gia", "Giá phổ thông", "Giá thương gia"
    });
    flightsTable_->setModel(flightsModel_);
    auto *h = flightsTable_->horizontalHeader();
    h->setSectionResizeMode(QHeaderView::ResizeToContents);   // cột đủ chứa chữ header
    h->setStretchLastSection(true);                           // cột cuối vẫn giãn hết

    // Nếu bạn thấy cột quá chật thì dùng kiểu “mặc định là nội dung, nhưng vẫn kéo được
    // h->setSectionResizeMode(QHeaderView::Interactive);
    // h->setDefaultSectionSize(130);     // đủ cho “Thời gian cất cánh”
    // h->setMinimumSectionSize(90);

}

/**
 * @brief Tải/Làm mới dữ liệu
 */
void DashboardPage::refreshData()
{
    // Lấy thông tin agent hiện tại
    Account* currentUser = accountManager_->getCurrentUser();
    if (!currentUser) {
        // Không có user đăng nhập, hiển thị giá trị mặc định
        salesCountLabel_->setText("0");
        salesTotalLabel_->setText("0");
        cancelCountLabel_->setText("0");
        doVeLabel_->setText("0");
        return;
    }
    
    std::string agentId = currentUser->getId();
    
    // Lấy thống kê từ ReportManager
    int dailySales = reportManager_->getDailyTicketsSold(agentId);
    double dailyRevenue = reportManager_->getDailyRevenue(agentId);
    int dailyCancellations = reportManager_->getDailyCancellations(agentId);
    int dailyChanges = reportManager_->getDailyTicketChanges(agentId);
    
    // Cập nhật các label
    salesCountLabel_->setText(QString::number(dailySales));
    
    // Format revenue with thousand separators
    QString revenueStr = QString::number(static_cast<long long>(dailyRevenue), 'f', 0);
    // Add thousand separators
    int pos = revenueStr.length() - 3;
    while (pos > 0) {
        revenueStr.insert(pos, ',');
        pos -= 3;
    }
    salesTotalLabel_->setText(revenueStr);
    
    cancelCountLabel_->setText(QString::number(dailyCancellations));
    doVeLabel_->setText(QString::number(dailyChanges));

    // Giả lập cập nhật Bảng (xóa dữ liệu cũ, thêm dữ liệu mới):
    flightsModel_->removeRows(0, flightsModel_->rowCount());
    
    // TODO: Tích hợp dữ liệu chuyến bay thực tế trong ngày
    // Hiện tại để trống hoặc dữ liệu giả lập
    
    QList<QStandardItem *> row1;
    row1 << new QStandardItem("VN123") << new QStandardItem("SGN-HAN")
         << new QStandardItem("08:00") << new QStandardItem("08:15")
         << new QStandardItem("100/150") << new QStandardItem("10/20")
         << new QStandardItem("1,500,000") << new QStandardItem("3,000,000");
    flightsModel_->appendRow(row1);
    
    QList<QStandardItem *> row2;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row2);

    QList<QStandardItem *> row3;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row3);

    QList<QStandardItem *> row4;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row4);

    QList<QStandardItem *> row5;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row5);

    QList<QStandardItem *> row6;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row6);

    QList<QStandardItem *> row7;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row7);

    QList<QStandardItem *> row8;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row8);

    QList<QStandardItem *> row9;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row9);

    QList<QStandardItem *> row10;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row10);

    QList<QStandardItem *> row11;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row11);

    QList<QStandardItem *> row12;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row12);

    QList<QStandardItem *> row13;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row13);

    QList<QStandardItem *> row14;
    row2 << new QStandardItem("VJ456") << new QStandardItem("DAD-SGN")
         << new QStandardItem("10:30") << new QStandardItem("10:45")
         << new QStandardItem("120/180") << new QStandardItem("5/12")
         << new QStandardItem("1,200,000") << new QStandardItem("2,500,000");
    flightsModel_->appendRow(row14);
    
    // --- [HẾT CHỖ NỐI API] ---
}