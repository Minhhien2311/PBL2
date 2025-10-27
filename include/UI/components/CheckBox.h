#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

class Checkbox : public sf::Drawable
{
public:
    Checkbox();

    void setPosition(sf::Vector2f center); // theo TÂM của cả cụm (ô + text)
    sf::Vector2f getPosition() const { return mCenter; }

    void setLabel(const std::wstring &s, const sf::Font &font, unsigned int charSize);
    void setChecked(bool v);
    bool isChecked() const { return mChecked; }

    void setOnChanged(std::function<void(bool)> cb) { mChanged = std::move(cb); }

    void handleEvent(const sf::Event &e, const sf::RenderWindow &win);

private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    bool hitTest_(sf::Vector2f p) const;

private:
    sf::RectangleShape mBox;  // ô vuông
    sf::RectangleShape mTick; // dấu tick đơn giản
    sf::Text mText;

    sf::Vector2f mCenter{0.f, 0.f};
    bool mChecked{false};
    std::function<void(bool)> mChanged;
};
