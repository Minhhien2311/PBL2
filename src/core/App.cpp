// ============================================================
// core/App.cpp
// Triển khai App: vòng lặp, điều hướng, quản lý font.
// ============================================================

#include "core/App.h"

#include <stdexcept>
#include <iostream>

#include "UI/states/LoginState.h"
#include "UI/states/AdminShellState.h"

// -------------------- Khởi tạo --------------------

App::App()
    : mWindowedSize({1280u, 720u}), mAccountMgr("data/admins.txt", "data/agents.txt") // Initialize AccountManager
{
    createWindow_(mWindowedSize, /*fullscreen=*/false);
    prepareFonts_();
    goTo(AppRoute::Login);
}

// -------------------- Vòng lặp chính --------------------
void App::run()
{
    sf::Clock clock;

    while (mWindow.isOpen())
    {
        sf::Event e{};
        while (mWindow.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                mWindow.close();
                break;
            }

            if (e.type == sf::Event::Resized)
            {
                // Cập nhật view theo kích thước mới
                sf::FloatRect area(0.f, 0.f,
                                   static_cast<float>(e.size.width),
                                   static_cast<float>(e.size.height));
                mWindow.setView(sf::View(area));
                relayout_();
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F11)
            {
                toggleFullscreen_();
                relayout_();
                continue;
            }

            // Forward input cho State
            if (mState)
                mState->handleInput(e);
        }

        const sf::Time dt = clock.restart();
        if (mState)
            mState->update(dt);

        render_();
    }
}

// -------------------- Điều hướng --------------------
void App::goTo(AppRoute route)
{
    switch (route)
    {
    case AppRoute::Login:
        mState = std::make_unique<LoginState>(*this);
        break;
    case AppRoute::AdminHome:
        mState = std::make_unique<AdminShellState>(*this);
        break;
    default:
        throw std::runtime_error("Unknown AppRoute");
    }

    if (mState)
    {
        mState->onAttach(); // state có thể truy cập window/fonts
        relayout_();        // layout theo size hiện tại
    }
}

// -------------------- Font --------------------
const sf::Font &App::getFont(const std::string &key) const
{
    auto it = mFonts.find(key);
    if (it == mFonts.end())
    {
        throw std::runtime_error("Font not found: " + key);
    }
    return it->second;
}

void App::loadFont_(const std::string &key, const std::string &path)
{
    sf::Font f;
    if (!f.loadFromFile(path))
    {
        throw std::runtime_error("Failed to load font: " + path);
    }
    mFonts.emplace(key, std::move(f));
}

void App::prepareFonts_()
{
    // Đường dẫn tính từ thư mục build/ (assets được sync vào build/assets/)
    loadFont_("Mulish-Regular", "assets/fonts/Mulish-Regular.ttf");
    loadFont_("Mulish-Bold", "assets/fonts/Mulish-Bold.ttf");
}

// -------------------- Window helpers --------------------
void App::createWindow_(sf::Vector2u size, bool fullscreen)
{
    mIsFullscreen = fullscreen;

    if (fullscreen)
    {
        const auto dm = sf::VideoMode::getDesktopMode();
        mWindow.create(dm, "PBL2 - Airline Ticket Manager", sf::Style::Fullscreen);
    }
    else
    {
        mWindow.create(
            sf::VideoMode(size.x, size.y),
            "PBL2 - Airline Ticket Manager",
            sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
    }

    mWindow.setVerticalSyncEnabled(true);

    const auto ws = mWindow.getSize();
    mWindow.setView(sf::View(sf::FloatRect(0.f, 0.f,
                                           static_cast<float>(ws.x),
                                           static_cast<float>(ws.y))));
}

void App::toggleFullscreen_()
{
    if (mIsFullscreen)
    {
        // Fullscreen -> Window
        createWindow_(mWindowedSize, /*fullscreen=*/false);
    }
    else
    {
        // Window -> Fullscreen (ghi nhớ size hiện tại để quay lại)
        mWindowedSize = mWindow.getSize();
        createWindow_(mWindowedSize, /*fullscreen=*/true);
    }
}

// -------------------- Layout & Render --------------------
void App::relayout_()
{
    if (mState)
        mState->relayout(mWindow.getSize());
}

void App::render_()
{
    mWindow.clear(sf::Color::White);
    if (mState)
    {
        // State kế thừa sf::Drawable → vẽ theo chuẩn SFML
        mWindow.draw(*mState);
    }
    mWindow.display();
}
