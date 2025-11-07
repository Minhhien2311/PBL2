
#include "ChangePasswordDialog.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include "core/AccountManager.h"
#include "entities/Account.h"
ChangePasswordDialog::ChangePasswordDialog(AccountManager* mgr, const QString &userId, QWidget *parent)
    : QDialog(parent), accountManager_(mgr), m_userId(userId)
{
    setWindowTitle(tr("Đổi mật khẩu"));
    setModal(true);
    QLabel *lblOld = new QLabel(tr("Mật khẩu hiện tại"));
    QLabel *lblNew = new QLabel(tr("Mật khẩu mới"));
    QLabel *lblConfirm = new QLabel(tr("Xác nhận mật khẩu"));
    QLineEdit *lineOld = new QLineEdit();
    QLineEdit *lineNew = new QLineEdit();
    QLineEdit *lineConfirm = new QLineEdit();
    lineOld->setEchoMode(QLineEdit::Password);
    lineNew->setEchoMode(QLineEdit::Password);
    lineConfirm->setEchoMode(QLineEdit::Password);
    QPushButton *btnOk = new QPushButton(tr("Đổi mật khẩu"));
    QPushButton *btnCancel = new QPushButton(tr("Hủy"));
    QGridLayout *g = new QGridLayout();
    g->addWidget(lblOld, 0, 0);
    g->addWidget(lineOld, 0, 1);
    g->addWidget(lblNew, 1, 0);
    g->addWidget(lineNew, 1, 1);
    g->addWidget(lblConfirm, 2, 0);
    g->addWidget(lineConfirm, 2, 1);
    QHBoxLayout *hbtn = new QHBoxLayout();
    hbtn->addStretch();
    hbtn->addWidget(btnOk);
    hbtn->addWidget(btnCancel);
    QVBoxLayout *main = new QVBoxLayout(this);
    main->addLayout(g);
    main->addLayout(hbtn);
    connect(btnCancel, &QPushButton::clicked, this, &ChangePasswordDialog::onCancelClicked);
    connect(btnOk, &QPushButton::clicked, this, [=]() {
    QString oldp = lineOld->text();
    QString newp = lineNew->text();
    QString confirm = lineConfirm->text();
    if (newp != confirm) {
    QMessageBox::warning(this, tr("Lỗi"), tr("Mật khẩu mới và xác nhận không khớp"));
    return;
    }
    if (oldp.isEmpty() || newp.isEmpty()) {
    QMessageBox::warning(this, tr("Lỗi"), tr("Vui lòng nhập đầy đủ thông tin"));
    return;
    }
    // Lấy user hiện tại từ AccountManager
    Account* current = accountManager_->getCurrentUser();
    if (!current) {
    QMessageBox::warning(this, tr("Lỗi"), tr("Không có người dùng đang đăng nhập"));
    return;
    }
    // Kiểm tra user id trùng với current (AccountsPage gọi dialog theo current user)
    if (QString::fromStdString(current->getId()) != m_userId) {
    QMessageBox::warning(this, tr("Lỗi"), tr("Không thể đổi mật khẩu cho user này"));
    return;
    }
    // Kiểm tra mật khẩu hiện tại
    if (!current->authenticate(oldp.toStdString())) {
    QMessageBox::warning(this, tr("Lỗi"), tr("Mật khẩu hiện tại không đúng"));
    return;
    }
    // Gọi AccountManager thay đổi mật khẩu (hàm changePassword sẽ tìm admin/agent, cập nhật object và ghi file)
    bool ok = accountManager_->changePassword(m_userId.toStdString(), oldp.toStdString(), newp.toStdString());
    if (ok) {
    QMessageBox::information(this, tr("Thành công"), tr("Đổi mật khẩu thành công"));
    accept();
    } else {
    QMessageBox::critical(this, tr("Lỗi"), tr("Đổi mật khẩu thất bại"));
    }
    });
}
ChangePasswordDialog::~ChangePasswordDialog()
{
}
void ChangePasswordDialog::onCancelClicked()
{
    reject();
}