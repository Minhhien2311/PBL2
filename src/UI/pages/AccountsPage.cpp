#include "UI/pages/AccountsPage.h"

// ============= Constructor =============
AccountsPage::AccountsPage(const sf::Font &uiFont, sf::Vector2f origin, float cw)
    : mFont(uiFont), mOrigin(origin), mContentW(cw)
{
    // ---- Title ----
    mTitle.setFont(mFont);
    mTitle.setCharacterSize(30);
    mTitle.setFillColor(sf::Color(28, 64, 110)); // #1C406E
    mTitle.setString(L"Thông tin tài khoản");

    // ---- Label factory ----
    auto makeLabel = [&](sf::Text &t, const sf::String &s)
    {
        t.setFont(mFont);
        t.setCharacterSize(18);
        t.setFillColor(sf::Color(28, 64, 110));
        t.setString(s);
    };
    makeLabel(mLblId, L"ID tài khoản");
    makeLabel(mLblFullname, L"Họ và tên");
    makeLabel(mLblUsername, L"Tên tài khoản");
    makeLabel(mLblPhone, L"Số điện thoại");
    makeLabel(mLblRole, L"Chức vụ");
    makeLabel(mLblEmail, L"Email");

    // ---- TextBox style ----
    auto prepBox = [&](TextBox &tb, const sf::String &placeholder)
    {
        tb.setFont(mFont); // quan trọng: setFont cho mọi textbox
        tb.setCharacterSize(18);
        tb.setPlaceholder(placeholder);
        tb.setSize({420.f, 40.f}); // width/height của ô nhập
    };
    prepBox(mTxtId, L"");
    prepBox(mTxtFullname, L"");
    prepBox(mTxtUsername, L"");
    prepBox(mTxtPhone, L"");
    prepBox(mTxtRole, L"");
    prepBox(mTxtEmail, L"");

    // ---- Buttons ----
    mBtnSave.setSize({260.f, 52.f});
    mBtnSave.setText(L"Cập nhập thông tin", mFont, 20);
    mBtnSave.setBackgroundColor(sf::Color(62, 108, 179));
    mBtnSave.setTextColor(sf::Color::White);
    mBtnSave.setOnAction([this]
                         {
        if (mOnSave) mOnSave(getProfile()); });

    mBtnChangePw.setSize({260.f, 52.f});
    mBtnChangePw.setText(L"Đổi mật khẩu", mFont, 20);
    mBtnChangePw.setBackgroundColor(sf::Color(98, 135, 184));
    mBtnChangePw.setTextColor(sf::Color::White);
    mBtnChangePw.setOnAction([this]
                             {
        if (mOnChangePw) mOnChangePw(); });

    // ---- Form outline ----
    mFormOutline.setFillColor(sf::Color(0, 0, 0, 0));
    mFormOutline.setOutlineThickness(2.f);
    mFormOutline.setOutlineColor(sf::Color(28, 64, 110));

    // ---- Layout tuyệt đối ----
    layout();
}

