#include "PromotionsPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDialog>
#include <QFormLayout>
#include <QDateEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QScrollArea>
#include "BoldItemDelegate.h"

// =================================================================================
// INTERNAL CLASS: PromotionsDialog (Được nhúng trực tiếp vào đây)
// =================================================================================
class PromotionsDialog : public QDialog {
public:
    // UI Components
    QLineEdit* codeEdit;
    QLineEdit* descEdit;
    QComboBox* typeCombo;
    QDoubleSpinBox* valueSpin;
    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;
    QComboBox* statusCombo;
    QPushButton* saveButton;
    QPushButton* cancelButton;
    
    bool isEditMode;

    // Constructor Thêm/Sửa gộp làm 1 cho gọn
    PromotionsDialog(QWidget* parent = nullptr, Promotion* existingProm = nullptr) 
        : QDialog(parent), isEditMode(existingProm != nullptr) 
    {
        // 1. SETUP WINDOW & STYLESHEET (Copy chuẩn từ FlightDialog)
        setWindowTitle(isEditMode ? "Cập nhật khuyến mãi" : "Thêm khuyến mãi mới");
        setFixedWidth(600); 
        setMinimumHeight(600);
        
        setStyleSheet(
            "QWidget { background: white; }"
            "QDialog { background: white; }"
            "QLabel { color: #133e87; font-weight: 600; font-size: 13px; border: none; }"
            "QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox { "
            "   background: white; border: 1px solid #133e87; border-radius: 4px; height: 30px; "
            "   padding-left: 8px; font-size: 13px; color: #133e87; }"
            "QLineEdit:focus, QComboBox:focus, QDoubleSpinBox:focus { border: 1.8px solid #133e87; }"
            "QLineEdit:read-only { background: #f5f7fa; color: #666; border: 1px solid #cbd5e0; }"
            "QGroupBox { background: white; border: 1px solid #133e87; border-radius: 6px; margin-top: 10px; font-size: 13px; font-weight: 500; }"
            "QGroupBox::title { font-weight: 700; subcontrol-origin: margin; subcontrol-position: top left; left: 15px; padding: 0 8px; color: #133e87; background-color: white; }"
        );

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(15);
        mainLayout->setContentsMargins(40, 20, 40, 20);

        // Header Title
        QLabel *titleLabel = new QLabel(isEditMode ? "Chỉnh sửa thông tin" : "Nhập thông tin khuyến mãi", this);
        titleLabel->setStyleSheet("color: #133e87; font-weight: bold; font-size: 20px; background: transparent;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Scroll Area Container
        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        QWidget* scrollContent = new QWidget();
        QVBoxLayout* contentLayout = new QVBoxLayout(scrollContent);
        contentLayout->setSpacing(15);
        
        // --- GROUP 1: THÔNG TIN CHUNG ---
        QGroupBox* infoGroup = new QGroupBox("Thông tin chung");
        QFormLayout* infoLayout = new QFormLayout(infoGroup);
        infoLayout->setContentsMargins(15, 20, 20, 15);
        infoLayout->setSpacing(12);

        codeEdit = new QLineEdit();
        codeEdit->setPlaceholderText("VD: SUMMER2025");
        
        descEdit = new QLineEdit();
        descEdit->setPlaceholderText("Mô tả ngắn gọn...");
        
        statusCombo = new QComboBox();
        statusCombo->addItem("Đang hoạt động", true);
        statusCombo->addItem("Tạm dừng", false);

        infoLayout->addRow("Mã khuyến mãi:", codeEdit);
        infoLayout->addRow("Mô tả:", descEdit);
        infoLayout->addRow("Trạng thái:", statusCombo);
        contentLayout->addWidget(infoGroup);

        // --- GROUP 2: GIÁ TRỊ GIẢM ---
        QGroupBox* valGroup = new QGroupBox("Cấu hình giảm giá");
        QFormLayout* valLayout = new QFormLayout(valGroup);
        valLayout->setContentsMargins(20, 20, 20, 15);
        valLayout->setSpacing(12);

        typeCombo = new QComboBox();
        typeCombo->addItem("Phần trăm (%)", "PERCENT");
        typeCombo->addItem("Tiền mặt (VNĐ)", "AMOUNT");

        valueSpin = new QDoubleSpinBox();
        valueSpin->setRange(0, 1000000000);
        valueSpin->setDecimals(0);
        valueSpin->setValue(10);
        valueSpin->setSuffix(" %");

        // Logic đổi suffix
        QObject::connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int idx){
            if (idx == 0) { // PERCENT
                valueSpin->setSuffix(" %");
                valueSpin->setRange(0, 100);
            } else { // AMOUNT
                valueSpin->setSuffix(" VNĐ");
                valueSpin->setRange(0, 1000000000);
            }
        });

