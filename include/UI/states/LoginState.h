// UI/states/LoginState.h
#pragma once
#include "core/State.h"
#include <SFML/Graphics.hpp>

// NOTE: thay các include dưới bằng đường dẫn component thật của bạn:
#include "UI/components/Button.h"  // giả định bạn có Button với API setText(...), setPosition, handleEvent, update, draw
#include "UI/components/TextBox.h" // giả định bạn có TextBox với API setPlaceholder, setPasswordMode, getText, ...
#include "UI/components/Checkbox.h"

class LoginState : public State
{
public:
    explicit LoginState(App &app);

    void handleInput(const sf::Event &e) override;
    void update(sf::Time dt) override;
    void draw(sf::RenderTarget &target) const override;
    void relayout(sf::Vector2u winSize) override;

private:
    void onLogin_(); // bấm đăng nhập -> điều hướng

private:
    // UI cơ bản
    sf::Text mTitle;
    sf::RectangleShape mPanel; // khung form

    // Labels (nếu bạn muốn)
    sf::Text mUsernameLabel;
    sf::Text mPasswordLabel;
    sf::Text mForgotText; // "Quên mật khẩu?"

    // Components
    Checkbox mShowPw; // <-- checkbox hiện/ẩn mật khẩu
    TextBox mUsernameBox;
    TextBox mPasswordBox;
    Button mLoginBtn;

    // tạm: checkbox show password (nếu component bạn đã có)
    // Checkbox mShowPw;  // nếu có, bạn thêm vào

    // Trạng thái hover/focus tuỳ component của bạn lo
};
