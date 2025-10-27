// // UI/states/AdminDashboardState.cpp
// #include "UI/states/AdminDashboardState.h"
// #include "core/App.h"

// AdminDashboardState::AdminDashboardState(App &app) : State(app)
// {
//     mTitle.setFont(mApp.getFont("Mulish-Bold"));
//     mTitle.setString(L"Màn hình chính (Admin)");
//     mTitle.setCharacterSize(28);
//     mTitle.setFillColor(sf::Color(29, 78, 137));
//     relayout(mApp.window().getSize());
// }

// void AdminDashboardState::handleInput(const sf::Event &e)
// {
//     (void)e; // chưa dùng
// }

// void AdminDashboardState::update(sf::Time) {}

// void AdminDashboardState::draw(sf::RenderTarget &target) const
// {
//     target.draw(mTitle);
// }

// void AdminDashboardState::relayout(sf::Vector2u win)
// {
//     sf::FloatRect b = mTitle.getLocalBounds();
//     mTitle.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
//     mTitle.setPosition(win.x / 2.f, 80.f);
// }
