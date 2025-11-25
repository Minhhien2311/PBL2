#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class AccountManager;

class AgentListPage : public QWidget
{
    Q_OBJECT
public:
    explicit AgentListPage(AccountManager* am, QWidget* parent = nullptr);
};