#include "AirportComboBox.h"

#include "core/AirportManager.h"

#include <QString>

// ========== CONSTRUCTOR ==========

AirportComboBox::AirportComboBox(AirportManager* airportManager, QWidget* parent)
    : QComboBox(parent), airportManager_(airportManager)
{
    setMinimumHeight(30);
    setMaxVisibleItems(12);
    
    refreshAirports();
    setCurrentIndex(0);
}

// ========== PUBLIC METHODS ==========

std::string AirportComboBox::getSelectedIATA() const
{
    if (!airportManager_) return "";
    
    if (currentIndex() <= 0) { 
        return ""; 
    }
    
    QString displayName = currentText();
    if (displayName.isEmpty()) return "";
    
    return airportManager_->getIATACode(displayName.toStdString());
}

bool AirportComboBox::setSelectedIATA(const std::string& iataCode)
{
    if (!airportManager_) return false;
    
    if (iataCode.empty()) {
        setCurrentIndex(0);
        return true;
    }
    
    std::string displayName = airportManager_->getDisplayName(iataCode);
    if (displayName.empty()) return false;
    
    int index = findText(QString::fromStdString(displayName));
    if (index >= 0) {
        setCurrentIndex(index);
        return true;
    }
    return false;
}

void AirportComboBox::refreshAirports()
{
    std::string currentIATA = getSelectedIATA();
    
    clear();
    
    addItem("Tất cả sân bay");
    
    if (!airportManager_) return;
    
    std::vector<std::string> displayNames = airportManager_->getAllDisplayNames();
    for (int i = 0; i < displayNames.size(); ++i) {
        addItem(QString::fromStdString(displayNames[i]));
    }
    
    setSelectedIATA(currentIATA);
}