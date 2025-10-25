#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SFML/Graphics.hpp>
#include <string>
#include <functional> // Cho std::function

namespace UI {

class TextBox : public sf::Drawable, public sf::Transformable {
public:
    TextBox();

    void setSize(const sf::Vector2f& size);
    void setFont(const sf::Font& font);
    void setCharacterSize(unsigned int size);
    void setPlaceholder(const std::string& text);
    void setPasswordMode(bool enabled);
    void setOutlineColor(const sf::Color& color);
    void setFillColor(const sf::Color& color);

    std::string getText() const;
    void setText(const std::string& text); // Thêm hàm này để có thể đặt text từ bên ngoài
    bool isSelected() const;

    void handleEvent(sf::Event& event, const sf::RenderWindow& window);
    void update(sf::Time dt); // Cho hiệu ứng con trỏ

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void updateTextDisplay();

    sf::RectangleShape mBox;
    sf::Text mText;
    sf::Text mPlaceholderText;
    sf::RectangleShape mCursor; // Con trỏ nhấp nháy

    std::string mInputString;
    std::string mDisplayString; // Chuỗi hiển thị (có thể là '*' nếu là password)
    std::string mPlaceholderString;

    bool mIsSelected = false;
    bool mIsPassword = false;

    // Cho hiệu ứng con trỏ
    sf::Clock mCursorClock;
    bool mShowCursor = false;
};

} // namespace UI

#endif // TEXTBOX_H
