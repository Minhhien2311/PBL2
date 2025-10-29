// ======================================================================
// UI/pages/FlightsPage.cpp
// Trang "Quản lý chuyến bay": ô tìm kiếm + bảng + nút chức năng.
// ======================================================================

#include "UI/pages/FlightsPage.h"

FlightsPage::FlightsPage(const sf::Font &uiFont,
                         sf::Vector2f originTopLeft,
                         float contentWidth)
    : mFont(&uiFont), mOriginTL(originTopLeft), mContentW(contentWidth)
{
    // ===== Title =====
    mTitle.setFont(*mFont);
    mTitle.setCharacterSize(26);
    mTitle.setFillColor(sf::Color(28, 64, 110));
    mTitle.setString(L"Quản lý chuyến bay");

    // ===== Labels =====
    auto cfgLabel = [&](sf::Text &t, const wchar_t *s)
    {
        t.setFont(*mFont);
        t.setCharacterSize(18);
        t.setFillColor(sf::Color(28, 64, 110));
        t.setString(s);
    };
    cfgLabel(mLblId, L"ID chuyến bay");
    cfgLabel(mLblFrom, L"Từ");
    cfgLabel(mLblTo, L"Đến");
    cfgLabel(mLblDate, L"Ngày khởi hành");
    cfgLabel(mLblTable, L"Tất cả chuyến bay");

    // ===== TextBoxes =====
    auto cfgEdit = [&](TextBox &tb, const wchar_t *placeholder)
    {
        tb.setFont(*mFont);
        tb.setCharacterSize(20);
        tb.setPlaceholder(placeholder);
        tb.setSize(mEditSize);
    };
    cfgEdit(mTbId, L"Nhập ID nội bộ");
    cfgEdit(mTbFrom, L"Điểm xuất phát");
    cfgEdit(mTbTo, L"Điểm đến");
    cfgEdit(mTbDate, L"Ngày/Tháng/Năm");

    // ===== Buttons tìm kiếm =====
    auto cfgBtn = [&](Button &b, const wchar_t *txt)
    {
        b.setSize(mBtnSize);
        b.setBackgroundColor(sf::Color(96, 139, 193));
        b.setText(txt, *mFont, 18);
        b.setTextColor(sf::Color::White);
    };
    cfgBtn(mBtnSearchId, L"Tìm theo ID");
    cfgBtn(mBtnSearchRoute, L"Tìm theo lộ trình bay");
    cfgBtn(mBtnSearchDate, L"Tìm theo ngày khởi hành");

    // ===== Buttons hành động =====
    cfgBtn(mBtnAdd, L"Thêm chuyến");
    cfgBtn(mBtnEdit, L"Sửa chuyến");
    cfgBtn(mBtnDelete, L"Xóa chuyến");

    // ===== Table (border + header giả lập) =====
    mTableBorder.setOutlineThickness(2.f);
    mTableBorder.setOutlineColor(sf::Color(180, 200, 230));
    mTableBorder.setFillColor(sf::Color::Transparent);

    std::vector<std::wstring> headers = {
        L"ID chuyến", L"ID tuyến", L"Thời gian khởi hành",
        L"Thời gian cất cánh", L"Hạng phổ thông", L"Hạng thương gia",
        L"Giá phổ thông", L"Giá thương gia"};
    for (const auto &h : headers)
    {
        sf::Text t;
        t.setFont(*mFont);
        t.setCharacterSize(16);
        t.setFillColor(sf::Color(28, 64, 110));
        t.setString(h);
        mTableHeaders.push_back(t);
    }

    // Layout + actions
    wireActions_();
    layout_();
}

// -------------------- handleEvent/update/draw --------------------
void FlightsPage::handleEvent(const sf::Event &ev, const sf::RenderWindow &win)
{
    mTbId.handleEvent(ev, win);
    mTbFrom.handleEvent(ev, win);
    mTbTo.handleEvent(ev, win);
    mTbDate.handleEvent(ev, win);

    mBtnSearchId.handleEvent(ev, win);
    mBtnSearchRoute.handleEvent(ev, win);
    mBtnSearchDate.handleEvent(ev, win);

    mBtnAdd.handleEvent(ev, win);
    mBtnEdit.handleEvent(ev, win);
    mBtnDelete.handleEvent(ev, win);
}

void FlightsPage::update(const sf::RenderWindow &win)
{
    mBtnSearchId.update(win);
    mBtnSearchRoute.update(win);
    mBtnSearchDate.update(win);
    mBtnAdd.update(win);
    mBtnEdit.update(win);
    mBtnDelete.update(win);
}

