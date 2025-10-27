#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "UI/states/State.h"
#include "UI/pages/AccountsPage.h"

class AccountsPage;

// AdminShellState: chứa sidebar + vùng content, định tuyến giữa các page.
// Hiện tại mới gắn sẵn AccountsPage; các mục khác sẽ thêm dần.
class AdminShellState : public UI::State
{
public:
    AdminShellState(App &app) : UI::State(app), mAccountsPage(app.getFont("Mulish-Regular"), {0.f, 0.f}, 0.f) {} // ctor trùng base
    void onAttach() override;                                                                                    // KHÔNG override
    void handleInput(sf::Event &e) override;                                                                     // ĐÚNG tên
    void update(sf::Time dt) override;
    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override;

private:
    // Router nội bộ
    enum class PageId
    {
        Revenue,    // Doanh thu (placeholder, vẽ tiêu đề)
        Accounts,   // Quản lý tài khoản (đã có page)
        Agents,     // Danh sách đại lý (placeholder)
        Routes,     // Quản lý tuyến bay (placeholder)
        Flights,    // Quản lý chuyến bay (placeholder)
        Promotions, // Quản lý khuyến mãi (placeholder)
        TicketsSold // Vé đã bán (placeholder)
    };

    void buildSidebar();      // tạo item sidebar
    void switchTo(PageId id); // đổi trang hiện tại
    void layoutContent();     // bố cục vùng content

    // Vẽ placeholder cho các trang chưa làm
    void drawPlaceholder(sf::RenderTarget &rt, const sf::String &title) const;

    // ----------- Sidebar -----------
    struct MenuItem
    {
        sf::Text text;     // label
        sf::FloatRect hit; // vùng hit test (theo toạ độ global)
        PageId id;
    };
    std::vector<MenuItem> mMenu;          // danh sách menu theo thứ tự
    sf::RectangleShape mSidebarBg;        // nền sidebar
    sf::RectangleShape mSidebarHighlight; // thanh highlight mục chọn
    sf::Text mSidebarHeaderNghiepVu;
    sf::Text mSidebarHeaderDashboard;
    sf::Text mSidebarHeaderAccount;
    sf::RectangleShape mLogoutBtnBg; // nút "Đăng xuất"
    sf::Text mLogoutText;

    PageId mCurrent{PageId::Revenue};

    // ----------- Content -----------
    sf::Vector2f mContentOrigin{180.f, 24.f};
    float mContentWidth{1080.f};

    // Page thật đã có
    AccountsPage mAccountsPage; // font tạm thời fake, sẽ set trong onAttach()

    // Text tiêu đề placeholder
    mutable sf::Text mPlaceholderTitle;

    // Font cache (đỡ gọi getFont nhiều lần)
    const sf::Font *mFontRegular{nullptr};
    const sf::Font *mFontBold{nullptr};
};
