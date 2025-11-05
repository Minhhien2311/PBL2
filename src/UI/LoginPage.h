#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

class QLineEdit;
class QCheckBox;
class QPushButton;
class AccountManager; // <--- Sửa lỗi: Khai báo sớm (Forward declare)

class LoginPage : public QWidget
{
    Q_OBJECT 

public:
    // <--- Sửa lỗi: Constructor nhận AccountManager
    explicit LoginPage(AccountManager* accManager, QWidget *parent = nullptr);

signals:
    void loginSuccess(const QString& role, const QString& token);

private slots:
    void onLoginClicked();
    void onShowPasswordToggled(bool checked);

private:
    void setupUi();
    void setupConnections();

    // Các thành phần UI
    QLineEdit *usernameEdit_;
    QLineEdit *passwordEdit_;
    QCheckBox *showPasswordCheck_;
    QPushButton *loginButton_;

    // <--- Sửa lỗi: Thêm con trỏ tới Manager
    AccountManager* accountManager_; 
};

#endif // LOGINPAGE_H

