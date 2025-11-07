#include "PromotionsPage.h"

PromotionsPage::PromotionsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* label = new QLabel("Giao diện Quản lý khuyến mãi sẽ được phát triển sau.", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18px; color: #888;");
    layout->addWidget(label);
}