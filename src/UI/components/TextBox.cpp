#include "UI/components/TextBox.h"
#include <cmath>

static void centerLeft(sf::Text &t, float xLeft, float yCenter)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left, b.top + b.height / 2.f);
    t.setPosition(std::round(xLeft), std::round(yCenter) + 1.f); // +1px cho đẹp
}

TextBox::TextBox()
{
    setSize({320.f, 40.f});
    mBox.setFillColor(sf::Color(245, 247, 250));
    mBox.setOutlineThickness(2.f);
    mBox.setOutlineColor(sf::Color(200, 210, 230));

    mText.setFillColor(sf::Color(35, 45, 60));
    mText.setCharacterSize(18);

    mPlaceholder.setFillColor(sf::Color(150, 160, 175));
    mPlaceholder.setCharacterSize(18);

    mCaret.setSize({1.5f, 22.f});
    mCaret.setFillColor(sf::Color(35, 45, 60));
}

void TextBox::setFont(const sf::Font &font)
{
    mText.setFont(font);
    mPlaceholder.setFont(font);
}

void TextBox::setCharacterSize(unsigned int s)
{
    mText.setCharacterSize(s);
    mPlaceholder.setCharacterSize(s);
}

void TextBox::setSize(sf::Vector2f sz)
{
    mBox.setSize(sz);
    mBox.setOrigin(sz.x / 2.f, sz.y / 2.f);
}

void TextBox::setPosition(sf::Vector2f center)
{
    mCenter = center;
    mBox.setPosition(center);

    float left = center.x - mBox.getSize().x / 2.f + 12.f; // padding trái
    float midY = center.y;

    centerLeft(mText, left, midY);
    centerLeft(mPlaceholder, left, midY);

    mCaret.setOrigin(0.f, mCaret.getSize().y / 2.f);
    mCaret.setPosition(left, midY);
}

void TextBox::setPlaceholder(const sf::String &s)
{
    mPlaceholder.setString(s);
    setPosition(mCenter);
}

void TextBox::setText(const sf::String &s)
{
    mBuffer = s;
    if (mPassword)
    {
        mText.setString(sf::String(std::wstring(mBuffer.getSize(), L'*')));
    }
    else
    {
        mText.setString(mBuffer);
    }
    // caret đặt sau text
    float left = mCenter.x - mBox.getSize().x / 2.f + 12.f;
    float midY = mCenter.y;
    sf::FloatRect tb = mText.getLocalBounds();
    mCaret.setPosition(left + tb.width + 4.f, midY);
}

bool TextBox::hitTest_(sf::Vector2f p) const
{
    return mBox.getGlobalBounds().contains(p);
}

void TextBox::handleEvent(const sf::Event &e, const sf::RenderWindow &win)
{
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        auto p = win.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y});
        mFocused = hitTest_(p);
        mBox.setOutlineColor(mFocused ? sf::Color(66, 120, 190) : sf::Color(200, 210, 230));
        return;
    }
    if (!mFocused)
        return;

    if (e.type == sf::Event::TextEntered)
    {
        uint32_t ch = e.text.unicode;
        if (ch == 8)
        { // Backspace
            if (mBuffer.getSize() > 0)
                mBuffer.erase(mBuffer.getSize() - 1, 1);
        }
        else if (ch >= 32 && ch != 127)
        {
            mBuffer += ch; // Unicode “thật”
        }
        setText(mBuffer);
    }
}

void TextBox::update(sf::Time dt)
{
    mCaretTimer += dt.asSeconds();
    if (mCaretTimer >= 0.5f)
    {
        mCaretTimer = 0.f;
        mCaretVisible = !mCaretVisible;
    }

    float left = mCenter.x - mBox.getSize().x / 2.f + 12.f;
    float midY = mCenter.y;
    sf::FloatRect tb = mText.getLocalBounds();
    mCaret.setPosition(left + tb.width + (tb.width > 0 ? 4.f : 0.f), midY);
}

void TextBox::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(mBox, states);
    if (mBuffer.isEmpty())
        target.draw(mPlaceholder, states);
    else
        target.draw(mText, states);
    if (mFocused && mCaretVisible)
        target.draw(mCaret, states);
}
