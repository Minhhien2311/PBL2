#include "SearchBookPage.h"

#include "AirportComboBox.h"
#include "BoldItemDelegate.h"
#include "BookingDialog.h"
#include "PageRefresher.h"
#include "core/AccountManager.h"
#include "core/AirportManager.h"
#include "core/BookingManager.h"
#include "core/FlightManager.h"
#include "entities/Account.h"
#include "entities/Flight.h"
#include "utils/DateTime.h"
#include "utils/Helpers.h"
#include "utils/Sorting.h"

#include <QCalendarWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

#include <string>

namespace {
    QString formatVietnamCurrency(int price) {
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

// Constants
namespace {
    constexpr int MAX_FLIGHT_PRICE = 99999999;
}

SearchBookPage::SearchBookPage(FlightManager* flManager,
                               BookingManager* bkManager,
                               AccountManager* accManager,
                               AirportManager* airportManager,
                               PassengerManager* passengerManager,
                               QWidget* parent)
    : QWidget(parent),
      flightManager_(flManager),
      bookingManager_(bkManager),
      accountManager_(accManager),
      airportManager_(airportManager),
      passengerManager_(passengerManager)
{
    Q_ASSERT(flightManager_ != nullptr);
    Q_ASSERT(bookingManager_ != nullptr);

    setupUi();
    setupModel();
    setupConnections();
    loadAllFlights();
}

void SearchBookPage::setupUi()
{
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLineEdit, QDateEdit, QComboBox, QSpinBox { background:white; border:1px solid #608bc1; "
            "border-radius:4px; height:26px; padding-left:6px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; "
            "height:24px; font-weight:600; }"
    );

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    

    // ========== TOP BAR ==========
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 10);
    topLayout->setSpacing(14);
    topLayout->addStretch();

    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);
    headerRow->addStretch();

    QPushButton* refreshButton = new QPushButton("Làm mới trang", topBar);
    
    refreshButton->setIcon(QIcon("C:/PBL2/assets/icons/reload.png")); 
    refreshButton->setIconSize(QSize(14, 14)); // Kích thước icon

