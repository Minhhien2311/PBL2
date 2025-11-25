#ifndef ACCOUNTSPAGE_H
#define ACCOUNTSPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QShowEvent>

class AccountManager;

class AccountsPage : public QWidget {
    Q_OBJECT 
public:
    explicit AccountsPage(AccountManager* accManager, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    
private:
    AccountManager* accountManager_;

    // --- Section 1: Thông tin tài khoản ---
    QLineEdit* idEdit_;
    QLineEdit* fullnameEdit_;
    QLineEdit* userEdit_;     // Tên đăng nhập
    QLineEdit* phoneEdit_;
    QLineEdit* roleEdit_;
    QLineEdit* emailEdit_;
    QPushButton* btnUpdateInfo_; // Nút "Sửa thông tin" / "Lưu thông tin"

    // --- Section 2: Đổi mật khẩu (Inline) ---
    QLineEdit* currentPassEdit_;
    QLineEdit* newPassEdit_;
    QLineEdit* confirmPassEdit_;
    QPushButton* btnChangePass_; // Nút "Đổi mật khẩu"

    // Helper functions
    void setupUi();
    void setupConnections();
    void loadAccountData(); 
    
    // Hàm helper tạo widget con (Label ở trên, Input ở dưới)
    QWidget* createFieldWidget(const QString& labelText, QWidget* inputWidget, bool isMandatory = false);
    
    // Hàm xử lý logic
    void onUpdateInfoClicked();
    void onChangePasswordClicked();
};

#endif // ACCOUNTSPAGE_H