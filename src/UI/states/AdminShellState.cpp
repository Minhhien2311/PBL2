// ============================================================
// UI/states/AdminShellState.cpp
// Tinh chỉnh layout theo mockup:
// - Content có lề 24px;
// - Sidebar có section title & item;
// - Nút "Đăng xuất" ghim đáy.
// ============================================================

#include "UI/states/AdminShellState.h"
#include "core/App.h"
#include <algorithm>

AdminShellState::AdminShellState(::App &app)
    : UI::State(app) {}

// -------------------- onAttach --------------------
void AdminShellState::onAttach()
{
    mFontRegular = &mApp.getFont("Mulish-Regular");
    mFontBold = &mApp.getFont("Mulish-Bold");

    // Sidebar background
    mSidebarBg.setFillColor(sf::Color(19, 62, 135)); // rgba(19, 62, 135, 1)

    // Logout visuals
    mLogoutBg.setFillColor(sf::Color::Transparent);
    mLogoutBg.setOutlineThickness(2.f);
    mLogoutBg.setOutlineColor(sf::Color::White);

    mLogoutText.setFont(*mFontBold);
    mLogoutText.setCharacterSize(18);
    mLogoutText.setFillColor(sf::Color::White);
    mLogoutText.setString(L"Đăng xuất");

    // Tính layout lần đầu + tạo page mặc định
    relayout(mApp.window().getSize());
    switchTo_(NavRow::Item::Accounts);
}

// -------------------- handleInput --------------------
void AdminShellState::handleInput(sf::Event &e)
{
    if (e.type == sf::Event::MouseMoved)
    {
        const sf::Vector2f mp(static_cast<float>(e.mouseMove.x),
                              static_cast<float>(e.mouseMove.y));
        mHoverIndex = -1;
        for (int i = 0; i < static_cast<int>(mNav.size()); ++i)
        {
            if (!mNav[i].isSection && mNav[i].item != NavRow::Item::Logout &&
                hit_(mp, mNav[i].bounds))
            {
                mHoverIndex = i;
                break;
            }
        }
    }

    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        const sf::Vector2f mp(static_cast<float>(e.mouseButton.x),
                              static_cast<float>(e.mouseButton.y));

        // Click logout button
        if (hit_(mp, mLogoutBounds))
        {
            mApp.goTo(AppRoute::Login);
            return;
        }

        // Click items
        for (const auto &row : mNav)
        {
            if (row.isSection || row.item == NavRow::Item::Logout)
                continue;
            if (hit_(mp, row.bounds))
            {
                switchTo_(row.item);
                return;
            }
        }
    }

    // Forward event cho page hiện tại (Accounts)
    if (mActiveItem == NavRow::Item::Accounts && mAccountsPage)
    {
        mAccountsPage->handleEvent(e, mApp.window());
    }
}

// -------------------- update --------------------
void AdminShellState::update(sf::Time /*dt*/)
{
    if (mActiveItem == NavRow::Item::Accounts && mAccountsPage)
    {
        mAccountsPage->update(mApp.window());
    }
}

// -------------------- relayout --------------------
void AdminShellState::relayout(sf::Vector2u windowSize)
{
    // Sidebar kích thước trọn chiều cao
    mSidebarBg.setPosition({0.f, 0.f});
    mSidebarBg.setSize({mSidebarWidth, static_cast<float>(windowSize.y)});

    // Content với lề 24px theo mockup
    const float L = 24.f, T = 24.f, R = 24.f;
    mContentOrigin = {mSidebarWidth + L, T};
    mContentWidth = std::max(0.f, static_cast<float>(windowSize.x) - mSidebarWidth - (L + R));

    // Xây lại sidebar + logout đáy
    buildSidebar_();

    // Tạo lại page hiện hành (đảm bảo origin/width mới)
    if (mActiveItem == NavRow::Item::Accounts)
    {
        createAccountsPage_();
    }
}

