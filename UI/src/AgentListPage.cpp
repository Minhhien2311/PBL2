#include "AgentListPage.h"

#include "BoldItemDelegate.h"
#include "core/AccountManager.h"
#include "entities/Account.h"
#include "entities/AccountAgent.h"

#include <QDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

namespace {
    QString formatVietnamCurrency(double price) {
        QString priceStr = QString::number(static_cast<int>(price));
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

AgentListPage::AgentListPage(AccountManager* am, QWidget* parent)
    : QWidget(parent),
      accountManager_(am)
{
    Q_ASSERT(accountManager_ != nullptr);
    setupUI();
    setupModel();
    setupConnections();
    loadData();
}

void AgentListPage::setupUI()
{
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel.PageTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLabel.SectionTitle { color:#123B7A; font-weight:700; font-size:17px; }"
        "QLineEdit { background:white; border:1px solid #608bc1; "
        "border-radius:4px; height:26px; padding-left:6px; }"
        "QPushButton.SearchBtn { background:#4478BD; color:white; border-radius:6px; "
        "height:24px; font-weight:600; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // ========== TOP BAR ==========
    QWidget *topBar = new QWidget(this);
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 10);
    topLayout->setSpacing(10);

    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);
    headerRow->addStretch();

    refreshButton_ = new QPushButton("Làm mới trang", topBar);
    refreshButton_->setIcon(QIcon("C:/PBL2/assets/icons/reload.png")); 
    refreshButton_->setIconSize(QSize(14, 14));
    refreshButton_->setStyleSheet(
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
    refreshButton_->setCursor(Qt::PointingHandCursor);
    headerRow->addWidget(refreshButton_);
    topLayout->addLayout(headerRow);
    mainLayout->addWidget(topBar);

    // ========== HEADER + BUTTONS ==========
    QWidget *tableHeader = new QWidget(this);
    QHBoxLayout *thLayout = new QHBoxLayout(tableHeader);
    thLayout->setContentsMargins(24, 0, 24, 0);
    thLayout->setSpacing(10);

    viewDetailsBtn_ = new QPushButton("Xem chi tiết", this);
    viewDetailsBtn_->setStyleSheet(
        "background: #4478BD; color: #FFFFFF; border:none; border-radius:6px; "
        "height:20px; padding:4px 10px; font-weight:600; "
        "} QPushButton:hover { background: #365a9e; color: white;"
    );
    viewDetailsBtn_->setCursor(Qt::PointingHandCursor);
    viewDetailsBtn_->setEnabled(false);
    thLayout->addWidget(viewDetailsBtn_);

    addAgentBtn_ = new QPushButton("Thêm mới", this);
    addAgentBtn_->setStyleSheet(
        "background: #27C93F; color: #FFFFFF; border:none; border-radius:6px; "
        "height:20px; padding:4px 10px; font-weight:600; "
        "} QPushButton:hover { background: #1b4d1b; color: white;"
    );
    addAgentBtn_->setCursor(Qt::PointingHandCursor);
    thLayout->addWidget(addAgentBtn_);
    thLayout->addStretch();

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

void AgentListPage::setupModel()
{
    model_ = new QStandardItemModel(0, 4, this);
    
    model_->setHorizontalHeaderLabels({
        "STT",
        "ID Đại lý",
        "Tên tài khoản",
        "Họ và tên",
        "Trạng thái"
    });

    tableView_->setModel(model_);

    QHeaderView *header = tableView_->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(QHeaderView::Stretch);
    
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    tableView_->setColumnWidth(0, 50);
}

void AgentListPage::setupConnections()
{
    connect(addAgentBtn_, &QPushButton::clicked, this, &AgentListPage::onAddAgentClicked);
    connect(viewDetailsBtn_, &QPushButton::clicked, this, &AgentListPage::onViewDetailClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &AgentListPage::refreshPage);
    connect(tableView_->selectionModel(), &QItemSelectionModel::selectionChanged, 
            this, &AgentListPage::onSelectionChanged);
}

void AgentListPage::loadData()
{
    model_->removeRows(0, model_->rowCount());

    const auto& agents = accountManager_->getAllAgents();

    for (const auto* agent : agents) {
        displayAgent(agent);
    }
}

void AgentListPage::displayAgent(const AccountAgent* agent)
{
    if (!agent) return;
    
    int stt = model_->rowCount() + 1;

    QList<QStandardItem*> rowItems;
    
    // 0. STT
    rowItems << new QStandardItem(QString::number(stt));
    rowItems << new QStandardItem(QString::fromStdString(agent->getId()));
    rowItems << new QStandardItem(QString::fromStdString(agent->getUsername()));
    rowItems << new QStandardItem(QString::fromStdString(agent->getFullName()));
    
    QString statusStr;
    QColor statusColor;

    if (agent->getIsActive()) {
        statusStr = "Hoạt động";
        statusColor = QColor("#27C93F");
    } else {
        statusStr = "Tạm khóa";
        statusColor = QColor("#9E9E9E");
    }
    rowItems << new QStandardItem(statusStr);
    rowItems.last()->setForeground(statusColor);

    for (QStandardItem *item : rowItems) {
        item->setTextAlignment(Qt::AlignCenter);
    }

    model_->appendRow(rowItems);
}

// ========== SLOTS ==========
void AgentListPage::onSelectionChanged()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    viewDetailsBtn_->setEnabled(!selected.isEmpty());
}

void AgentListPage::onAddAgentClicked()
{
    showAddAgentDialog();
}

void AgentListPage::onViewDetailClicked()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một đại lý để xem chi tiết.");
        return;
    }
    
