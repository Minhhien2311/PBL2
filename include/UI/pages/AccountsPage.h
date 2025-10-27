#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "UI/components/TextBox.h"
#include "UI/components/Button.h"

// Trang "Quản lý tài khoản" — chỉ giao diện, không xử lý DB ở đây.
// Dùng tuyệt đối: textbox đặt theo TÂM, label theo góc trái trên.
class AccountsPage
{
public:
    struct Profile
    {
        sf::String id;
        sf::String fullName;
        sf::String username;
        sf::String phone;
        sf::String role;
        sf::String email;
    };

    // uiFont: phông chữ giao diện; origin: góc trái vùng content; cw: chiều rộng content
    AccountsPage(const sf::Font &uiFont, sf::Vector2f origin, float cw);

    // Data binding
    void setInitialProfile(const Profile &p);
    Profile getProfile() const;

    // Callback cho 2 nút
    void setOnSave(std::function<void(const Profile &)> cb) { mOnSave = std::move(cb); }
    void setOnChangePassword(std::function<void()> cb) { mOnChangePw = std::move(cb); }

    // Vòng đời
    void handleEvent(const sf::Event &e, const sf::RenderWindow &win);
    void update(const sf::RenderWindow &win); // kiểu em đang dùng
    void update(sf::Time /*dt*/);             // overload tương thích (không dùng)
    void draw(sf::RenderTarget &rt) const;

    // Cho phép đổi lại layout khi đổi kích thước cửa sổ
    void setOrigin(sf::Vector2f origin)
    {
        mOrigin = origin;
        layout();
    }
    void setContentWidth(float w)
    {
        mContentW = w;
        layout();
    }

private:
    void layout(); // tính lại vị trí tuyệt đối

    // Fonts & màu
    const sf::Font &mFont;

    // Gốc vùng content & kích thước
    sf::Vector2f mOrigin;
    float mContentW;

    // Title
    sf::Text mTitle;

    // Labels
    sf::Text mLblId, mLblFullname, mLblUsername, mLblPhone, mLblRole, mLblEmail;

    // TextBoxes (đặt theo tâm)
    TextBox mTxtId, mTxtFullname, mTxtUsername, mTxtPhone, mTxtRole, mTxtEmail;

    // Buttons (đặt theo tâm)
    Button mBtnSave, mBtnChangePw;

    // Khung bao quanh form
    sf::RectangleShape mFormOutline;

    // Callbacks
    std::function<void(const Profile &)> mOnSave;
    std::function<void()> mOnChangePw;
};
