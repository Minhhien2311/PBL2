#ifndef ROUTEDIALOG_H
#define ROUTEDIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QPushButton;
class AirportComboBox;
class AirportManager;
class QComboBox;

class RouteDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructor cho chế độ THÊM MỚI
    explicit RouteDialog(AirportManager* airportManager, QWidget* parent = nullptr);
    
    // Constructor cho chế độ SỬA (truyền dữ liệu hiện tại vào)
    RouteDialog(AirportManager* airportManager,
                const QString& currentId,
                const QString& currentFrom,
                const QString& currentTo,
                QWidget* parent = nullptr);

    // Getters để lấy dữ liệu sau khi dialog đóng
    QString getFromIATA() const;
    QString getToIATA() const;

private slots:
    void onAccept();

private:
    void setupUi(bool isEditMode);

    AirportManager* airportManager_;
    bool isEditMode_;
    QString currentId_;  // Chỉ dùng khi edit

    // UI Components
    QComboBox* airlineCombo_;
    AirportComboBox* fromCombo_;
    AirportComboBox* toCombo_;
    QPushButton* saveButton_;
    QPushButton* cancelButton_;
};

#endif // ROUTEDIALOG_H