#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QLabel;
class AccountManager;

class LoginPage : public QWidget
{
    Q_OBJECT 

public:
    explicit LoginPage(AccountManager* accManager, QWidget *parent = nullptr);

signals:
    // Tín hiệu báo đăng nhập thành công
    void loginSuccess(const QString& role, const QString& token);
    
    // Tín hiệu yêu cầu chuyển sang trang Quên mật khẩu
    void forgotPasswordClicked();

private slots:
    void onLoginClicked();
    void onShowPasswordToggled(bool checked);
    void onForgotPasswordClicked();

private:
    void setupUi();
    void setupConnections();

    // Các thành phần UI
    QLineEdit *usernameEdit_;
    QLineEdit *passwordEdit_;
    QCheckBox *showPasswordCheck_;
    QPushButton *loginButton_;
    QLabel* forgotPasswordLabel_;
    QLabel* NotiLabel;

    AccountManager* accountManager_; 
};

#endif // LOGINPAGE_H