        valLayout->addRow("Loại giảm:", typeCombo);
        valLayout->addRow("Giá trị:", valueSpin);
        contentLayout->addWidget(valGroup);

        // --- GROUP 3: THỜI GIAN ---
        QGroupBox* timeGroup = new QGroupBox("Thời gian áp dụng");
        QFormLayout* timeLayout = new QFormLayout(timeGroup);
        timeLayout->setContentsMargins(20, 20, 20, 15);
        timeLayout->setSpacing(12);

        startDateEdit = new QDateEdit(QDate::currentDate());
        startDateEdit->setCalendarPopup(true);
        startDateEdit->setDisplayFormat("dd/MM/yyyy");

        endDateEdit = new QDateEdit(QDate::currentDate().addDays(30));
        endDateEdit->setCalendarPopup(true);
        endDateEdit->setDisplayFormat("dd/MM/yyyy");

        timeLayout->addRow("Ngày bắt đầu:", startDateEdit);
        timeLayout->addRow("Ngày kết thúc:", endDateEdit);
        contentLayout->addWidget(timeGroup);

        scrollArea->setWidget(scrollContent);
        mainLayout->addWidget(scrollArea);

        // Footer Buttons
        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->setSpacing(15);
        
        cancelButton = new QPushButton("Hủy bỏ");
        cancelButton->setCursor(Qt::PointingHandCursor);
        cancelButton->setStyleSheet("QPushButton { background: #E0E0E0; color: #333; border: none; border-radius: 6px; height: 35px; font-weight: bold; } QPushButton:hover { background: #BDBDBD; }");

        saveButton = new QPushButton(isEditMode ? "Lưu thay đổi" : "Tạo mới");
        saveButton->setCursor(Qt::PointingHandCursor);
        saveButton->setStyleSheet("QPushButton { background: #608bc1; color: white; border: none; border-radius: 6px; height: 35px; font-weight: bold; } QPushButton:hover { background: #365a9e; }");

        btnLayout->addWidget(cancelButton, 1);
        btnLayout->addWidget(saveButton, 2);
        mainLayout->addLayout(btnLayout);

        // Connect Buttons
        QObject::connect(saveButton, &QPushButton::clicked, this, &PromotionsDialog::onSave);
        QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        // FILL DATA IF EDIT MODE
        if (existingProm) {
            codeEdit->setText(QString::fromStdString(existingProm->getCode()));
            codeEdit->setReadOnly(true); // Cấm sửa ID
            descEdit->setText(QString::fromStdString(existingProm->getDescription()));
            
            int typeIdx = (existingProm->getDiscountType() == "PERCENT") ? 0 : 1;
            typeCombo->setCurrentIndex(typeIdx);
            
            valueSpin->setValue(existingProm->getDiscountValue());
            
            startDateEdit->setDate(QDate::fromString(QString::fromStdString(existingProm->getStartDate()), "dd/MM/yyyy"));
            endDateEdit->setDate(QDate::fromString(QString::fromStdString(existingProm->getEndDate()), "dd/MM/yyyy"));
            
            statusCombo->setCurrentIndex(existingProm->getIsActive() ? 0 : 1);
        }
    }

    void onSave() {
        if (codeEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Thiếu dữ liệu", "Vui lòng nhập Mã khuyến mãi!");
            return;
        }
        if (startDateEdit->date() > endDateEdit->date()) {
            QMessageBox::warning(this, "Lỗi logic", "Ngày kết thúc phải sau ngày bắt đầu!");
            return;
        }
        accept();
    }

    // Helper function để lấy đối tượng Promotion từ form
    Promotion getData() {
        return Promotion(
            codeEdit->text().toStdString(),
            descEdit->text().toStdString(),
            typeCombo->currentData().toString().toStdString(),
            valueSpin->value(),
            startDateEdit->date().toString("dd/MM/yyyy").toStdString(),
            endDateEdit->date().toString("dd/MM/yyyy").toStdString(),
            statusCombo->currentData().toBool()
        );
    }
};

