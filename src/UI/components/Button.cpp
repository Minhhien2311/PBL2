#include "UI/components/Button.h"

namespace UI {

Button::Button() {
    // Thiết lập mặc định
    mShape.setFillColor(mNormalColor);
    mText.setFillColor(mTextColor);
}

void Button::setSize(const sf::Vector2f& size) {
    mShape.setSize(size);
    mShape.setOrigin(size.x / 2.f, size.y / 2.f); // Đặt gốc ở giữa
    centerText(); // Căn lại text sau khi đổi size
}

void Button::setPosition(const sf::Vector2f& position) {
    sf::Transformable::setPosition(position); // Gọi hàm gốc
    centerText(); // Căn lại text sau khi đổi vị trí
}


void Button::setText(const std::string& text, const sf::Font& font, unsigned int characterSize) {
    mText.setString(text);
    mText.setFont(font);
    mText.setCharacterSize(characterSize);
    centerText(); // Căn lại text
}

void Button::setBackgroundColor(const sf::Color& color) {
    mNormalColor = color;
    if (!mIsHovered) {
        mShape.setFillColor(mNormalColor);
    }
}

void Button::setTextColor(const sf::Color& color) {
    mTextColor = color;
    mText.setFillColor(mTextColor);
}

void Button::setHoverColor(const sf::Color& color) {
    mHoverColor = color;
    if (mIsHovered) {
        mShape.setFillColor(mHoverColor);
    }
}

void Button::setOnAction(std::function<void()> action) {
    mAction = action;
}

void Button::handleEvent(sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

            // Tạo bounding box bao gồm cả gốc Transformable
            sf::FloatRect bounds = mShape.getGlobalBounds();
            sf::Transform transform = getTransform(); // Lấy transform của Button
            bounds = transform.transformRect(bounds); // Áp dụng transform vào bounds

            if (bounds.contains(mousePos)) {
                if (mAction) {
                    mAction(); // Gọi hàm callback nếu có
                }
            }
        }
    }
}

void Button::update(const sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Tạo bounding box bao gồm cả gốc Transformable
    sf::FloatRect bounds = mShape.getGlobalBounds();
    sf::Transform transform = getTransform(); // Lấy transform của Button
    bounds = transform.transformRect(bounds); // Áp dụng transform vào bounds


    bool currentlyHovered = bounds.contains(mousePos);

    if (currentlyHovered && !mIsHovered) {
        mIsHovered = true;
        mShape.setFillColor(mHoverColor); // Đổi màu khi hover
    } else if (!currentlyHovered && mIsHovered) {
        mIsHovered = false;
        mShape.setFillColor(mNormalColor); // Trả lại màu gốc
    }
}


void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform(); // Áp dụng transform của Button
    target.draw(mShape, states);
    target.draw(mText, states);
}

void Button::centerText() {
    sf::FloatRect textBounds = mText.getLocalBounds();
    mText.setOrigin(textBounds.left + textBounds.width / 2.0f,
                    textBounds.top + textBounds.height / 2.0f);
    // Vị trí text sẽ tương đối so với gốc của Button (đã được setOrigin ở giữa)
    mText.setPosition(0, 0); // Đặt text vào giữa Shape (vì cả 2 đều có gốc ở giữa)
}


} // namespace UI