// ============= Layout tuyệt đối =============
void AccountsPage::layout()
{
    // Mốc layout — tune theo ảnh của bạn
    const float left = mOrigin.x + 40.f;      // mép trái content
    const float top = mOrigin.y + 10.f;       // mép trên
    const float colW = 420.f;                 // width textbox
    const float colGap = 120.f;               // gap 2 cột
    const float col2X = left + colW + colGap; // x cột phải
    const float rowH = 78.f;                  // mỗi hàng (label + box)
    const float yForm = top + 56.f;           // bắt đầu form dưới title

    // Title
    mTitle.setPosition(left, top);

    // ----- Hàng 1: ID & Họ tên -----
    mLblId.setPosition({left, yForm});
    mTxtId.setPosition({left + colW * 0.5f, yForm + 30.f}); // textbox theo TÂM

    mLblFullname.setPosition({col2X, yForm});
    mTxtFullname.setPosition({col2X + colW * 0.5f, yForm + 30.f});

    // ----- Hàng 2: Username & Phone -----
    float y2 = yForm + rowH;
    mLblUsername.setPosition({left, y2});
    mTxtUsername.setPosition({left + colW * 0.5f, y2 + 30.f});

    mLblPhone.setPosition({col2X, y2});
    mTxtPhone.setPosition({col2X + colW * 0.5f, y2 + 30.f});

    // ----- Hàng 3: Role & Email -----
    float y3 = y2 + rowH;
    mLblRole.setPosition({left, y3});
    mTxtRole.setPosition({left + colW * 0.5f, y3 + 30.f});

    mLblEmail.setPosition({col2X, y3});
    mTxtEmail.setPosition({col2X + colW * 0.5f, y3 + 30.f});

    // ----- Buttons -----
    float centerX = left + (colW + colGap + colW) * 0.5f;
    mBtnSave.setPosition({centerX, y3 + 90.f});
    mBtnChangePw.setPosition({centerX, y3 + 90.f + 72.f});

    // ----- Outline bao quanh form -----
    const float formLeft = left - 14.f;
    const float formTop = yForm - 26.f;
    const float formWidth = (colW * 2.f + colGap) + 28.f;
    const float formHeight = (rowH * 3.f) + 26.f + 90.f + 72.f + 30.f;
    mFormOutline.setSize({formWidth, formHeight});
    mFormOutline.setPosition({formLeft, formTop});
}

// ============= Data binding =============
void AccountsPage::setInitialProfile(const Profile &p)
{
    mTxtId.setText(p.id);
    mTxtFullname.setText(p.fullName);
    mTxtUsername.setText(p.username);
    mTxtPhone.setText(p.phone);
    mTxtRole.setText(p.role);
    mTxtEmail.setText(p.email);
}

AccountsPage::Profile AccountsPage::getProfile() const
{
    Profile p;
    p.id = mTxtId.getText();
    p.fullName = mTxtFullname.getText();
    p.username = mTxtUsername.getText();
    p.phone = mTxtPhone.getText();
    p.role = mTxtRole.getText();
    p.email = mTxtEmail.getText();
    return p;
}

// ============= Loop methods =============
void AccountsPage::handleEvent(const sf::Event &e, const sf::RenderWindow &win)
{
    // Forward cho các control
    mTxtId.handleEvent(e, win);
    mTxtFullname.handleEvent(e, win);
    mTxtUsername.handleEvent(e, win);
    mTxtPhone.handleEvent(e, win);
    mTxtRole.handleEvent(e, win);
    mTxtEmail.handleEvent(e, win);

    mBtnSave.handleEvent(e, win);
    mBtnChangePw.handleEvent(e, win);
}

void AccountsPage::update(const sf::RenderWindow &win)
{
    // Nếu TextBox của em cần dt: đổi thành update(dt). Ở đây cho 0s để tương thích.
    mTxtId.update(sf::seconds(0.f));
    mTxtFullname.update(sf::seconds(0.f));
    mTxtUsername.update(sf::seconds(0.f));
    mTxtPhone.update(sf::seconds(0.f));
    mTxtRole.update(sf::seconds(0.f));
    mTxtEmail.update(sf::seconds(0.f));

    mBtnSave.update(win);
    mBtnChangePw.update(win);
}

// Overload cho trường hợp State gọi update(dt)
void AccountsPage::update(sf::Time /*dt*/)
{
    // Không cần gì thêm — phần lớn control của em không phụ thuộc dt
}

void AccountsPage::draw(sf::RenderTarget &rt) const
{
    rt.draw(mTitle);

    rt.draw(mFormOutline);
    rt.draw(mLblId);
    rt.draw(mLblFullname);
    rt.draw(mLblUsername);
    rt.draw(mLblPhone);
    rt.draw(mLblRole);
    rt.draw(mLblEmail);

    rt.draw(mTxtId);
    rt.draw(mTxtFullname);
    rt.draw(mTxtUsername);
    rt.draw(mTxtPhone);
    rt.draw(mTxtRole);
    rt.draw(mTxtEmail);

    rt.draw(mBtnSave);
    rt.draw(mBtnChangePw);
}
