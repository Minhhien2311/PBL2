#ifndef FORGOTPASSWORDPAGE_H
#define FORGOTPASSWORDPAGE_H

#include <QWidget>

class AccountManager;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class ForgotPasswordPage : public QWidget
{
    Q_OBJECT

public:
    explicit ForgotPasswordPage(AccountManager* accManager, QWidget *parent = nullptr);
    ~ForgotPasswordPage();

    void clearFields();

signals:
    void backToLogin();

private slots:
    void onResetClicked();
    void onBackClicked();
    void onShowPasswordToggled(bool checked);

private:
    void setupUi();
    void setupConnections();

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