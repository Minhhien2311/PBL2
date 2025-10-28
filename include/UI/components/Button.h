#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

// Nút bấm đơn giản, định vị theo TÂM (center)
class Button : public sf::Drawable
{
public:
    Button();

    void setEnabled(bool e)
    {
        mEnabled = e;
        if (!e)
            mPressed = false;
    }
    bool enabled() const { return mEnabled; }

    void setSize(sf::Vector2f size);
    void setPosition(sf::Vector2f center); // định vị theo TÂM
    sf::Vector2f getPosition() const { return mCenter; }

    // Thêm overload để khi truyển std::string utf-8 không lỗi font
    void setText(const sf::String &s, const sf::Font &font, unsigned int charSize);

    void setBackgroundColor(sf::Color c)
    {
        mBgColor = c;
        mShape.setFillColor(c);
    }
    void setTextColor(sf::Color c) { mText.setFillColor(c); }

    // Đăng ký callback khi bấm nút
    void setOnAction(std::function<void()> cb) { mOnClick = std::move(cb); }

    void handleEvent(const sf::Event &e, const sf::RenderWindow &win);
    void update(const sf::RenderWindow &win); // hover effect

private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    bool hitTest_(sf::Vector2f p) const;

private:
    bool mEnabled{true};
    sf::RectangleShape mShape;
    sf::Text mText;
    sf::Vector2f mCenter{0.f, 0.f};
    sf::Color mBgColor{sf::Color(66, 120, 190)};
    sf::Color mBgHover{sf::Color(52, 96, 160)};
    bool mPressed{false};
    std::function<void()> mOnClick;
};
