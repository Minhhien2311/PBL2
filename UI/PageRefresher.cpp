#include "PageRefresher.h"
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QList>

void PageRefresher::clearSearchFields(QWidget* container)
{
    if (!container) return;
    
    // Clear all QLineEdit widgets
    QList<QLineEdit*> lineEdits = container->findChildren<QLineEdit*>();
    for (QLineEdit* edit : lineEdits) {
        edit->clear();
    }
    
    // Reset all QComboBox to index 0
    QList<QComboBox*> comboBoxes = container->findChildren<QComboBox*>();
    for (QComboBox* combo : comboBoxes) {
        combo->setCurrentIndex(0);
    }
    
    // Clear all QDateEdit widgets
    QList<QDateEdit*> dateEdits = container->findChildren<QDateEdit*>();
    for (QDateEdit* date : dateEdits) {
        date->clear();
    }
}

QPushButton* PageRefresher::createRefreshButton(QWidget* parent, const QString& text)
{
    QPushButton* button = new QPushButton(text, parent);
    button->setStyleSheet(
        "QPushButton { background:#5886C0; color:white; border:none; "
        "border-radius:6px; height:32px; padding:0 16px; font-weight:600; }"
        "QPushButton:hover { background:#466a9a; }"
    );
    button->setCursor(Qt::PointingHandCursor);
    button->setMinimumWidth(140);
    return button;
}

void PageRefresher::executeRefresh(std::function<void()> callback)
{
    if (callback) {
        callback();
    }
}