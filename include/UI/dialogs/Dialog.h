#pragma once
#include <SFML/Graphics.hpp>

// Base class cho mọi Dialog (mặc định là modal)
class Dialog
{
public:
    virtual ~Dialog() = default;

    // Trạng thái mở/đóng
    void open() { mOpen = true; }
    void close() { mOpen = false; }
    bool isOpen() const { return mOpen; }

    // Mặc định dialog là modal (state có thể dùng để chặn event xuống page)
    virtual bool isModal() const { return true; }

    // Luôn căn vị trí theo kích thước cửa sổ tại đây
    virtual void relayout(sf::Vector2u windowSize) = 0;

    // Chỉ nên nhận event khi isOpen()==true
    virtual void handleEvent(const sf::Event &e, const sf::RenderWindow &win) = 0;

    virtual void update(sf::Time dt) = 0;
    virtual void draw(sf::RenderTarget &target) const = 0;

protected:
    bool mOpen{false};
};
