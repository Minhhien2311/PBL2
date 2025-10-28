// ======================================================================
// UI/pages/AccountsPage.cpp
// Trang "Quản lý tài khoản": tiêu đề + 2 cột × 3 hàng + 2 nút.
// ======================================================================

#include "UI/pages/AccountsPage.h"

// -------------------- ctor --------------------
AccountsPage::AccountsPage(const sf::Font &uiFont,
                           sf::Vector2f originTopLeft,
                           float contentWidth)
    : mFont(&uiFont),
      mOriginTL(originTopLeft),
      mContentW(contentWidth)
{
    // ===== Title =====
    mTitle.setFont(*mFont);
    mTitle.setCharacterSize(28);
    mTitle.setFillColor(sf::Color(28, 64, 110)); // #1C406E
    mTitle.setString(L"Thông tin tài khoản");

    // ===== Labels =====
    auto cfgLabel = [&](sf::Text &t, const wchar_t *s)
    {
        t.setFont(*mFont);
        t.setCharacterSize(18);
        t.setFillColor(sf::Color(28, 64, 110));
        t.setString(s);
    };
    cfgLabel(mLblId, L"ID tài khoản");
    cfgLabel(mLblFullName, L"Họ và tên");
    cfgLabel(mLblUsername, L"Tên tài khoản");
    cfgLabel(mLblPhone, L"Số điện thoại");
    cfgLabel(mLblRole, L"Chức vụ");
    cfgLabel(mLblEmail, L"Email");

    // ===== TextBoxes (CENTER-based) =====
    auto cfgEdit = [&](TextBox &tb, const wchar_t *placeholder)
    {
        tb.setFont(*mFont);
        tb.setCharacterSize(20);
        tb.setPlaceholder(placeholder);
        tb.setSize(mEditSize);
    };
    cfgEdit(mTbId, L"");
    cfgEdit(mTbFullName, L"Nhập họ và tên");
    cfgEdit(mTbUsername, L"Nhập tên tài khoản");
    cfgEdit(mTbPhone, L"Nhập số điện thoại");
    cfgEdit(mTbRole, L"Nhập chức vụ");
    cfgEdit(mTbEmail, L"Nhập email");

    // ===== Buttons (CENTER-based) =====
    mBtnUpdate.setSize(mBtnSize);
    mBtnUpdate.setBackgroundColor(sf::Color(96, 139, 193)); // primary
    mBtnUpdate.setText(L"Cập nhập thông tin", *mFont, 20);
    mBtnUpdate.setTextColor(sf::Color::White);

    mBtnChangePw.setSize(mBtnSize);
    mBtnChangePw.setBackgroundColor(sf::Color(96, 139, 193)); // rgba(96, 139, 193, 1)
    mBtnChangePw.setText(L"Đổi mật khẩu", *mFont, 20);
    mBtnChangePw.setTextColor(sf::Color::White);

    // Gắn hành vi & bố trí
    wireActions_();
    layout_();
}

// -------------------- public: event/update/draw --------------------
void AccountsPage::handleEvent(const sf::Event &ev, const sf::RenderWindow &win)
{
    // Nếu chỉ hiển thị: KHÔNG forward event cho 6 TextBox
    if (!mReadOnly)
    {
        mTbId.handleEvent(ev, win);
        mTbFullName.handleEvent(ev, win);
        mTbUsername.handleEvent(ev, win);
        mTbPhone.handleEvent(ev, win);
        mTbRole.handleEvent(ev, win);
        mTbEmail.handleEvent(ev, win);
    }

    // Nút "Cập nhập thông tin":
    if (!mReadOnly)
        mBtnUpdate.handleEvent(ev, win); // chỉ bật khi không read-only

    // Nút "Đổi mật khẩu" vẫn cho bấm
    mBtnChangePw.handleEvent(ev, win);
}

void AccountsPage::update(const sf::RenderWindow &win)
{
    // Button hover…
    if (!mReadOnly)
        mBtnUpdate.update(win);
    mBtnChangePw.update(win);
}

void AccountsPage::draw(sf::RenderTarget &target) const
{
    // Title
    target.draw(mTitle);

    // Labels
    target.draw(mLblId);
    target.draw(mLblFullName);
    target.draw(mLblUsername);
    target.draw(mLblPhone);
    target.draw(mLblRole);
    target.draw(mLblEmail);

    // Edits
    target.draw(mTbId);
    target.draw(mTbFullName);
    target.draw(mTbUsername);
    target.draw(mTbPhone);
    target.draw(mTbRole);
    target.draw(mTbEmail);

    // Buttons
    target.draw(mBtnUpdate);
    target.draw(mBtnChangePw);
}

