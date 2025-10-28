// UI/states/LoginState.cpp
#include "UI/states/LoginState.h"
#include "UI/states/AdminShellState.h"
#include <SFML/System/Utf.hpp>
#include <iterator>
#include "core/App.h"
#include <iostream>
#include <memory>

static inline std::string toUtf8(const sf::String &s)
{
    std::string out;
    sf::Utf32::toUtf8(s.begin(), s.end(), std::back_inserter(out));
    return out;
}

LoginState::LoginState(App &app) : UI::State(app)
{
    // ==== Title ====
    mTitle.setFont(mApp.getFont("Mulish-Bold"));
    mTitle.setString(L"PHẦN MỀM QUẢN LÝ BÁN VÉ MÁY BAY");
    mTitle.setCharacterSize(40);
    mTitle.setFillColor(sf::Color(29, 78, 137)); // xanh đậm

    // ==== Panel ====
    mPanel.setFillColor(sf::Color(255, 255, 255));
    mPanel.setOutlineThickness(2.f);
    mPanel.setOutlineColor(sf::Color(44, 82, 130));

    // ==== Labels ====
    mUsernameLabel.setFont(mApp.getFont("Mulish-Bold"));
    mUsernameLabel.setCharacterSize(26);
    mUsernameLabel.setString(L"Đăng nhập");
    mUsernameLabel.setFillColor(sf::Color(44, 82, 130));

    mPasswordLabel.setFont(mApp.getFont("Mulish-Regular"));
    mPasswordLabel.setCharacterSize(14);
    mPasswordLabel.setString(L"Hiển thị mật khẩu"); // nếu bạn có checkbox

    mForgotText.setFont(mApp.getFont("Mulish-Regular"));
    mForgotText.setCharacterSize(16);
    mForgotText.setString(L"Quên mật khẩu?");
    mForgotText.setFillColor(sf::Color(100, 120, 160));

    mErrorText.setFont(mApp.getFont("Mulish-Regular"));
    mErrorText.setCharacterSize(14);
    mErrorText.setString(L"Tên đăng nhập hoặc mật khẩu không đúng");
    mErrorText.setFillColor(sf::Color(200, 48, 48));

    // ==== Username / Password boxes ====
    // NOTE: tuỳ API TextBox của bạn. Dưới đây là ví dụ thường gặp.
    mUsernameBox.setFont(mApp.getFont("Mulish-Regular"));
    mPasswordBox.setFont(mApp.getFont("Mulish-Regular"));
    mUsernameBox.setCharacterSize(18);
    mPasswordBox.setCharacterSize(18);

    mUsernameBox.setPlaceholder(L"Username");
    mPasswordBox.setPlaceholder(L"Password");
    mPasswordBox.setPasswordMode(true);

    // --- Checkbox "Hiển thị mật khẩu" ---
    mShowPw.setLabel(L"Hiển thị mật khẩu", mApp.getFont("Mulish-Regular"), 16);
    mShowPw.setChecked(false);
    mShowPw.setOnChanged([this](bool on)
                         {
        // on == true -> hiện mật khẩu (không *); off -> ẩn (dùng *)
        mPasswordBox.setPasswordMode(!on);

        // giữ nguyên nội dung đã gõ (để cập nhật hiển thị)
        mPasswordBox.setText(mPasswordBox.getText()); });

    // ==== Button ====
    mLoginBtn.setText(L"Đăng nhập", mApp.getFont("Mulish-Bold"), 18);
    mLoginBtn.setBackgroundColor(sf::Color(66, 120, 190));
    mLoginBtn.setTextColor(sf::Color::White);

    mLoginBtn.setOnAction([this]
                          { 
        // 1) Lấy input
        const std::string user = toUtf8(mUsernameBox.getText());
        const std::string pass = toUtf8(mPasswordBox.getText());

        

        // 2) Gọi AccountManager::login
        // Đổi accountMgr() thành getter bạn đang có (getAccountManager()...) nếu khác tên.
        bool ok = mApp.accountMgr().login(user, pass);
        if (!ok)
        {
            // Display error message
            sf::FloatRect eb = mErrorText.getLocalBounds();
            mErrorText.setOrigin(eb.left + eb.width / 2.f, eb.top + eb.height / 2.f);
            mErrorText.setPosition(mPanel.getPosition().x, mLoginBtn.getPosition().y + mLoginBtn.getPosition().y + 10.f); // Position below login button
        }

        // 3) (Tối giản) Đăng nhập OK -> vào AdminShellState
        if (ok)
            mApp.goTo(AppRoute::AdminHome); });

    relayout(mApp.window().getSize());
}

