#pragma once
#include <SFML/Graphics.hpp>

class TextBox : public sf::Drawable
{
public:
    TextBox();

    void setSize(sf::Vector2f sz);
    void setPosition(sf::Vector2f center); // ĐỊNH VỊ THEO TÂM
    sf::Vector2f getPosition() const { return mCenter; }

    void setFont(const sf::Font &font);
    void setCharacterSize(unsigned int s);

    void setPlaceholder(const sf::String &s); // ← dùng sf::String
    void setPasswordMode(bool on) { mPassword = on; }

    void setText(const sf::String &s); // ← dùng sf::String
    sf::String getText() const { return mBuffer; }

    void handleEvent(const sf::Event &e, const sf::RenderWindow &win);
    void update(sf::Time dt);

private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    bool hitTest_(sf::Vector2f p) const;

private:
    sf::RectangleShape mBox;
    sf::Text mText;
    sf::Text mPlaceholder;
    sf::RectangleShape mCaret;

    sf::Vector2f mCenter{0.f, 0.f};
    sf::String mBuffer; // ← LƯU NỘI BỘ DẠNG sf::String (UTF-32)
    bool mFocused{false};
    bool mPassword{false};
    float mCaretTimer{0.f};
    bool mCaretVisible{true};
};
