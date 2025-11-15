#ifndef FORGOTPASSWORDDIALOG_H
#define FORGOTPASSWORDDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class ForgotPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ForgotPasswordDialog(QWidget *parent = nullptr);

    QString getEmail() const;
    QString getNewPassword() const;

private slots:
    void onResetClicked();
    void onCancelClicked();

private:
    QLineEdit* emailEdit_;
    QLineEdit* newPasswordEdit_;
    QLineEdit* confirmPasswordEdit_;
    QPushButton* resetButton_;
    QPushButton* cancelButton_;
};

#endif // FORGOTPASSWORDDIALOG_H