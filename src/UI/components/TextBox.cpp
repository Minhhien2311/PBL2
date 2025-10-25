#include "UI/components/TextBox.h"

namespace UI {

TextBox::TextBox() {
    // Thiết lập mặc định
    mBox.setFillColor(sf::Color(255, 255, 255, 50));
    mBox.setOutlineThickness(1);
    mBox.setOutlineColor(sf::Color::White);
    mText.setFillColor(sf::Color::White);
    mPlaceholderText.setFillColor(sf::Color(255, 255, 255, 120));
    mCursor.setFillColor(sf::Color::White);
    mCursor.setSize({1.f, 20.f}); // Kích thước con trỏ mặc định
}

void TextBox::setSize(const sf::Vector2f& size) {
    mBox.setSize(size);
    mBox.setOrigin(size.x / 2.f, size.y / 2.f); // Đặt gốc ở giữa để dễ đặt vị trí
    // Cập nhật lại vị trí text/placeholder/cursor dựa trên size mới
    updateTextDisplay();
}

void TextBox::setFont(const sf::Font& font) {
    mText.setFont(font);
    mPlaceholderText.setFont(font);
    // Cập nhật lại kích thước con trỏ theo font size
    mCursor.setSize({1.f, static_cast<float>(mText.getCharacterSize()) * 1.2f});
    updateTextDisplay();
}

void TextBox::setCharacterSize(unsigned int size) {
    mText.setCharacterSize(size);
    mPlaceholderText.setCharacterSize(size);
    // Cập nhật lại kích thước con trỏ theo font size
    mCursor.setSize({1.f, static_cast<float>(size) * 1.2f});
    updateTextDisplay();
}

void TextBox::setPlaceholder(const std::string& text) {
    mPlaceholderString = text;
    mPlaceholderText.setString(mPlaceholderString);
    updateTextDisplay();
}

void TextBox::setPasswordMode(bool enabled) {
    mIsPassword = enabled;
    updateTextDisplay();
}

void TextBox::setOutlineColor(const sf::Color& color) {
    mBox.setOutlineColor(color);
}
void TextBox::setFillColor(const sf::Color& color){
     mBox.setFillColor(color);
}


std::string TextBox::getText() const {
    return mInputString;
}
void TextBox::setText(const std::string& text){
    mInputString = text;
    updateTextDisplay();
}

bool TextBox::isSelected() const {
    return mIsSelected;
}

void TextBox::handleEvent(sf::Event& event, const sf::RenderWindow& window) {
    // Xử lý click chuột để chọn/bỏ chọn
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            // Tạo bounding box bao gồm cả gốc Transformable
            sf::FloatRect bounds = mBox.getGlobalBounds();
            sf::Transform transform = getTransform(); // Lấy transform của TextBox
            bounds = transform.transformRect(bounds); // Áp dụng transform vào bounds

            if (bounds.contains(mousePos)) {
                mIsSelected = true;
                mCursorClock.restart(); // Reset con trỏ
                mShowCursor = true;
                mBox.setOutlineColor(sf::Color::Cyan); // Đổi màu viền khi chọn
            } else {
                mIsSelected = false;
                 mBox.setOutlineColor(sf::Color::White); // Trả lại màu gốc
            }
        }
    }

    // Xử lý nhập text nếu đang được chọn
    if (mIsSelected && event.type == sf::Event::TextEntered) {
        // Xử lý Backspace (ASCII = 8)
        if (event.text.unicode == 8) {
            if (!mInputString.empty()) {
                mInputString.pop_back();
            }
        }
        // Chỉ nhận ký tự ASCII in được và không phải là Enter/Tab
        else if (event.text.unicode >= 32 && event.text.unicode < 128) {
            // Thêm giới hạn độ dài nếu muốn
            mInputString += static_cast<char>(event.text.unicode);
        }
        updateTextDisplay();
        mCursorClock.restart(); // Reset con trỏ khi gõ
        mShowCursor = true;
    }
}

void TextBox::update(sf::Time dt) {
    // Hiệu ứng nhấp nháy con trỏ
    if (mIsSelected) {
        if (mCursorClock.getElapsedTime().asSeconds() > 0.5f) {
            mShowCursor = !mShowCursor;
            mCursorClock.restart();
        }
    } else {
        mShowCursor = false;
    }

    // Cập nhật vị trí con trỏ
    // Vị trí con trỏ sẽ là cuối của text hiển thị
    sf::Vector2f textPos = mText.findCharacterPos(mDisplayString.length());
    mCursor.setPosition(textPos.x + 1.f, mText.getPosition().y); // +1 để nó nằm sau ký tự cuối
     mCursor.setOrigin(0.f, 0.f); // Đặt gốc ở top-left
    mCursor.setSize({1.f, static_cast<float>(mText.getCharacterSize()) * 1.2f}); // Đảm bảo size đúng
}


void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform(); // Áp dụng transform của TextBox

    target.draw(mBox, states);

    if (mInputString.empty() && !mPlaceholderString.empty()) {
        target.draw(mPlaceholderText, states);
    } else {
        target.draw(mText, states);
    }

    if (mShowCursor) {
        target.draw(mCursor, states);
    }
}

void TextBox::updateTextDisplay() {
    if (mIsPassword) {
        mDisplayString.assign(mInputString.length(), '*');
    } else {
        mDisplayString = mInputString;
    }
    mText.setString(mDisplayString);

    // Căn chỉnh vị trí Text và Placeholder bên trong Box
    float padding = 5.f; // Khoảng cách từ lề
    sf::Vector2f boxSize = mBox.getSize();
    sf::Vector2f boxOrigin = mBox.getOrigin(); // Gốc đang ở giữa

    // Vị trí top-left của box (tính từ gốc ở giữa)
    sf::Vector2f topLeftPos = -boxOrigin;

    mText.setPosition(topLeftPos.x + padding,
                      topLeftPos.y + boxSize.y / 2.f - mText.getCharacterSize() / 1.f); // Điều chỉnh để căn giữa theo chiều dọc
    mPlaceholderText.setPosition(mText.getPosition());
    mPlaceholderText.setCharacterSize(mText.getCharacterSize()); // Đảm bảo cùng size
    mPlaceholderText.setFont(*mText.getFont()); // Đảm bảo cùng font

    // Giới hạn text hiển thị nếu dài hơn box (cần phức tạp hơn để scroll)
    // Tạm thời chỉ cắt bớt nếu quá dài
    while (mText.getGlobalBounds().width > boxSize.x - 2 * padding && !mDisplayString.empty()) {
         if (mIsPassword) {
            // Không cắt password hiển thị
            break;
         } else {
             // Cắt từ đầu cho giống ô input thông thường
             mDisplayString.erase(0, 1);
             mText.setString(mDisplayString);
         }
    }
}

} // namespace UI
