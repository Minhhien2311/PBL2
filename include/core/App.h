// core/App.h
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include "core/State.h"
#include "core/AccountManager.h"

// Tuyến điều hướng đơn giản bằng enum + switch-case
enum class AppRoute
{
    Login,
    AdminHome
};

class App
{
public:
    App();
    void run();

    AccountManager &accountMgr() { return mAccountMgr; }
    const AccountManager &accountMgr() const { return mAccountMgr; }

    // f11 hỗ trợ bật tắt full màn hình
    void toggleFullscreen();

    // Điều hướng tập trung (chỉ 1 nơi dùng switch-case)
    void goTo(AppRoute route);

    // Cung cấp tài nguyên dùng chung
    sf::RenderWindow &window() { return mWindow; }
    const sf::Font &getFont(const std::string &name) const; // "Mulish-Regular", "Mulish-Bold"

private:
    AccountManager mAccountMgr;
    void loadFonts_(); // nạp font 1 lần
    void relayout_();  // khi resize, gọi xuống state

private:
    sf::RenderWindow mWindow;
    std::unique_ptr<State> mState; // state hiện tại

    // cache font theo tên
    std::unordered_map<std::string, sf::Font> mFonts;

    // tiện: lưu route hiện tại (để biết đang ở đâu)
    AppRoute mCurrentRoute{AppRoute::Login};
};
