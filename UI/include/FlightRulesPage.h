#pragma once

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class FlightManager;

class FlightRulesPage : public QWidget
{
    Q_OBJECT

public:
    explicit FlightRulesPage(FlightManager* fm, QWidget* parent = nullptr);
};