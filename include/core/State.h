// core/State.h
#pragma once
#include <SFML/Graphics.hpp>

class App; // forward

// Base cho mọi màn hình (state)
class State
{
public:
    explicit State(App &app) : mApp(app) {}
    virtual ~State() = default;

    // Nhận input từ App
    virtual void handleInput(const sf::Event &e) = 0;
    // Cập nhật logic theo thời gian (nếu cần)
    virtual void update(sf::Time dt) = 0;
    // Vẽ ra cửa sổ
    virtual void draw(sf::RenderTarget &target) const = 0;
    // Căn lại layout khi window đổi size
    virtual void relayout(sf::Vector2u winSize) = 0;

protected:
    App &mApp;
};
