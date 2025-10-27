#include "UI/states/AdminShellState.h"
#include <SFML/System/Utf.hpp>    // nếu bạn cần convert UTF (chưa dùng ở đây)
#include "core/App.h"             // Required for App::accountMgr()
#include "UI/states/LoginState.h" // Required for LoginState

// ====== Helpers ngắn ======
static sf::Text makeText(const sf::Font &f, const sf::String &s, unsigned size,
                         sf::Color col, sf::Vector2f pos)
{
    sf::Text t;
    t.setFont(f);
    t.setString(s);
    t.setCharacterSize(size);
    t.setFillColor(col);
    t.setPosition(pos);
    return t;
}

void AdminShellState::onAttach()
{
    // Lấy font từ App (đổi key cho khớp với App của bạn)
    mFontRegular = &mApp.getFont("Mulish-Regular");
    mFontBold = &mApp.getFont("Mulish-Bold");

    // Khởi tạo page Accounts với font đúng (vì ở header mình phải dựng tạm)
    new (&mAccountsPage) AccountsPage(*mFontRegular, {mContentOrigin.x, 40.f}, mContentWidth);

    // Đổ sẵn thông tin người dùng (nếu có)
    if (Account *acc = mApp.accountMgr().getCurrentUser())
    {
        AccountsPage::Profile p;
        p.id = acc->getId(); // đổi theo API Account*
        p.fullName = acc->getFullName();
        p.username = acc->getUsername();
        p.phone = acc->getPhone();
        p.role = L"Admin"; // hoặc lấy từ acc
        p.email = acc->getEmail();
        mAccountsPage.setInitialProfile(p);
    }

    // Nối callback của AccountsPage
    // mAccountsPage.setOnSave([this](const AccountsPage::Profile &p)
    //                         {
    //                             // TODO: convert sf::String -> UTF-8 rồi gọi AccountManager cập nhật
    //                             // (tuỳ theo API của bạn). Tạm thời in/log hoặc làm gì đó đơn giản.
    //                             // Ví dụ:
    //                             // mApp.accountMgr().updateProfile(...);
    //                         });
    // mAccountsPage.setOnChangePassword([this]()
    //                                   {
    //                                       // TODO: mở dialog đổi mật khẩu
    //                                   });

    // Sidebar
    buildSidebar();
    layoutContent();
    switchTo(PageId::Revenue); // mặc định vào Doanh thu

    // Placeholder title
    mPlaceholderTitle.setFont(*mFontBold);
    mPlaceholderTitle.setCharacterSize(28);
    mPlaceholderTitle.setFillColor(sf::Color(28, 64, 110));
}

void AdminShellState::buildSidebar()
{
    // Nền sidebar
    mSidebarBg.setSize({160.f, (float)mApp.window().getSize().y});
    mSidebarBg.setPosition({0.f, 0.f});
    mSidebarBg.setFillColor(sf::Color(23, 63, 120)); // xanh đậm

    mSidebarHighlight.setSize({160.f, 34.f});
    mSidebarHighlight.setFillColor(sf::Color(255, 255, 255, 28));
    mSidebarHighlight.setPosition({0.f, 0.f});

    // Group headers
    mSidebarHeaderNghiepVu = makeText(*mFontBold, L"NGHIỆP VỤ", 14, sf::Color(220, 230, 245), {14.f, 80.f});
    mSidebarHeaderDashboard = makeText(*mFontBold, L"DASHBOARD", 14, sf::Color(220, 230, 245), {14.f, 190.f});
    mSidebarHeaderAccount = makeText(*mFontBold, L"TÀI KHOẢN", 14, sf::Color(220, 230, 245), {14.f, 250.f});

    // Menu items (thứ tự hiển thị + id)
    mMenu.clear();
    auto addItem = [&](sf::Vector2f pos, const sf::String &name, PageId id)
    {
        MenuItem it;
        it.text = makeText(*mFontRegular, name, 16, sf::Color(240, 244, 252), pos);
        it.id = id;
        // tạo hit box rộng toàn chiều ngang
        it.hit = {0.f, pos.y - 4.f, 160.f, 28.f};
        mMenu.push_back(it);
    };

    float y = 108.f;
    addItem({20.f, y}, L"Quản lý tuyến bay", PageId::Routes);
    y += 28.f;
    addItem({20.f, y}, L"Quản lý chuyến bay", PageId::Flights);
    y += 28.f;
    addItem({20.f, y}, L"Quản lý lượt bay", PageId::Revenue);
    y += 28.f; // tạm map -> Revenue
    addItem({20.f, y}, L"Quản lý khuyến mãi", PageId::Promotions);
    y += 40.f;

    mSidebarHeaderDashboard.setPosition({14.f, y});
    y += 20.f;
    addItem({20.f, y}, L"Vé đã bán", PageId::TicketsSold);
    y += 28.f;
    addItem({20.f, y}, L"Doanh thu", PageId::Revenue);
    y += 40.f;

    mSidebarHeaderAccount.setPosition({14.f, y});
    y += 20.f;
    addItem({20.f, y}, L"Quản lý tài khoản", PageId::Accounts);
    y += 28.f;
    addItem({20.f, y}, L"Danh sách đại lý", PageId::Agents);
    y += 48.f;

    // Nút đăng xuất
    mLogoutBtnBg.setSize({132.f, 40.f});
    mLogoutBtnBg.setPosition({14.f, (float)mApp.window().getSize().y - 54.f});
    mLogoutBtnBg.setFillColor(sf::Color(220, 234, 255, 35));
    mLogoutText = makeText(*mFontRegular, L"Đăng xuất", 18, sf::Color(240, 244, 252),
                           {mLogoutBtnBg.getPosition().x + 18.f, mLogoutBtnBg.getPosition().y + 8.f});
}

