#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class FlightManager;

class FlightRulesPage : public QWidget
{
    Q_OBJECT
public:
    explicit FlightRulesPage(FlightManager* fm, QWidget* parent = nullptr);
};