#include "AirportComboBox.h"
#include "core/AirportManager.h"
#include "DSA/DynamicArray.h"
#include <QString>

AirportComboBox::AirportComboBox(AirportManager* airportManager, QWidget* parent)
    : QComboBox(parent), airportManager_(airportManager)
{
    setMinimumHeight(30);
    setMaxVisibleItems(12);
    refreshAirports();
}

std::string AirportComboBox::getSelectedIATA() const
{
    if (!airportManager_) return "";
    
    QString displayName = currentText();
    if (displayName.isEmpty()) return "";
    
    return airportManager_->getIATACode(displayName.toStdString());
}

bool AirportComboBox::setSelectedIATA(const std::string& iataCode)
{
    if (!airportManager_) return false;
    
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
    // Lưu lựa chọn hiện tại
    std::string currentIATA = getSelectedIATA();
    
    // Xóa tất cả items
    clear();
    
    if (!airportManager_) return;
    
    // Thêm tất cả sân bay (đã được sắp xếp alphabet trong AirportManager)
    DynamicArray<std::string> displayNames = airportManager_->getAllDisplayNames();
    for (int i = 0; i < displayNames.size(); ++i) {
        addItem(QString::fromStdString(displayNames[i]));
    }
    
    // Khôi phục lựa chọn cũ nếu có
    if (!currentIATA.empty()) {
        setSelectedIATA(currentIATA);
    }
}
