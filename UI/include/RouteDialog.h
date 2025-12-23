#ifndef ROUTEDIALOG_H
#define ROUTEDIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QPushButton;
class AirportComboBox;
class AirportManager;
class QComboBox;
class QLabel;

class RouteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RouteDialog(AirportManager* airportManager, QWidget* parent = nullptr);
    
    RouteDialog(AirportManager* airportManager,
                const QString& currentId,
                const QString& currentFrom,
                const QString& currentTo,
                QWidget* parent = nullptr);

    QString getFromIATA() const;
    QString getToIATA() const;

private slots:
    void onAccept();

private:
    void setupUi(bool isEditMode);

    AirportManager* airportManager_;
    bool isEditMode_;
    QString currentId_;

    // ========== UI COMPONENTS ==========
    AirportComboBox* fromCombo_;
    AirportComboBox* toCombo_;
    QPushButton* saveButton_;
    QPushButton* cancelButton_;
public:
    QLabel* statusLabel_;
};

#endif // ROUTEDIALOG_H