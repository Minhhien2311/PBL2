#include "AgentListPage.h"
#include "core/AccountManager.h"

AgentListPage::AgentListPage(AccountManager* am, QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* label = new QLabel("Giao diện Danh sách đại lý sẽ được phát triển sau.", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18px; color: #888;");
    layout->addWidget(label);
}