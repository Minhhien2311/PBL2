#include "FlightRulesPage.h"
#include "core/FlightManager.h"

FlightRulesPage::FlightRulesPage(FlightManager* fm, QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* label = new QLabel("Giao diện Quản lý luật bay sẽ được phát triển sau.", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18px; color: #888;");
    layout->addWidget(label);
}