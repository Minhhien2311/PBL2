#ifndef ACCOUNTSPAGE_H
#define ACCOUNTSPAGE_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShowEvent>
#include <QWidget>

class AccountManager;

class AccountsPage : public QWidget {
    Q_OBJECT
public:
    explicit AccountsPage(AccountManager* accManager, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private:
    AccountManager* accountManager_;

    // ========== ACCOUNT INFORMATION SECTION ==========
    QLineEdit* idEdit_;
    QLineEdit* fullnameEdit_;
    QLineEdit* userEdit_;
    QLineEdit* phoneEdit_;
    QLineEdit* roleEdit_;
    QLineEdit* emailEdit_;
    QPushButton* btnUpdateInfo_;

    // ========== PASSWORD CHANGE SECTION ==========
    QLineEdit* currentPassEdit_;
    QLineEdit* newPassEdit_;
    QLineEdit* confirmPassEdit_;
    QPushButton* btnChangePass_;

    void setupUi();
    void setupConnections();
    void loadAccountData();
    QWidget* createFieldWidget(const QString& labelText, QWidget* inputWidget, bool isMandatory = false);
    void onUpdateInfoClicked();
    void onChangePasswordClicked();
};

#endif