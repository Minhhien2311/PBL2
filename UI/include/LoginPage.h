#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

class AccountManager;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(AccountManager* accManager, QWidget *parent = nullptr);

signals:
    void loginSuccess(const QString& role, const QString& token);
    void forgotPasswordClicked();

private slots:
    void onLoginClicked();
    void onShowPasswordToggled(bool checked);
    void onForgotPasswordClicked();

private:
    void setupUi();
    void setupConnections();

    QLineEdit *usernameEdit_;
    QLineEdit *passwordEdit_;
    QCheckBox *showPasswordCheck_;
    QPushButton *loginButton_;
    QLabel* forgotPasswordLabel_;
    QLabel* NotiLabel;

    AccountManager* accountManager_;
};

#endif // LOGINPAGE_H