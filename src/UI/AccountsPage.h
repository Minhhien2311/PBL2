#ifndef ACCOUNTSPAGE_H
#define ACCOUNTSPAGE_H

#include <QWidget>
#include <qpushbutton.h>

class AccountManager; // <--- Sửa lỗi: Khai báo sớm
class QLineEdit;      // <--- Sửa lỗi: Khai báo sớm

class AccountsPage : public QWidget {
    Q_OBJECT 
public:
    // <--- Sửa lỗi: Constructor nhận AccountManager
    explicit AccountsPage(AccountManager* accManager, QWidget* parent=nullptr);
    void refreshAccountData(); // Hàm public để reload dữ liệu
    
private:
    AccountManager* accountManager_; // <--- Sửa lỗi: Thêm con trỏ

    // <--- Sửa lỗi: Thêm con trỏ UI làm biến thành viên
    QLineEdit* idEdit_;
    QLineEdit* userEdit_;
    QLineEdit* roleEdit_;
    QLineEdit* fullnameEdit_;
    QLineEdit* phoneEdit_;
    QLineEdit* emailEdit_;
    QPushButton* btnUpdate_;

    void loadAccountData(); // Hàm helper để tải/làm mới
};

#endif // ACCOUNTSPAGE_H

