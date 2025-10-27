#pragma once
#include <SFML/Graphics.hpp>

// Label đơn giản bọc sf::Text (header-only)
class Label : public sf::Drawable
{
public:
    Label()
    {
        mText.setFillColor(sf::Color(42, 59, 85)); // #2A3B55
        mText.setCharacterSize(18);
    }

    // Setters tiện dụng
    void setFont(const sf::Font &f) { mText.setFont(f); }
    void setString(const sf::String &s) { mText.setString(s); }
    void setCharacterSize(unsigned s) { mText.setCharacterSize(s); }
    void setFillColor(sf::Color c) { mText.setFillColor(c); }
    void setPosition(float x, float y) { mText.setPosition(x, y); }
    void setPosition(sf::Vector2f pos) { mText.setPosition(pos); }

    // Getters hữu ích
    sf::FloatRect getLocalBounds() const { return mText.getLocalBounds(); }
    sf::FloatRect getGlobalBounds() const { return mText.getGlobalBounds(); }
    const sf::Text &raw() const { return mText; }

private:
    sf::Text mText;

    void draw(sf::RenderTarget &rt, sf::RenderStates st) const override
    {
        rt.draw(mText, st);
    }
};
