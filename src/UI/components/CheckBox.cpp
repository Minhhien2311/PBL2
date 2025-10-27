#include "UI/components/Checkbox.h"

Checkbox::Checkbox()
{
    mBox.setSize({16.f, 16.f});
    mBox.setOrigin(9.f, 9.f);
    mBox.setFillColor(sf::Color::Transparent);
    mBox.setOutlineThickness(2.f);
    mBox.setOutlineColor(sf::Color(180, 190, 210));

    mTick.setSize({12.f, 12.f});
    mTick.setOrigin(6.f, 6.f);
    mTick.setFillColor(sf::Color(66, 120, 190));
}

void Checkbox::setLabel(const std::wstring &s, const sf::Font &font, unsigned int charSize)
{
    mText.setFont(font);
    mText.setString(s);
    mText.setCharacterSize(charSize);
    mText.setFillColor(sf::Color(80, 90, 110));
    setPosition(mCenter); // cập nhật layout
}

void Checkbox::setChecked(bool v)
{
    mChecked = v;
}

void Checkbox::setPosition(sf::Vector2f center)
{
    mCenter = center;
    mBox.setPosition(center);
    mTick.setPosition(center);

    sf::FloatRect b = mText.getLocalBounds();
    mText.setOrigin(b.left, b.top + b.height / 2.f);
    mText.setPosition(center.x + 18.f + 8.f, center.y); // 18 = cạnh ô, +8 khoảng cách
}

bool Checkbox::hitTest_(sf::Vector2f p) const
{
    return mBox.getGlobalBounds().contains(p) || mText.getGlobalBounds().contains(p);
}

void Checkbox::handleEvent(const sf::Event &e, const sf::RenderWindow &win)
{
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        auto p = win.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y});
        if (hitTest_(p))
        {
            mChecked = !mChecked;
            if (mChanged)
                mChanged(mChecked);
        }
    }
}

void Checkbox::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(mBox, states);
    if (mChecked)
        target.draw(mTick, states);
    target.draw(mText, states);
}