// =================================================================================
// IMPLEMENTATION: PromotionPage
// =================================================================================

PromotionsPage::PromotionsPage(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setupModel();
    setupConnections();
    
    // Tạo folder data nếu chưa có
    if (!QDir("data").exists()) QDir().mkdir("data");
    
    refreshPage(); 
}

PromotionsPage::~PromotionsPage() {}

void PromotionsPage::setupUi() {
    this->setStyleSheet(
        "QWidget { background: #F2F6FD; }"
        "QLabel { color: #123B7A; font-weight:600; font-size:14px; }"
        "QLineEdit { background:white; border:1px solid #133e87; border-radius:4px; height:30px; padding-left:6px; }"
        "QPushButton.SearchBtn:hover { background: #365a9e; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. TOP BAR
    QWidget *topBar = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(24, 20, 24, 10);
    topLayout->addStretch();

    refreshButton_ = new QPushButton("Làm mới dữ liệu", topBar);
    refreshButton_->setIcon(QIcon("C:/PBL2/assets/icons/reload.png")); // Nhớ chỉnh path icon
    refreshButton_->setCursor(Qt::PointingHandCursor);
    refreshButton_->setStyleSheet("QPushButton { background: transparent; color: #133e87; font-weight: bold; border: none; } QPushButton:hover { text-decoration: underline; }");
    topLayout->addWidget(refreshButton_);
    mainLayout->addWidget(topBar);

    // 2. SEARCH BOX
    QWidget *searchContainer = new QWidget(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(searchContainer);
    searchLayout->setContentsMargins(24, 0, 24, 10);
    searchLayout->setAlignment(Qt::AlignLeft);

    QWidget* quickSearchBox = new QWidget;
    quickSearchBox->setStyleSheet("background: white; border: 1px solid #133e87; border-radius: 6px;");
    QHBoxLayout* qsLayout = new QHBoxLayout(quickSearchBox);
    qsLayout->setContentsMargins(15, 10, 15, 10);
    qsLayout->setSpacing(10);

    QLabel* searchLabel = new QLabel("Tìm khuyến mãi:");
    searchLabel->setStyleSheet("color: #133e87; font-weight:600; font-size:14px; border: none;");
    qsLayout->addWidget(searchLabel);

    codeSearchEdit_ = new QLineEdit;
    codeSearchEdit_->setPlaceholderText("Nhập mã KM...");
    codeSearchEdit_->setMinimumWidth(200);
    codeSearchEdit_->setFixedHeight(36);
    codeSearchEdit_->setStyleSheet("border: 1px solid #133e87; background: white; color: #133e87;");
    qsLayout->addWidget(codeSearchEdit_);

    searchBtn_ = new QPushButton("Tìm kiếm");
    searchBtn_->setStyleSheet(" background: #608bc1; color:white; border-radius:6px; height:36px; font-weight:600; padding: 0 15px; border: none;} QPushButton:hover { background: #365a9e; }");

    searchBtn_->setCursor(Qt::PointingHandCursor);
    qsLayout->addWidget(searchBtn_);

    searchLayout->addWidget(quickSearchBox);
    mainLayout->addWidget(searchContainer);

    // 3. CRUD BAR
    QWidget *crudHeader = new QWidget(this);
    QHBoxLayout *crudLayout = new QHBoxLayout(crudHeader);
    crudLayout->setContentsMargins(24, 10, 24, 10);

    statusLabel_ = new QLabel("Sẵn sàng", this);
    crudLayout->addWidget(statusLabel_);
    crudLayout->addStretch();

    addButton_ = new QPushButton("Thêm mới", this);
    editButton_ = new QPushButton("Sửa", this);
    deleteButton_ = new QPushButton("Xóa", this);
    addButton_->setStyleSheet("background: #27C93F; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #1b4d1b; color: white;");
    editButton_->setStyleSheet("background: #FFBD2E; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #b35c00; color: white;");
    deleteButton_->setStyleSheet("background: #FF5F57; color: #FFFFFF; border:none; border-radius:6px; height:20px; padding:4px 10px; font-weight:600; } QPushButton:hover { background: #8b1c1c; color: white;");

    crudLayout->addWidget(addButton_);
    crudLayout->addWidget(editButton_);
    crudLayout->addWidget(deleteButton_);
    mainLayout->addWidget(crudHeader);

    // 4. TABLE
    QWidget *tableContainer = new QWidget(this);
    QVBoxLayout *tblLayout = new QVBoxLayout(tableContainer);
    tblLayout->setContentsMargins(24, 0, 18, 20);

    tableView_ = new QTableView(this);
    tableView_->setStyleSheet("QTableView { background: white; border: 1px solid #133e87; selection-background-color: #4478BD; }");
    tableView_->setItemDelegate(new BoldItemDelegate(this));
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView_->setShowGrid(false);
    tableView_->setAlternatingRowColors(true);
    tableView_->verticalHeader()->setVisible(false);

    tblLayout->addWidget(tableView_);
    mainLayout->addWidget(tableContainer, 1);
}

void PromotionsPage::setupModel() {
    model_ = new QStandardItemModel(0, 7, this);
    model_->setHorizontalHeaderLabels({"Mã KM", "Mô tả", "Loại", "Giá trị", "Bắt đầu", "Kết thúc", "Trạng thái"});
    tableView_->setModel(model_);

    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    tableView_->setColumnWidth(1, 250); // Cột Mô tả rộng hơn
}

void PromotionsPage::setupConnections() {
    connect(refreshButton_, &QPushButton::clicked, this, &PromotionsPage::refreshPage);
    connect(searchBtn_, &QPushButton::clicked, this, &PromotionsPage::onSearchByCode);
    connect(addButton_, &QPushButton::clicked, this, &PromotionsPage::onAddPromotion);
    connect(editButton_, &QPushButton::clicked, this, &PromotionsPage::onEditPromotion);
    connect(deleteButton_, &QPushButton::clicked, this, &PromotionsPage::onDeletePromotion);
    connect(codeSearchEdit_, &QLineEdit::returnPressed, this, &PromotionsPage::onSearchByCode);
}

// --- FILE IO ---
void PromotionsPage::loadDataFromFile() {
    promotions_.clear();
    QFile file(QString::fromStdString(DATA_FILE));
    if (!file.exists()) return;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { // Open the file in read-only text mode
        QTextStream in(&file); // Create a QTextStream to read from the file
        in.setEncoding(QStringConverter::Utf8); // Set the encoding to UTF-8
        while (!in.atEnd()) { // Loop until the end of the file is reached
            QString line = in.readLine(); // Read a line from the file
            if (line.trimmed().isEmpty()) continue;
            QStringList parts = line.split("|");
            if (parts.size() >= 7) {
                Promotion p(parts[0].toStdString(), parts[1].toStdString(), parts[2].toStdString(),
                            parts[3].toDouble(), parts[4].toStdString(), parts[5].toStdString(),
                            parts[6] == "1");
                promotions_.push_back(p);
            }
        }
        file.close();
    }
}

void PromotionsPage::saveDataToFile() {
    QFile file(QString::fromStdString(DATA_FILE));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        for (const auto& p : promotions_) {
            out << QString::fromStdString(p.getCode()) << "|"
                << QString::fromStdString(p.getDescription()) << "|"
                << QString::fromStdString(p.getDiscountType()) << "|"
                << p.getDiscountValue() << "|"
                << QString::fromStdString(p.getStartDate()) << "|"
                << QString::fromStdString(p.getEndDate()) << "|"
                << (p.getIsActive() ? "1" : "0") << "\n";
        }
        file.close();
    }
}

void PromotionsPage::refreshTable(const std::vector<Promotion>& data) {
    model_->removeRows(0, model_->rowCount());
    for (const auto& p : data) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(p.getCode()));
        row << new QStandardItem(QString::fromStdString(p.getDescription()));
        
        QString typeStr = (p.getDiscountType() == "PERCENT") ? "Phần trăm (%)" : "Tiền mặt";
        row << new QStandardItem(typeStr);

        QString valStr = (p.getDiscountType() == "PERCENT") 
            ? QString::number(p.getDiscountValue()) + "%" 
            : QString::number(p.getDiscountValue()) + " đ";
        row << new QStandardItem(valStr);

        row << new QStandardItem(QString::fromStdString(p.getStartDate()));
        row << new QStandardItem(QString::fromStdString(p.getEndDate()));

        QStandardItem* statusItem = new QStandardItem(p.getIsActive() ? "Hoạt động" : "Hết hạn");
        statusItem->setForeground(p.getIsActive() ? QColor("#27C93F") : QColor("#FF5F57"));
        row << statusItem;

        for (auto item : row) item->setTextAlignment(Qt::AlignCenter);
        model_->appendRow(row);
    }
    statusLabel_->setText(QString("Tổng cộng: %1 khuyến mãi").arg(data.size()));
}

