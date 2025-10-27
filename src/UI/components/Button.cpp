#include "UI/components/Button.h"

Button::Button()
{
    mShape.setFillColor(mBgColor);
    mShape.setOutlineThickness(0.f);
    setSize({160.f, 44.f});
}

void Button::setSize(sf::Vector2f size)
{
    mShape.setSize(size);
    mShape.setOrigin(size.x / 2.f, size.y / 2.f); // định vị theo TÂM
}

void Button::setPosition(sf::Vector2f center)
{
    mCenter = center;
    mShape.setPosition(center);
    // căn giữa text theo shape
    sf::FloatRect tb = mText.getLocalBounds();
    mText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    mText.setPosition(center);
}

void Button::setText(const sf::String &s, const sf::Font &font, unsigned int charSize)
{
    mText.setFont(font);
    mText.setString(s);
    mText.setCharacterSize(charSize);
    sf::FloatRect tb = mText.getLocalBounds();
    mText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    mText.setPosition(mCenter);
}

bool Button::hitTest_(sf::Vector2f p) const
{
    return mShape.getGlobalBounds().contains(p);
}

void Button::handleEvent(const sf::Event &e, const sf::RenderWindow &win)
{
    if (!mEnabled)
        return;
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        auto p = win.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y});
        if (hitTest_(p))
            mPressed = true;
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left)
    {
        auto p = win.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y});
        bool clicked = mPressed && hitTest_(p);
        mPressed = false;
        if (clicked && mOnClick)
            mOnClick();
    }
}

void Button::update(const sf::RenderWindow &win)
{
    if (!mEnabled)
    {
        mShape.setFillColor(sf::Color(140, 165, 195));
        return;
    }
    auto p = win.mapPixelToCoords(sf::Mouse::getPosition(win));
    bool hover = hitTest_(p);
    mShape.setFillColor(hover ? mBgHover : mBgColor);
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(mShape, states);
    target.draw(mText, states);
}