    showAgentDetailsDialog();
}

void AgentListPage::showAgentDetailsDialog()
{
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    // Lấy ID từ cột 1
    QString agentId = model_->data(selected.first().siblingAtColumn(1)).toString();
    AccountAgent* agent = accountManager_->findAgentById(agentId.toStdString());
    
    if (!agent) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy thông tin đại lý!");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Chi tiết đại lý");
    dialog.setFixedWidth(700);
    dialog.setStyleSheet(
        "QDialog { background: white; }"
        "QLabel { border: none; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 20, 50, 20);

    QLabel* titleLabel = new QLabel("Chi tiết thông tin đại lý", &dialog);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #123B7A;"
        "   font-weight: bold;"
        "   font-size: 20px;"
        "   background: transparent;"
        "   border: none;"
        "}"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QGroupBox* infoFrame = new QGroupBox("Thông tin đại lý", &dialog);
    infoFrame->setStyleSheet(
        "QGroupBox {"
        "   background: white;"
        "   border: 1px solid #608bc1;"
        "   border-radius: 6px;"
        "   margin-top: 10px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QGroupBox::title {"
        "   font-weight: 650;"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   left: 15px;"
        "   padding: 0 8px;"
        "   color: #1E4B8C;"
        "   background-color: white;"
        "}"
    );

    QVBoxLayout* frameLayout = new QVBoxLayout(infoFrame);
    frameLayout->setContentsMargins(30, 15, 30, 12);
    frameLayout->setSpacing(10);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(20);
    gridLayout->setVerticalSpacing(12);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 2);

    auto addInfoRow = [&](int row, const QString& label, const QString& value, 
                          bool valueBold = false, const QString& valueColor = "") {
        QLabel* labelWidget = new QLabel(label);
        labelWidget->setStyleSheet(
            "QLabel {"
            "   color: #1E4B8C;"
            "   font-size: 13px;"
            "   font-weight: 500;"
            "   background: transparent;"
            "   border: none;"
            "}"
        );

        QLabel* valueWidget = new QLabel(value);
        QString style = QString(
            "QLabel {"
            "   color: %1;"
            "   font-size: 13px;"
            "   font-weight: %2;"
            "   background: transparent;"
            "   border: none;"
            "}"
        ).arg(valueColor.isEmpty() ? "#2C3E50" : valueColor)
         .arg(valueBold ? "600" : "400");
        valueWidget->setStyleSheet(style);

        gridLayout->addWidget(labelWidget, row, 0, Qt::AlignLeft);
        gridLayout->addWidget(valueWidget, row, 1, Qt::AlignLeft);
    };

    addInfoRow(0, "ID Đại lý:", QString::fromStdString(agent->getId()), true);
    addInfoRow(1, "Tên đăng nhập:", QString::fromStdString(agent->getUsername()));
    addInfoRow(2, "Họ và tên:", QString::fromStdString(agent->getFullName()), true);
    addInfoRow(3, "Số điện thoại:", QString::fromStdString(agent->getPhone()));
    addInfoRow(4, "Email:", QString::fromStdString(agent->getEmail()));
    
    QString statusStr = agent->getIsActive() ? "Hoạt động" : "Tạm khóa";
    QString statusColor = agent->getIsActive() ? "#27C93F" : "#FF5F57";
    addInfoRow(5, "Trạng thái:", statusStr, true, statusColor);

    frameLayout->addLayout(gridLayout);
    mainLayout->addWidget(infoFrame);

    QPushButton* closeButton = new QPushButton("Đóng", &dialog);
    closeButton->setStyleSheet(
        "QPushButton {"
        "   background: #4478BD;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 6px;"
        "   height: 20px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: #365a9e;"
        "}"
    );
    closeButton->setCursor(Qt::PointingHandCursor);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    mainLayout->addWidget(closeButton);

    dialog.adjustSize();
    dialog.exec();
}