void PromotionsPage::refreshPage() {
    codeSearchEdit_->clear();
    loadDataFromFile();
    refreshTable(promotions_);
}

// --- ACTIONS ---
void PromotionsPage::onSearchByCode() {
    QString keyword = codeSearchEdit_->text().trimmed().toLower();
    if (keyword.isEmpty()) {
        refreshTable(promotions_);
        return;
    }
    std::vector<Promotion> results;
    for (const auto& p : promotions_) {
        if (QString::fromStdString(p.getCode()).toLower().contains(keyword))
            results.push_back(p);
    }
    refreshTable(results);
    statusLabel_->setText(QString("Tìm thấy %1 kết quả").arg(results.size()));
}

void PromotionsPage::onAddPromotion() {
    // Gọi dialog nội bộ
    PromotionsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Promotion newP = dialog.getData();
        
        // Check trùng ID
        for(const auto& p : promotions_) {
            if(p.getCode() == newP.getCode()) {
                QMessageBox::critical(this, "Lỗi", "Mã khuyến mãi đã tồn tại!");
                return;
            }
        }
        
        promotions_.push_back(newP);
        saveDataToFile();
        refreshPage();
        QMessageBox::information(this, "Thành công", "Đã thêm khuyến mãi mới!");
    }
}

void PromotionsPage::onEditPromotion() {
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Chú ý", "Vui lòng chọn khuyến mãi cần sửa!");
        return;
    }
    
    QString code = model_->item(selected.first().row(), 0)->text();
    Promotion* target = nullptr;
    for (auto& p : promotions_) {
        if (QString::fromStdString(p.getCode()) == code) {
            target = &p;
            break;
        }
    }
    
    if (target) {
        PromotionsDialog dialog(this, target);
        if (dialog.exec() == QDialog::Accepted) {
            *target = dialog.getData(); // Update data
            saveDataToFile();
            refreshPage();
            QMessageBox::information(this, "Thành công", "Cập nhật thành công!");
        }
    }
}

void PromotionsPage::onDeletePromotion() {
    QModelIndexList selected = tableView_->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    QString code = model_->item(selected.first().row(), 0)->text();
    if (QMessageBox::question(this, "Xác nhận", "Bạn có chắc muốn xóa khuyến mãi: " + code + "?") == QMessageBox::Yes) {
        promotions_.erase(std::remove_if(promotions_.begin(), promotions_.end(),
            [&](const Promotion& p){ return QString::fromStdString(p.getCode()) == code; }),
            promotions_.end());
        saveDataToFile();
        refreshPage();
    }
}