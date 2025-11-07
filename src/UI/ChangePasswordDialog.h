#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H
#include <QDialog>

class AccountManager;

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT
public:
    // nhận con trỏ AccountManager* (không phải singleton) và userId (ID tài khoản cần đổi)
    explicit ChangePasswordDialog(AccountManager* mgr, const QString &userId, QWidget *parent = nullptr);
    ~ChangePasswordDialog();
private slots:
    void onCancelClicked();
private:
    AccountManager* accountManager_;
    QString m_userId;
};
#endif // CHANGEPASSWORDDIALOG_H
