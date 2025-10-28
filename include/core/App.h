#pragma once
// ============================================================
// core/App.h
//
// Trung tâm vòng lặp SFML, điều hướng State, quản lý font/tài nguyên.
// - State kế thừa sf::Drawable → App vẽ bằng mWindow.draw(*mState).
// - Khi resize/F11 → gọi mState->relayout(windowSize).
// - Điều hướng route: Login → AdminHome (AdminShellState).
// ============================================================

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "UI/states/State.h"     // UI::State base
#include "core/AccountManager.h" // Quản lý đăng nhập (đã có trong dự án)

// Tuyến (route) của ứng dụng
enum class AppRoute
{
    Login = 0,
    AdminHome
};

class App
{
public:
    explicit App();

    // Vòng lặp chính
    void run();

    // Điều hướng giữa các màn hình
    void goTo(AppRoute route);

    // Tài nguyên chung
    sf::RenderWindow &window() { return mWindow; }
    const sf::RenderWindow &window() const { return mWindow; }

    const sf::Font &getFont(const std::string &key) const;

    AccountManager &accountMgr() { return mAccountMgr; }             // Trả về tham chiếu để sửa đổi
    const AccountManager &accountMgr() const { return mAccountMgr; } // Trả về tham chiếu hằng

private:
    // ===== Cửa sổ & trạng thái hiển thị =====
    sf::RenderWindow mWindow;
    bool mIsFullscreen = false;
    sf::Vector2u mWindowedSize{1280u, 720u}; // ghi nhớ size khi thoát fullscreen

    // ===== State hiện tại =====
    std::unique_ptr<UI::State> mState;

    // ===== Tài nguyên dùng chung =====
    std::unordered_map<std::string, sf::Font> mFonts;
    AccountManager mAccountMgr;

private:
    // ===== Helpers =====
    void createWindow_(sf::Vector2u size, bool fullscreen);
    void toggleFullscreen_();

    void loadFont_(const std::string &key, const std::string &path);
    void prepareFonts_();

    void relayout_(); // gọi khi resize/F11
    void render_();   // vẽ khung hình
};
