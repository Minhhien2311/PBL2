#pragma once
// ============================================================
// UI/states/AdminShellState.h
//
// Shell sau đăng nhập:
// - Sidebar trái: có SECTION (không click) + ITEM (click để đổi page) + nút "Đăng xuất" ghim đáy.
// - Content phải: hiện tại chỉ hiển thị AccountsPage.
// - Lề content: 24px (trên/dưới/trái/phải) giống mockup.
//
// Không đổi mạch chương trình, chỉ nâng cấp layout.
// ============================================================

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#include "UI/states/State.h"
#include "UI/pages/AccountsPage.h"
#include "UI/pages/FlightsPage.h"
#include "UI/pages/RoutesPage.h"

class AdminShellState : public UI::State
{
public:
    explicit AdminShellState(::App &app);

    void onAttach() override;
    void handleInput(sf::Event &e) override;
    void update(sf::Time dt) override;
    void relayout(sf::Vector2u windowSize) override;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
    // ===== Fonts =====
    const sf::Font *mFontRegular = nullptr;
    const sf::Font *mFontBold = nullptr;

    // ===== Sidebar layout =====
    float mSidebarWidth = 240.f;
    float mNavItemHeight = 48.f;
    float mSidebarPadX = 16.f;   // padding trái cho text
    float mSidebarPadTop = 24.f; // bắt đầu từ top
    float mSectionGapY = 10.f;   // khoảng cách giữa section title và item đầu
    float mGroupGapY = 18.f;     // khoảng cách giữa hai group

    struct NavRow
    {
        std::wstring label;
        sf::FloatRect bounds;   // tọa độ tuyệt đối
        bool isSection = false; // section title => không click
        enum class Item
        {
            Dashboard,
            Flights,
            Routes,
            Accounts,
            Logout,
            None
        } item = Item::None;
    };

    std::vector<NavRow> mNav; // gồm cả section & item
    int mHoverIndex = -1;     // index item hover
    NavRow::Item mActiveItem = NavRow::Item::Accounts;

    // Sidebar visuals
    sf::RectangleShape mSidebarBg;

    // ===== Logout button (ghim đáy) =====
    sf::FloatRect mLogoutBounds{}; // click area
    sf::RectangleShape mLogoutBg;  // vẽ như 1 nút đơn giản
    sf::Text mLogoutText;

    // ===== Content layout =====
    sf::Vector2f mContentOrigin{0.f, 0.f}; // TL của vùng content (đÃ có lề 24px)
    float mContentWidth = 0.f;             // bề rộng content (đÃ trừ 2*24px)

    // ===== Current Page =====
    std::unique_ptr<AccountsPage> mAccountsPage;

    // ===== Các page khác trong Menu =====
    std::unique_ptr<RoutesPage> mFlightsPage;
    std::unique_ptr<FlightsPage> mFlightsPage;

private:
    // Build lại sidebar & bounds
    void buildSidebar_();
    // Tạo lại page Accounts (theo origin/width hiện thời)
    void createAccountsPage_();
    void createRoutespage_();
    void createFlightspage_();
    void createDashboardpage_();
    void createReportsPage_();
    void createPromotionsPage_();
    void createRevenuePage_();
    // Chuyển trang theo item
    void switchTo_(NavRow::Item item);
    // Hit test
    static bool hit_(const sf::Vector2f &p, const sf::FloatRect &r);
};
