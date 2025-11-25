#ifndef FORGOTPASSWORDPAGE_H
#define FORGOTPASSWORDPAGE_H

#include <QWidget>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QLabel;
class AccountManager; // Khai báo Forward declaration

class ForgotPasswordPage : public QWidget
{
    Q_OBJECT

public:
    // Nhận AccountManager để tự xử lý logic reset
    explicit ForgotPasswordPage(AccountManager* accManager, QWidget *parent = nullptr);
    ~ForgotPasswordPage();

    // Hàm xóa trắng các ô nhập liệu (gọi khi chuyển trang đến đây)
    void clearFields();

signals:
    // Tín hiệu báo cho MainWindow biết để chuyển lại trang Login
    void backToLogin();

private slots:
    void onResetClicked();
    void onBackClicked();
    void onShowPasswordToggled(bool checked);

private:
    void setupUi();
    void setupConnections();

    // UI Elements
    QLineEdit *emailEdit_;
    QLineEdit *newPassEdit_;
    QLineEdit *confirmPassEdit_;
    QCheckBox *showPasswordCheck_;
    QPushButton *resetButton_;
    QLabel *backLabel_; 
    QLabel *notiLabel_; 

    AccountManager* accountManager_;
};

#endif // FORGOTPASSWORDPAGE_H