    refreshButton->setStyleSheet(
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
    
    refreshButton->setCursor(Qt::PointingHandCursor);
    
    headerRow->addWidget(refreshButton);
    topLayout->addLayout(headerRow);

    connect(refreshButton, &QPushButton::clicked, this, &SearchBookPage::refreshPage);

    // ========== SEARCH BOX ==========
    QWidget* searchBox = new QWidget;
    QVBoxLayout* searchBoxLayout = new QVBoxLayout(searchBox);
    searchBoxLayout->setContentsMargins(12, 12, 12, 12);
    searchBoxLayout->setSpacing(10);
    
    searchBox->setStyleSheet(
        "QWidget { background: white; border: 1px solid #133e87; border-radius: 6px; }"
    );

    QLabel* searchTitle = new QLabel("🔎 Tìm kiếm chuyến bay theo nhiều tiêu chí");
    searchTitle->setStyleSheet("font-weight: 600; color: #123B7A; font-size: 14px; background: transparent; border: none;");
    searchBoxLayout->addWidget(searchTitle);

    QGridLayout* filterLayout = new QGridLayout();
    filterLayout->setHorizontalSpacing(12);
    filterLayout->setVerticalSpacing(8);

    QLabel* fromLbl = new QLabel("Điểm đi");
    fromLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(fromLbl, 0, 0);
    
    QLabel* toLbl = new QLabel("Điểm đến");
    toLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(toLbl, 0, 1);
    
    QLabel* dateLbl = new QLabel("Ngày khởi hành");
    dateLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(dateLbl, 0, 2);
    
    QLabel* airlineLbl = new QLabel("Hãng hàng không");
    airlineLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(airlineLbl, 0, 3);
    
    QLabel* priceLbl = new QLabel("Khoảng giá mong muốn");
    priceLbl->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(priceLbl, 0, 4, 1, 3);

    fromSearchCombo_ = new AirportComboBox(airportManager_, this);
    fromSearchCombo_->setMinimumHeight(36);
    filterLayout->addWidget(fromSearchCombo_, 1, 0);

    toSearchCombo_ = new AirportComboBox(airportManager_, this);
    toSearchCombo_->setMinimumHeight(36);
    filterLayout->addWidget(toSearchCombo_, 1, 1);

    dateSearchEdit_ = new QLineEdit(this);
    dateSearchEdit_->setPlaceholderText("DD/MM/YYYY");
    dateSearchEdit_->setMinimumHeight(36);
    filterLayout->addWidget(dateSearchEdit_, 1, 2);

    airlineFilterCombo_ = new QComboBox(this);
    airlineFilterCombo_->addItem("Tùy chọn", "");
    std::vector<std::string> airlines = Helpers::loadAirlinesFromFile("C:/PBL2/data/airlines.txt");
    for (const std::string& airline : airlines) {
        airlineFilterCombo_->addItem(QString::fromStdString(airline), 
                                      QString::fromStdString(airline));
    }
    airlineFilterCombo_->setMinimumHeight(36);
    filterLayout->addWidget(airlineFilterCombo_, 1, 3);

    priceMinEdit_ = new QLineEdit(this);
    priceMinEdit_->setPlaceholderText("Tùy chọn");
    priceMinEdit_->setValidator(new QIntValidator(0, MAX_FLIGHT_PRICE, this));
    priceMinEdit_->setMinimumHeight(36);
    filterLayout->addWidget(priceMinEdit_, 1, 4);

    QLabel* dashLabel = new QLabel("—");
    dashLabel->setAlignment(Qt::AlignCenter);
    dashLabel->setStyleSheet("background: transparent; border: none; color: #123B7A;");
    filterLayout->addWidget(dashLabel, 1, 5);

    priceMaxEdit_ = new QLineEdit(this);
    priceMaxEdit_->setPlaceholderText("Tùy chọn");
    priceMaxEdit_->setValidator(new QIntValidator(0, MAX_FLIGHT_PRICE, this));
    priceMaxEdit_->setMinimumHeight(36);
    filterLayout->addWidget(priceMaxEdit_, 1, 6);
    QPushButton* searchBtn = new QPushButton("Tìm kiếm", this);
    searchBtn->setMinimumHeight(36);
    searchBtn->setMinimumWidth(110);
    searchBtn->setCursor(Qt::PointingHandCursor);
    searchBtn->setStyleSheet(
        "QPushButton { background-color: #4472C4; color: white; padding: 0 20px; "
        "border-radius: 6px; font-weight: 600; }"
        "QPushButton:hover { background-color: #365a9e; }"
    );
    filterLayout->addWidget(searchBtn, 1, 7);

    // Set column stretch
    filterLayout->setColumnStretch(0, 1);
    filterLayout->setColumnStretch(1, 1);
    filterLayout->setColumnStretch(2, 1);
    filterLayout->setColumnStretch(3, 1);
    filterLayout->setColumnStretch(7, 1);

    searchBoxLayout->addLayout(filterLayout);

    warningLabel_ = new QLabel(this);
    warningLabel_->setStyleSheet("color: #FF0000; font-size: 11px; font-weight: 500; border: none; background: transparent;");
    searchBoxLayout->addWidget(warningLabel_);
    warningLabel_->setVisible(false);

    topLayout->addWidget(searchBox);

    connect(searchBtn, &QPushButton::clicked, this, &SearchBookPage::onSearchClicked);

    mainLayout->addWidget(topBar);

    // ========== TABLE HEADER ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *thLayout = new QHBoxLayout(tableHeader);
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(10);

    statusLabel_ = new QLabel("", this);
    statusLabel_->setStyleSheet("color: #123B7A; font-size: 13px; font-weight: 650;");
    thLayout->addWidget(statusLabel_);

    thLayout->addStretch();
    sortingCombo_ = new QComboBox(this);
    sortingCombo_->setStyleSheet(
        "QComboBox { "
        "   background: white; "
        "   border: 1px solid #1e3e87; "
        "   border-radius: 2px; "
        "   height: 20px; "
        "   padding-left: 6px; "
        "}"
        "QComboBox QAbstractItemView { "
        "   background-color: white; "
        "   border: 1px solid #1e3e87; "
        "   selection-background-color: #4472C4; "
        "   selection-color: white; "
        "   outline: 0px; "
        "}"
    );

    sortingCombo_->setCursor(Qt::PointingHandCursor);
    
    sortingCombo_->addItem("Sắp xếp mặc định");
    sortingCombo_->addItem("Giá: Thấp đến Cao");
    sortingCombo_->addItem("Giá: Cao đến Thấp");
    sortingCombo_->addItem("Giờ bay: Sớm nhất");
    sortingCombo_->addItem("Hãng bay: A-Z");

    thLayout->addWidget(sortingCombo_);

    bookButton_ = new QPushButton("Đặt vé cho chuyến đã chọn", this);
    bookButton_->setCursor(Qt::PointingHandCursor);

    QString btnStyle =
        "QPushButton { background: #27C93F; color: white; border:1px solid #1fab33; "
        "border-radius:6px; height:20px; padding:4px 12px; font-weight:600; }"
        "QPushButton:hover { background: #1fab33; color: white; }";

    bookButton_->setStyleSheet(btnStyle);

    thLayout->addWidget(bookButton_);

    mainLayout->addWidget(tableHeader);

    // ========== TABLE ==========
    QWidget *tableBox = new QWidget(this);
    QVBoxLayout *tblWrap = new QVBoxLayout(tableBox);
    tblWrap->setContentsMargins(24, 10, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setStyleSheet(
        "QTableView { background:white; border:0px solid #133e87; }"
    );
    tableView_->setItemDelegate(new BoldItemDelegate(this));

    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->verticalHeader()->setVisible(false);
    tableView_->setAlternatingRowColors(true);
    tableView_->setShowGrid(false);

    tableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    tblWrap->addWidget(tableView_);
    mainLayout->addWidget(tableBox, 1);
}

void SearchBookPage::setupModel()
{
    model_ = new QStandardItemModel(0, 9, this);
    model_->setHorizontalHeaderLabels({
        "STT", "Mã Chuyến", "Mã tuyến", "Hãng hàng không", "Số hiệu",
        "Ngày khởi hành", "Giờ khởi hành", "Ghế trống", "Giá từ"
    });

    tableView_->setModel(model_);

    QHeaderView *header = tableView_->horizontalHeader();

    header->setSectionResizeMode(QHeaderView::Stretch);

    header->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView_->setColumnWidth(0, 50);
}

void SearchBookPage::setupConnections()
{
    connect(bookButton_, &QPushButton::clicked, this, &SearchBookPage::onBookClicked);
    connect(sortingCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &SearchBookPage::onSortChanged);
}

// ========== HELPER FUNCTIONS ==========
void SearchBookPage::fillTable(const std::vector<Flight*>& flights)
{
    model_->removeRows(0, model_->rowCount());

    for (int i = 0; i < flights.size(); ++i) {
        Flight* flight = flights[i];

        if (!flight) continue;
        QString airline = QString::fromStdString(flight->getAirline());

        QString priceFormatted = formatVietnamCurrency(flight->getFareEconomy());

        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(i + 1))
            << new QStandardItem(QString::fromStdString(flight->getFlightId()))
            << new QStandardItem(QString::fromStdString(flight->getRouteId()))
            << new QStandardItem(airline)
            << new QStandardItem(QString::fromStdString(flight->getFlightNumber()))
            << new QStandardItem(QString::fromStdString(flight->getDepartureDate()))
            << new QStandardItem(QString::fromStdString(flight->getDepartureTime()))
            << new QStandardItem(QString::number(flight->getAvailableSeats()) + "/" + QString::number(flight->getTotalCapacity()))
            << new QStandardItem(priceFormatted);
        for (QStandardItem *item : row) {
        item->setTextAlignment(Qt::AlignCenter);
    }
        model_->appendRow(row);
    }
}

// ========== SLOTS ==========
void SearchBookPage::onSearchClicked()
{
    FlightManager::SearchCriteria criteria;
    criteria.fromIATA = fromSearchCombo_->getSelectedIATA();
    criteria.toIATA = toSearchCombo_->getSelectedIATA();

    bool hasFrom = !criteria.fromIATA.empty();
    bool hasTo = !criteria.toIATA.empty();
    if (hasFrom != hasTo) {
        warningLabel_->setText("Vui lòng chọn cả điểm đi và điểm đến để tìm kiếm.");
        warningLabel_->setVisible(true);
        QTimer::singleShot(4000, [this]() {
            warningLabel_->setVisible(false);
        });
        return;
    }
    
    QString dateText = dateSearchEdit_->text().trimmed();
    if (!dateText.isEmpty()) {
        QDate selectedDate = QDate::fromString(dateText, "dd/MM/yyyy");

        if (selectedDate.isValid()) {
            criteria.date = selectedDate.toString("dd/MM/yyyy").toStdString();
        } else {
            warningLabel_->setText("Vui lòng nhập ngày đúng định dạng: ngày/tháng/năm (ví dụ: 25/12/2025)");
            warningLabel_->setVisible(true);
            QTimer::singleShot(4000, [this]() {
                warningLabel_->setVisible(false);
            });
            return;
        }
    }
    
    if (airlineFilterCombo_->currentIndex() > 0) {
        criteria.airline = airlineFilterCombo_->currentData().toString().toStdString();
    }
    
    QString minText = priceMinEdit_->text().trimmed();
    QString maxText = priceMaxEdit_->text().trimmed();
    
    if (!minText.isEmpty()) {
        bool ok;
        int minPrice = minText.toInt(&ok);
        if (ok && minPrice >= 0) {
            criteria.minPrice = minPrice;
        }
    }
    
    if (!maxText.isEmpty()) {
        bool ok;
        int maxPrice = maxText.toInt(&ok);
        if (ok && maxPrice >= 0) {
            criteria.maxPrice = maxPrice;
        }
    }
    
    auto results = flightManager_->searchFlights(criteria);

    time_t now = utils::DateTime::toUnix(utils::DateTime::nowUtc());
    std::vector<Flight*> futureResults;
    
    for (Flight* flight : results) {
        time_t flightTime = utils::DateTime::toUnix(
            utils::DateTime::fromDmYHm(
                flight->getDepartureDate(), 
                flight->getDepartureTime()
            )
        );
        if (flightTime >= now) {
            futureResults.push_back(flight);
        }
    }
    
    currentFlights_ = futureResults;
    fillTable(futureResults);
    
    statusLabel_->setText(
        QString("Tìm thấy %1 chuyến bay").arg(futureResults.size())
    );
}

// ========== BOOKING ==========
void SearchBookPage::onBookClicked()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Chọn một chuyến bay trước khi đặt vé.");
        return;
    }

    QString flightId = model_->itemFromIndex(selected.first().siblingAtColumn(1))->text();
    
    Flight* flight = flightManager_->findFlightById(flightId.toStdString());
    if (!flight) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy chuyến bay.");
        return;
    }
    
    BookingDialog dialog(flight, flightManager_, bookingManager_, accountManager_, passengerManager_, airportManager_, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Thành công", "Đặt vé thành công!");
        
        onSearchClicked();
    }
}

void SearchBookPage::loadAllFlights()
{
    const std::vector<Flight*> flights = flightManager_->getFutureFlights(true);
    currentFlights_ = flights;
    
    fillTable(flights);
    
    statusLabel_->setText(
        QString("Hiển thị tất cả %1 chuyến bay").arg(flights.size())
    );
}

void SearchBookPage::onSortChanged(int index)
{
    if (currentFlights_.empty()) return;

    std::vector<Flight*> sortedList;

    switch(index) {
        case 0:
            sortedList = currentFlights_;
            break;

        case 1:
            sortedList = Sorting::sortByPrice(currentFlights_);
            break;

        case 2:
            sortedList = Sorting::sortByPrice(currentFlights_);
            std::reverse(sortedList.begin(), sortedList.end());
            break;

        case 3:
            sortedList = Sorting::sortByArrivalTime(currentFlights_);
            break;
        case 4:
            sortedList = Sorting::sortByAirline(currentFlights_);
            break;
    }

    fillTable(sortedList);
}

void SearchBookPage::refreshPage() {
    PageRefresher::clearSearchFields(this);
    PageRefresher::executeRefresh([this]() {
        loadAllFlights();
    });
}
