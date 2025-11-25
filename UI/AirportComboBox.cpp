#include "AirportComboBox.h"
#include "core/AirportManager.h"
#include <QString>

AirportComboBox::AirportComboBox(AirportManager* airportManager, QWidget* parent)
    : QComboBox(parent), airportManager_(airportManager)
{
    setMinimumHeight(30);
    setMaxVisibleItems(12);
    
    // Refresh xong thì mặc định chọn cái đầu tiên (là "Tất cả")
    refreshAirports();
    setCurrentIndex(0); 
}

std::string AirportComboBox::getSelectedIATA() const
{
    if (!airportManager_) return "";
    
    // [QUAN TRỌNG] Nếu đang chọn dòng đầu tiên (Tất cả sân bay) -> Trả về rỗng
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
    
    // Nếu truyền vào rỗng -> Reset về dòng "Tất cả" (Index 0)
    if (iataCode.empty()) {
        setCurrentIndex(0);
        return true;
    }
    
    std::string displayName = airportManager_->getDisplayName(iataCode);
    if (displayName.empty()) return false;
    
    // Tìm text trong combobox
    int index = findText(QString::fromStdString(displayName));
    if (index >= 0) {
        setCurrentIndex(index);
        return true;
    }
    return false;
}

void AirportComboBox::refreshAirports()
{
    // Lưu lựa chọn hiện tại (IATA code)
    std::string currentIATA = getSelectedIATA();
    
    // Xóa hết cũ
    clear();
    
    // [QUAN TRỌNG] Thêm dòng mặc định ở đầu tiên
    addItem("Tất cả sân bay"); 
    
    if (!airportManager_) return;
    
    // Thêm các sân bay thực tế vào sau
    std::vector<std::string> displayNames = airportManager_->getAllDisplayNames();
    for (int i = 0; i < displayNames.size(); ++i) {
        addItem(QString::fromStdString(displayNames[i]));
    }
    
    // Khôi phục lại lựa chọn cũ
    // Nếu lúc trước đang chọn 1 sân bay cụ thể, set lại nó
    // Nếu lúc trước chọn "Tất cả" (currentIATA rỗng), thì hàm setSelectedIATA("") sẽ set về 0
    setSelectedIATA(currentIATA);
}