void FlightsPage::draw(sf::RenderTarget &target) const
{
    target.draw(mTitle);
    target.draw(mLblId);
    target.draw(mLblFrom);
    target.draw(mLblTo);
    target.draw(mLblDate);

    target.draw(mTbId);
    target.draw(mTbFrom);
    target.draw(mTbTo);
    target.draw(mTbDate);

    target.draw(mBtnSearchId);
    target.draw(mBtnSearchRoute);
    target.draw(mBtnSearchDate);

    // Table title + border
    target.draw(mLblTable);
    target.draw(mTableBorder);
    for (auto &h : mTableHeaders)
        target.draw(h);

    // Bottom buttons
    target.draw(mBtnAdd);
    target.draw(mBtnEdit);
    target.draw(mBtnDelete);
}

// -------------------- form data --------------------
void FlightsPage::setForm(const SearchForm &f)
{
    mTbId.setText(f.flightId);
    mTbFrom.setText(f.departure);
    mTbTo.setText(f.arrival);
    mTbDate.setText(f.date);
}

FlightsPage::SearchForm FlightsPage::getForm() const
{
    return {
        mTbId.getText(),
        mTbFrom.getText(),
        mTbTo.getText(),
        mTbDate.getText()};
}

// -------------------- layout helpers --------------------
void FlightsPage::layout_()
{
    const float availLeft = mOriginTL.x + mMarginLR;
    const float availTop = mOriginTL.y + mMarginTB;
    const float availW = std::max(0.f, mContentW - 2.f * mMarginLR);
    float y = availTop;

    // Title
    mTitle.setPosition(availLeft, y);
    y += mTitle.getLocalBounds().height + mTitleGap + 12.f;

    // Hàng nhập liệu
    float x = availLeft;
    mTbId.setPosition({x + mEditSize.x * 0.5f, y + mEditSize.y * 0.5f});
    placeLabelAbove_(mLblId, {x + mEditSize.x * 0.5f, y + mEditSize.y * 0.5f});

    x += mEditSize.x + 40.f;
    mTbFrom.setPosition({x + mEditSize.x * 0.5f, y + mEditSize.y * 0.5f});
    placeLabelAbove_(mLblFrom, {x + mEditSize.x * 0.5f, y + mEditSize.y * 0.5f});

    x += mEditSize.x + 20.f;
    mTbTo.setPosition({x + mEditSize.x * 0.5f, y + mEditSize.y * 0.5f});
    placeLabelAbove_(mLblTo, {x + mEditSize.x * 0.5f, y + mEditSize.y * 0.5f});

    x += mEditSize.x + 20.f;
    mTbDate.setPosition({x + mEditSize.x * 0.5f, y + mEditSize.y * 0.5f});
    placeLabelAbove_(mLblDate, {x + mEditSize.x * 0.5f, y + mEditSize.y * 0.5f});

    y += mEditSize.y + 16.f;

    // Buttons tìm kiếm
    float btnY = y + 8.f + mBtnSize.y * 0.5f;
    x = availLeft + 60.f;
    mBtnSearchId.setPosition({x, btnY});
    mBtnSearchRoute.setPosition({x + mBtnSize.x + 200.f, btnY});
    mBtnSearchDate.setPosition({x + mBtnSize.x * 2.f + 420.f, btnY});

    // Table
    y += mBtnSize.y + 36.f;
    mLblTable.setPosition(availLeft, y);
    y += 30.f;
    mTableBorder.setPosition({availLeft, y});
    mTableBorder.setSize({availW, 300.f});

    // Header
    float colW = availW / static_cast<float>(mTableHeaders.size());
    for (int i = 0; i < (int)mTableHeaders.size(); ++i)
    {
        auto &h = mTableHeaders[i];
        h.setPosition(availLeft + colW * i + 6.f, y + 6.f);
    }

    // Bottom buttons
    y += 340.f;
    const float midX = availLeft + availW / 2.f;
    mBtnAdd.setPosition({midX - mBtnSize.x - 140.f, y});
    mBtnEdit.setPosition({midX, y});
    mBtnDelete.setPosition({midX + mBtnSize.x + 140.f, y});
}

void FlightsPage::placeLabelAbove_(sf::Text &label, const sf::Vector2f &tbCenter) const
{
    const float left = tbCenter.x - mEditSize.x * 0.5f;
    const float top = tbCenter.y - mEditSize.y * 0.5f - mLabelDy - 4.f;
    label.setPosition(left, top);
}

void FlightsPage::wireActions_()
{
    mBtnSearchId.setOnAction([this]()
                             {
        if (onSearchById) onSearchById(getForm()); });
    mBtnSearchRoute.setOnAction([this]()
                                {
        if (onSearchByRoute) onSearchByRoute(getForm()); });
    mBtnSearchDate.setOnAction([this]()
                               {
        if (onSearchByDate) onSearchByDate(getForm()); });

    mBtnAdd.setOnAction([this]()
                        {
        if (onAddFlight) onAddFlight(); });
    mBtnEdit.setOnAction([this]()
                         {
        if (onEditFlight) onEditFlight(); });
    mBtnDelete.setOnAction([this]()
                           {
        if (onDeleteFlight) onDeleteFlight(); });
}
