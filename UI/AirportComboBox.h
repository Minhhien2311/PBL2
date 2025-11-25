#ifndef AIRPORTCOMBOBOX_H
#define AIRPORTCOMBOBOX_H

#include <QComboBox>
#include <string>

class AirportManager;

class AirportComboBox : public QComboBox {
    Q_OBJECT
    
private:
    AirportManager* airportManager_;
    
public:

    explicit AirportComboBox(AirportManager* airportManager, QWidget* parent = nullptr);
    
    std::string getSelectedIATA() const;
    
    bool setSelectedIATA(const std::string& iataCode);
    
    void refreshAirports();
};

#endif // AIRPORTCOMBOBOX_H
