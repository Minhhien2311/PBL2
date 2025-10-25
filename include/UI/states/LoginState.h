#ifndef LOGIN_STATE_H
#define LOGIN_STATE_H

#include "UI/states/State.h"
#include "UI/components/TextBox.h" // Cần tạo file này
#include "UI/components/Button.h"  // Cần tạo file này
#include <SFML/Graphics.hpp>
#include <string>

namespace UI {

class LoginState : public State {
public:
    LoginState(App& app); // Constructor nhận tham chiếu App

    void handleInput(sf::Event& event) override;
    void update(sf::Time dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void initUI(); // Hàm helper để khởi tạo các thành phần UI
    void handleLogin(); // Hàm xử lý logic khi nhấn nút Login

    // Tài nguyên đồ họa
    sf::Texture mBackgroundTexture;
    sf::Sprite mBackgroundSprite;
    sf::Texture mIconTexture;
    sf::Sprite mIconSprite;

    // Thành phần UI
    sf::Text mTitleText;
    sf::Text mUsernameLabel;
    sf::Text mPasswordLabel;
    TextBox mUsernameBox;
    TextBox mPasswordBox;
    Button mLoginButton;

    // Font (lấy từ App)
    sf::Font& mFontRegular;
    sf::Font& mFontBold;
};

} // namespace UI

#endif // LOGIN_STATE_H