void AgentListPage::showAddAgentDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Thêm Đại lý mới");
    dialog.setFixedWidth(700);
    dialog.setMinimumHeight(550);
    
    dialog.setStyleSheet(
        "QDialog { background: white; }"
        "QLabel { color: #133e87; font-weight: 600; font-size: 12px; border: none; }"
        "QLineEdit { "
        "   background: white; border: 1px solid #608bc1; border-radius: 4px; height: 20px; "
        "   padding-left: 8px; font-size: 12px; color: #1E4B8C; }"
        "QLineEdit:focus { border: 2px solid #1E4B8C; }"
        "QGroupBox { "
        "   background: white; border: 1px solid #608bc1; border-radius: 6px; "
        "   margin-top: 12px; font-size: 13px; font-weight: bold; }"
        "QGroupBox::title { "
        "   font-weight: 650; subcontrol-origin: margin; subcontrol-position: top left; "
        "   left: 15px; padding: 0 8px; color: #1E4B8C; background-color: white; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 10);

    // Header
    QLabel* titleLabel = new QLabel("Nhập thông tin đại lý mới", &dialog);
    titleLabel->setStyleSheet(
        "color: #123B7A; font-weight: bold; font-size: 20px; background: transparent; border: none;"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // ========== NHÓM 1: THÔNG TIN TÀI KHOẢN ==========
    QGroupBox* accountGroup = new QGroupBox("Thông tin tài khoản");
    QFormLayout* accountLayout = new QFormLayout(accountGroup);
    accountLayout->setContentsMargins(30, 20, 30, 15);
    accountLayout->setSpacing(10);
    
    QLineEdit* usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("VD: agent123");
    
    QLineEdit* passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Tối thiểu 6 ký tự");
    passwordEdit->setEchoMode(QLineEdit::Password);

    accountLayout->addRow("Tên đăng nhập:", usernameEdit);
    accountLayout->addRow("Mật khẩu:", passwordEdit);

    mainLayout->addWidget(accountGroup);

    // ========== NHÓM 2: THÔNG TIN CÁ NHÂN ==========
    QGroupBox* personalGroup = new QGroupBox("Thông tin cá nhân");
    QFormLayout* personalLayout = new QFormLayout(personalGroup);
    personalLayout->setContentsMargins(30, 20, 30, 15);
    personalLayout->setSpacing(10);
    personalLayout->setLabelAlignment(Qt::AlignLeft);

    QLineEdit* fullNameEdit = new QLineEdit();
    fullNameEdit->setPlaceholderText("VD: Nguyễn Văn A");
    
    QLineEdit* phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("VD: 0912345678");
    
    QLineEdit* emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("VD: agent@example.com");

    personalLayout->addRow("Họ và tên:", fullNameEdit);
    personalLayout->addRow("Số điện thoại:", phoneEdit);
    personalLayout->addRow("Email:", emailEdit);

    mainLayout->addWidget(personalGroup);

    mainLayout->addSpacing(10);

    // Footer Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    QPushButton* cancelBtn = new QPushButton("Hủy bỏ");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(
        "QPushButton { "
        "   background: #E0E0E0; color: #333; border: none; border-radius: 6px; "
        "   height: 35px; font-weight: bold; font-size: 13px; "
        "} QPushButton:hover { background: #BDBDBD; }"
    );

    QPushButton* saveBtn = new QPushButton("Thêm đại lý");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        "QPushButton { "
        "   background: #608bc1; color: white; border: none; border-radius: 6px; "
        "   height: 35px; font-weight: bold; font-size: 13px; "
        "} QPushButton:hover { background: #365a9e; }"
    );

    buttonLayout->addWidget(cancelBtn, 1);
    buttonLayout->addWidget(saveBtn, 2);
    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(saveBtn, &QPushButton::clicked, [&]() {
        // Validation
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text().trimmed();
        QString fullName = fullNameEdit->text().trimmed();
        QString phone = phoneEdit->text().trimmed();
        QString email = emailEdit->text().trimmed();

        if (username.isEmpty() || password.isEmpty() || fullName.isEmpty() || 
            phone.isEmpty() || email.isEmpty()) {
            QMessageBox::warning(&dialog, "Lỗi", "Vui lòng điền đầy đủ thông tin!");
            return;
        }

        if (password.length() < 6) {
            QMessageBox::warning(&dialog, "Lỗi", "Mật khẩu phải có ít nhất 6 ký tự!");
            return;
        }

        // Create new agent
        bool success = accountManager_->createNewAgent(
            username.toStdString(),
            password.toStdString(),
            fullName.toStdString(),
            phone.toStdString(),
            email.toStdString()
        );

        if (success) {
            QMessageBox::information(&dialog, "Thành công", 
                "✅ Đã tạo tài khoản Đại lý mới thành công!");
            loadData();
            emit agentAdded();
            dialog.accept();
        } else {
            QMessageBox::warning(&dialog, "Lỗi", 
                "❌ Tên đăng nhập đã tồn tại hoặc có lỗi xảy ra!");
        }
    });

    dialog.exec();
}

void AgentListPage::refreshPage()
{
    loadData();
}