void LoginState::handleInput(sf::Event &e)
{
    // Gửi event cho components (tùy API của bạn)
    mUsernameBox.handleEvent(e, mApp.window());
    mShowPw.handleEvent(e, mApp.window());
    mPasswordBox.handleEvent(e, mApp.window());
    mLoginBtn.handleEvent(e, mApp.window());

    // // Enter để đăng nhập luôn
    // if (e.type == sf::Event::KeyPressed || e.key.code == sf::Keyboard::Enter)
    // {
    //     onLogin_();
    // }
}

void LoginState::update(sf::Time dt)
{
    // Cho button/inputs update (hover animation, caret blink, v.v.)
    mUsernameBox.update(dt);
    mPasswordBox.update(dt);
    mLoginBtn.update(mApp.window());
}

void LoginState::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    (void)states;
    // Vẽ title
    target.draw(mTitle);
    // Vẽ panel khung
    target.draw(mPanel);
    // Vẽ label “Đăng nhập”
    target.draw(mUsernameLabel);

    // Vẽ input + button + link
    target.draw(mUsernameBox);
    target.draw(mPasswordBox);
    target.draw(mLoginBtn);
    target.draw(mForgotText);
    target.draw(mShowPw);
}

void LoginState::relayout(sf::Vector2u win)
{
    // Căn giữa title
    sf::FloatRect tb = mTitle.getLocalBounds();
    mTitle.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    mTitle.setPosition(win.x / 2.f, 80.f); // 80 = khoảng cách mép trên

    // Căn giữa màn hình theo win size hiện tại
    const float cx = win.x / 2.f;
    const float panelW = 520.f, panelH = 360.f;
    const float boxW = 360.f, boxH = 40.f;
    const float gap = 24.f;

    // Panel khung form đăng nhập
    mPanel.setSize({panelW, panelH});
    mPanel.setOrigin(panelW / 2.f, panelH / 2.f);
    mPanel.setPosition(cx, win.y * 0.55f);

    // Header "Đăng nhập"
    sf::FloatRect hb = mUsernameLabel.getLocalBounds(); // tuỳ tên biến header của bạn
    mUsernameLabel.setOrigin(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
    mUsernameLabel.setPosition(cx, mPanel.getPosition().y - panelH / 2.f + 34.f);

    // Username
    float y = mUsernameLabel.getPosition().y + 60.f;
    mUsernameBox.setSize({boxW, boxH});
    mUsernameBox.setPosition({cx, y + boxH / 10.f});
    y += boxH + gap;

    // Password
    mPasswordBox.setSize({boxW, boxH});
    mPasswordBox.setPosition({cx, y + boxH / 8.f});
    y += boxH;

    // Checkbox dưới ô PW, lệch trái 10px từ mép ô nhập
    float inputLeft = cx - boxW / 2.f;
    float checkCenterX = inputLeft + 14.f;
    float checkCenterY = mPasswordBox.getPosition().y + boxH / 2.f + 22.f; // ↑ chỉnh ở đây
    mShowPw.setPosition({checkCenterX, checkCenterY});

    // Nút Đăng nhập
    mLoginBtn.setSize({180.f, 46.f});
    mLoginBtn.setPosition({cx, checkCenterY + 50.f}); // trước là 36/40 → cho gần hơn

    // Link "Quên mật khẩu?"
    sf::FloatRect fb = mForgotText.getLocalBounds();
    mForgotText.setOrigin(fb.left + fb.width / 2.f, 0.f);
    mForgotText.setPosition(cx, mLoginBtn.getPosition().y + 32.f); // trước 40–54 → 18 cho sát hơn
}
