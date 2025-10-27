#pragma once
#include <SFML/Graphics.hpp>

// Base class trừu tượng cho mọi "Page" (màn nội dung)
class Page
{
public:
    virtual ~Page() = default;

    // Gọi khi page được đưa lên foreground (tạo/tải dữ liệu nếu cần)
    virtual void onAttach() {}
    // Gọi ngay trước khi rời khỏi foreground (lưu/giải phóng nếu cần)
    virtual void onDetach() {}

    // Sắp xếp lại vị trí tuyệt đối dựa trên kích thước cửa sổ
    virtual void relayout(sf::Vector2u windowSize) = 0;

    // Truyền toàn bộ event vào page (nếu state không chặn)
    virtual void handleEvent(const sf::Event &e, const sf::RenderWindow &win) = 0;

    // Cập nhật logic theo thời gian
    virtual void update(sf::Time dt) = 0;

    // Vẽ nội dung của page
    virtual void draw(sf::RenderTarget &target) const = 0;
};