void AdminShellState::layoutContent()
{
    // hiện chưa cần nhiều – mình để gọn ở onAttach; trang nào cần tính thêm thì bổ sung
}

void AdminShellState::switchTo(PageId id)
{
    mCurrent = id;

    // Đặt highlight về đúng item
    for (auto &it : mMenu)
    {
        if (it.id == id)
        {
            mSidebarHighlight.setPosition({0.f, it.hit.top});
            break;
        }
    }
}

void AdminShellState::handleInput(sf::Event &e)
{
    const sf::RenderWindow &win = mApp.window();

    // Click chọn menu / đăng xuất
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = (sf::Vector2f)sf::Mouse::getPosition(win);

        // nút đăng xuất
        if (mLogoutBtnBg.getGlobalBounds().contains(mp))
        {
            mApp.accountMgr().logout();
            mApp.goTo(AppRoute::Login);
            return;
        }
        // duyệt menu
        for (auto &it : mMenu)
        {
            if (it.hit.contains(mp))
            {
                switchTo(it.id);
                break;
            }
        }
    }

    // Forward event cho page hiện tại
    switch (mCurrent)
    {
    case PageId::Accounts:
        mAccountsPage.handleEvent(e, win);
        break;
    default:
        break;
    }
}

void AdminShellState::update(sf::Time dt)
{
    const sf::RenderWindow &win = mApp.window();
    (void)dt;

    switch (mCurrent)
    {
    case PageId::Accounts:
        mAccountsPage.update(win);
        break;
    default:
        break;
    }
}

void AdminShellState::draw(sf::RenderTarget &rt, sf::RenderStates) const
{
    // Sidebar
    rt.draw(mSidebarBg);
    rt.draw(mSidebarHeaderNghiepVu);
    rt.draw(mSidebarHeaderDashboard);
    rt.draw(mSidebarHeaderAccount);
    for (auto &it : mMenu)
        rt.draw(it.text);
    rt.draw(mSidebarHighlight);
    rt.draw(mLogoutBtnBg);
    rt.draw(mLogoutText);

    // Content
    switch (mCurrent)
    {
    case PageId::Revenue:
        drawPlaceholder(rt, L"Thống kê doanh thu");
        break;
    case PageId::Accounts:
        mAccountsPage.draw(rt);
        break;
    case PageId::Routes:
        drawPlaceholder(rt, L"Quản lý tuyến bay");
        break;
    case PageId::Flights:
        drawPlaceholder(rt, L"Quản lý chuyến bay");
        break;
    case PageId::Promotions:
        drawPlaceholder(rt, L"Quản lý khuyến mãi");
        break;
    case PageId::TicketsSold:
        drawPlaceholder(rt, L"Vé đã bán");
        break;
    case PageId::Agents:
        drawPlaceholder(rt, L"Danh sách đại lý");
        break;
    }
}

void AdminShellState::drawPlaceholder(sf::RenderTarget &rt, const sf::String &title) const
{
    mPlaceholderTitle.setString(title);
    mPlaceholderTitle.setPosition(mContentOrigin.x + 20.f, 40.f);
    rt.draw(mPlaceholderTitle);
}
