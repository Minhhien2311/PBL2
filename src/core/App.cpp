// core/App.cpp
#include "core/App.h"
#include "UI/states/LoginState.h"
#include "UI/states/AdminDashboardState.h"
#include <iostream>

App::App() : mAccountMgr("data/admins.txt", "data/agents.txt")
{
    auto dm = sf::VideoMode::getDesktopMode();
    mWindow.create(dm, "PBL2", sf::Style::Fullscreen); // <-- full màn
    mWindow.setFramerateLimit(60);
    loadFonts_();
    goTo(AppRoute::Login);
}

void App::run()
{
    sf::Clock clock;
    while (mWindow.isOpen())
    {
        sf::Event e{};
        while (mWindow.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                mWindow.close();
            if (e.type == sf::Event::Resized)
                relayout_();
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F11)
                toggleFullscreen();
            if (mState)
                mState->handleInput(e);
        }

        const sf::Time dt = clock.restart();
        if (mState)
            mState->update(dt);

        mWindow.clear(sf::Color(245, 248, 255)); // nền nhạt
        if (mState)
            mState->draw(mWindow);
        mWindow.display();
    }
}

void App::goTo(AppRoute route)
{
    mCurrentRoute = route;

    // Tập trung switch-case điều hướng ở đúng 1 chỗ
    switch (route)
    {
    case AppRoute::Login:
        mState = std::make_unique<LoginState>(*this);
        break;
    case AppRoute::AdminHome:
        mState = std::make_unique<AdminDashboardState>(*this);
        break;
    }
    relayout_(); // tạo xong state thì bố trí lại layout theo size hiện tại
}

const sf::Font &App::getFont(const std::string &name) const
{
    auto it = mFonts.find(name);
    if (it == mFonts.end())
    {
        static sf::Font fallback;
        std::cerr << "[WARN] Font '" << name << "' not found. Using default.\n";
        return fallback;
    }
    return it->second;
}

void App::loadFonts_()
{
    // Đảm bảo rule Makefile đã copy assets sang build/assets
    // Bạn có thể đổi tên font theo thư mục của bạn
    sf::Font reg, bold;
    if (!reg.loadFromFile("assets/fonts/Mulish-Regular.ttf"))
        std::cerr << "[ERR] Cannot load assets/fonts/Mulish-Regular.ttf\n";
    if (!bold.loadFromFile("assets/fonts/Mulish-Bold.ttf"))
        std::cerr << "[ERR] Cannot load assets/fonts/Mulish-Bold.ttf\n";
    mFonts.emplace("Mulish-Regular", std::move(reg));
    mFonts.emplace("Mulish-Bold", std::move(bold));
}

void App::relayout_()
{
    if (mState)
        mState->relayout(mWindow.getSize());
}

void App::toggleFullscreen()
{
    static bool full = true;
    full = !full;
    auto dm = sf::VideoMode::getDesktopMode();
    if (full)
        mWindow.create(dm, "PBL2", sf::Style::Fullscreen);
    else
        mWindow.create({900, 600}, "PBL2 - Login", sf::Style::Titlebar | sf::Style::Close);
    mWindow.setFramerateLimit(60);
    relayout_(); // gọi lại layout cho state hiện tại
}
