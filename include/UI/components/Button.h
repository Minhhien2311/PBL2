#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <string>
#include <functional> // Cho std::function

namespace UI {

class Button : public sf::Drawable, public sf::Transformable {
public:
    Button();

    void setSize(const sf::Vector2f& size);
    void setPosition(const sf::Vector2f& position); // Ghi đè để cập nhật text
    void setText(const std::string& text, const sf::Font& font, unsigned int characterSize);
    void setBackgroundColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void setHoverColor(const sf::Color& color); // Màu khi di chuột qua

    void setOnAction(std::function<void()> action); // Hàm callback khi nhấn

    void handleEvent(sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window); // Cần window để lấy vị trí chuột

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void centerText(); // Hàm helper để căn giữa text

    sf::RectangleShape mShape;
    sf::Text mText;

    sf::Color mNormalColor = sf::Color::Blue;
    sf::Color mHoverColor = sf::Color::Cyan;
    sf::Color mTextColor = sf::Color::White;

    bool mIsHovered = false;
    std::function<void()> mAction; // Hành động khi nút được nhấn
};

} // namespace UI

#endif // BUTTON_H
