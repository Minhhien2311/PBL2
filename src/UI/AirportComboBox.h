#ifndef AIRPORTCOMBOBOX_H
#define AIRPORTCOMBOBOX_H

#include <QComboBox>
#include <string>

class AirportManager;

/**
 * @brief Widget dropdown để chọn sân bay
 * @details Kế thừa QComboBox, hiển thị tên sân bay nhưng làm việc với mã IATA
 */
class AirportComboBox : public QComboBox {
    Q_OBJECT
    
private:
    AirportManager* airportManager_;
    
public:
    /**
     * @brief Constructor
     * @param airportManager Con trỏ đến AirportManager
     * @param parent Widget cha
     */
    explicit AirportComboBox(AirportManager* airportManager, QWidget* parent = nullptr);
    
    /**
     * @brief Lấy mã IATA của sân bay đang chọn
     * @return Mã IATA (VD: "SGN"), hoặc chuỗi rỗng nếu không có lựa chọn
     */
    std::string getSelectedIATA() const;
    
    /**
     * @brief Chọn sân bay theo mã IATA
     * @param iataCode Mã IATA cần chọn
     * @return true nếu chọn thành công, false nếu không tìm thấy
     */
    bool setSelectedIATA(const std::string& iataCode);
    
    /**
     * @brief Làm mới danh sách sân bay
     * @details Load lại danh sách từ AirportManager, giữ nguyên lựa chọn hiện tại nếu có
     */
    void refreshAirports();
};

#endif // AIRPORTCOMBOBOX_H