// -------------------- draw --------------------
void AdminShellState::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // 1) Sidebar
    target.draw(mSidebarBg, states);

    // 2) Mục & section
    for (int i = 0; i < static_cast<int>(mNav.size()); ++i)
    {
        const auto &row = mNav[i];
        if (row.isSection)
        {
            sf::Text sec;
            sec.setFont(*mFontBold);
            sec.setCharacterSize(18); // rgba(203, 220, 235, 1)
            sec.setFillColor(sf::Color(203, 220, 235));
            sec.setString(row.label);
            sec.setPosition(row.bounds.left + mSidebarPadX, row.bounds.top);
            target.draw(sec, states);
            continue;
        }

        const bool isActive = (row.item == mActiveItem);
        const bool isHover = (i == mHoverIndex);

        if (isActive || isHover)
        {
            sf::RectangleShape bg;
            bg.setPosition({row.bounds.left, row.bounds.top});
            bg.setSize({row.bounds.width, row.bounds.height}); // rgba(96, 139, 193, 1)
            bg.setFillColor(isActive ? sf::Color(96, 139, 193) : sf::Color(96, 139, 193));
            target.draw(bg, states);
        }

        // Text item
        sf::Text txt;
        txt.setFont(*mFontRegular);
        txt.setCharacterSize(16);
        txt.setFillColor(sf::Color::White); // rgba(203, 220, 235, 1)
        txt.setString(row.label);
        txt.setPosition({row.bounds.left + mSidebarPadX,
                         row.bounds.top + (mNavItemHeight - 18.f) * 0.5f});
        target.draw(txt, states);
    }

    // 3) Logout ghim đáy (nút)
    target.draw(mLogoutBg, states);
    target.draw(mLogoutText, states);

    // 4) Content (Accounts)
    if (mActiveItem == NavRow::Item::Accounts && mAccountsPage)
    {
        mAccountsPage->draw(target);
    }
}

// -------------------- helpers --------------------
void AdminShellState::buildSidebar_()
{
    mNav.clear();

    // Tạo group theo mockup
    auto pushSection = [&](const wchar_t *title, float &y)
    {
        mNav.push_back({title, sf::FloatRect(0.f, y, mSidebarWidth, 18.f), true, NavRow::Item::None});
        y += 18.f + mSectionGapY;
    };
    auto pushItem = [&](const wchar_t *label, NavRow::Item item, float &y)
    {
        mNav.push_back({label, sf::FloatRect(0.f, y, mSidebarWidth, mNavItemHeight), false, item});
        y += mNavItemHeight;
    };

    float y = mSidebarPadTop;

    // NGHIỆP VỤ
    pushSection(L"NGHIỆP VỤ", y);
    pushItem(L"Quản lý tuyến bay", NavRow::Item::Routes, y);
    pushItem(L"Quản lý chuyến bay", NavRow::Item::Flights, y);
    pushItem(L"Quản lý luật bay", NavRow::Item::Flights, y);     // có thể trỏ về Flights tạm thời
    pushItem(L"Quản lý khuyến mãi", NavRow::Item::Dashboard, y); // tạm chưa có page

    y += mGroupGapY;

    // DASHBOARD
    pushSection(L"DASHBOARD", y);
    pushItem(L"Vé đã bán", NavRow::Item::Dashboard, y);
    pushItem(L"Doanh thu", NavRow::Item::Dashboard, y);

    y += mGroupGapY;

    // TÀI KHOẢN
    pushSection(L"TÀI KHOẢN", y);
    pushItem(L"Quản lý tài khoản", NavRow::Item::Accounts, y);
    pushItem(L"Danh sách đại lý", NavRow::Item::Routes, y); // tạm chưa có page

    // Logout ghim đáy
    const float h = mNavItemHeight;
    const float sidebarH = mSidebarBg.getSize().y;
    const float padBottom = 24.f;
    const float top = sidebarH - padBottom - h;

    mLogoutBounds = sf::FloatRect(0.f, top, mSidebarWidth, h);
    mLogoutBg.setPosition({mLogoutBounds.left + 16.f, mLogoutBounds.top});
    mLogoutBg.setSize({mLogoutBounds.width - 32.f, mLogoutBounds.height});
    mLogoutText.setPosition({mLogoutBg.getPosition().x + (mLogoutBg.getSize().x - mLogoutText.getLocalBounds().width) * 0.5f - 4.f,
                             mLogoutBg.getPosition().y + (mLogoutBg.getSize().y - 18.f) * 0.5f - 2.f});
}

void AdminShellState::createAccountsPage_()
{
    if (!mFontRegular)
        return;
    mAccountsPage = std::make_unique<AccountsPage>(
        *mFontRegular, mContentOrigin, mContentWidth);
}

void AdminShellState::switchTo_(NavRow::Item item)
{
    mActiveItem = item;
    if (mActiveItem == NavRow::Item::Accounts)
    {
        createAccountsPage_();
    }
    else
    {
        mAccountsPage.reset();
    }
}

bool AdminShellState::hit_(const sf::Vector2f &p, const sf::FloatRect &r)
{
    return r.contains(p);
}