// -------------------- public: form data --------------------
void AccountsPage::setForm(const AccountForm &f)
{
    mTbId.setText(f.id);
    mTbFullName.setText(f.fullName);
    mTbUsername.setText(f.username);
    mTbPhone.setText(f.phone);
    mTbRole.setText(f.role);
    mTbEmail.setText(f.email);
}

AccountsPage::AccountForm AccountsPage::getForm() const
{
    return {
        mTbId.getText(),
        mTbFullName.getText(),
        mTbUsername.getText(),
        mTbPhone.getText(),
        mTbRole.getText(),
        mTbEmail.getText()};
}

// -------------------- private: layout --------------------
void AccountsPage::layout_()
{
    // Khung khả dụng bên trong content (thêm margin trong)
    const float availLeft = mOriginTL.x + mMarginLR;
    const float availTop = mOriginTL.y + mMarginTB;
    const float availW = std::max(0.f, mContentW - 2.f * mMarginLR);

    // Cột trái/phải: căn theo center của mỗi cột
    const float colWidth = mEditSize.x;
    const float totalW = colWidth * 2.f + mColGap;
    const float startX = availLeft + std::max(0.f, (availW - totalW) * 0.5f);

    const float colLeftCX = startX + colWidth * 0.5f;
    const float colRightCX = startX + colWidth + mColGap + colWidth * 0.5f;

    float y = availTop;

    // Tiêu đề (top-left)
    mTitle.setPosition(startX, y);
    y += mTitle.getLocalBounds().height + mTitleGap;

    // Hàng 1: ID | Họ tên
    mCId = {colLeftCX, y + mEditSize.y * 0.5f};
    mCFullName = {colRightCX, y + mEditSize.y * 0.5f};
    mTbId.setPosition(mCId);
    mTbFullName.setPosition(mCFullName);
    placeLabelAbove_(mLblId, mCId);
    placeLabelAbove_(mLblFullName, mCFullName);
    y += mEditSize.y + mRowGap;

    // Hàng 2: Tên TK | SĐT
    mCUsername = {colLeftCX, y + mEditSize.y * 0.5f};
    mCPhone = {colRightCX, y + mEditSize.y * 0.5f};
    mTbUsername.setPosition(mCUsername);
    mTbPhone.setPosition(mCPhone);
    placeLabelAbove_(mLblUsername, mCUsername);
    placeLabelAbove_(mLblPhone, mCPhone);
    y += mEditSize.y + mRowGap;

    // Hàng 3: Chức vụ | Email
    mCRole = {colLeftCX, y + mEditSize.y * 0.5f};
    mCEmail = {colRightCX, y + mEditSize.y * 0.5f};
    mTbRole.setPosition(mCRole);
    mTbEmail.setPosition(mCEmail);
    placeLabelAbove_(mLblRole, mCRole);
    placeLabelAbove_(mLblEmail, mCEmail);
    y += mEditSize.y + mRowGap;

    // Nút: căn giữa theo 2 cột
    const float midX = (colLeftCX + colRightCX) * 0.5f;
    mBtnUpdate.setPosition({midX, y + mBtnSize.y * 0.5f});

    y += mBtnSize.y + 18.f;
    mBtnChangePw.setPosition({midX, y + mBtnSize.y * 0.5f});
}

void AccountsPage::placeLabelAbove_(sf::Text &label, const sf::Vector2f &tbCenter) const
{
    // TextBox center-based → label top-left đặt lệch lên mLabelDy
    const float left = tbCenter.x - mEditSize.x * 0.5f;
    const float top = tbCenter.y - mEditSize.y * 0.5f - mLabelDy;
    label.setPosition(left, top);
}

// -------------------- private: actions --------------------
void AccountsPage::wireActions_()
{
    if (!mReadOnly)
    {
        mBtnUpdate.setOnAction([this]()
                               {
            if (onSubmitUpdate) onSubmitUpdate(getForm()); });
    }
    else
    {
        mBtnUpdate.setOnAction(nullptr); // vô hiệu
    }

    mBtnChangePw.setOnAction([this]()
                             {
        if (onChangePassword) onChangePassword(); });
}
