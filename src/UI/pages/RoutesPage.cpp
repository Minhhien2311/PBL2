// ======================================================================
// UI/pages/RoutesPage.cpp
// Trang "Quản lý tuyến bay": tiêu đề + 2 cột × 3 hàng + 2 nút.
// ======================================================================

#include "UI/pages/RoutesPage.h"

// -------------------- ctor --------------------
RoutesPage::RoutesPage(const sf::Font &uiFont,
                       sf::Vector2f originTopLeft,
                       float contentWidth)
    : mFont(&uiFont), mOriginTL(originTopLeft), mContentW(contentWidth)
{
    // ===== Title =====
    mTitle.setFont(*mFont);
    mTitle.setCharacterSize(26);
    mTitle.setFillColor(sf::Color(28, 64, 110));
    mTitle.setString(L"Quản lý tuyến bay");

    // ===== Labels =====
    auto cfgLabel = [&](sf::Text &t, const wchar_t *s)
    {
        t.setFont(*mFont);
        t.setCharacterSize(18);
        t.setFillColor(sf::Color(28, 64, 110));
        t.setString(s);
    };
    cfgLabel(mLblRouteId, L"Mã tuyến");
    cfgLabel(mLblDeparture, L"Điểm đi");
    cfgLabel(mLblArrival, L"Điểm đến");
    cfgLabel(mLblDistance, L"Khoảng cách (km)");
    cfgLabel(mLblDuration, L"Thời gian bay (phút)");
    cfgLabel(mLblNote, L"Ghi chú");

    // ===== TextBoxes =====
    auto cfgEdit = [&](TextBox &tb, const wchar_t *placeholder)
    {
        tb.setFont(*mFont);
        tb.setCharacterSize(20);
        tb.setPlaceholder(placeholder);
        tb.setSize(mEditSize);
    };
    cfgEdit(mTbRouteId, L"Nhập mã tuyến");
    cfgEdit(mTbDeparture, L"Nhập điểm đi");
    cfgEdit(mTbArrival, L"Nhập điểm đến");
    cfgEdit(mTbDistance, L"Nhập khoảng cách");
    cfgEdit(mTbDuration, L"Nhập thời gian bay");
    cfgEdit(mTbNote, L"Nhập ghi chú");

    // ===== Buttons =====
    mBtnAdd.setSize(mBtnSize);
    mBtnAdd.setBackgroundColor(sf::Color(96, 139, 193));
    mBtnAdd.setText(L"Thêm tuyến bay", *mFont, 20);
    mBtnAdd.setTextColor(sf::Color::White);

    mBtnUpdate.setSize(mBtnSize);
    mBtnUpdate.setBackgroundColor(sf::Color(96, 139, 193));
    mBtnUpdate.setText(L"Cập nhật tuyến", *mFont, 20);
    mBtnUpdate.setTextColor(sf::Color::White);

    // Layout + actions
    wireActions_();
    layout_();
}

// -------------------- handleEvent/update/draw --------------------
void RoutesPage::handleEvent(const sf::Event &ev, const sf::RenderWindow &win)
{
    mTbRouteId.handleEvent(ev, win);
    mTbDeparture.handleEvent(ev, win);
    mTbArrival.handleEvent(ev, win);
    mTbDistance.handleEvent(ev, win);
    mTbDuration.handleEvent(ev, win);
    mTbNote.handleEvent(ev, win);

    mBtnAdd.handleEvent(ev, win);
    mBtnUpdate.handleEvent(ev, win);
}

void RoutesPage::update(const sf::RenderWindow &win)
{
    mBtnAdd.update(win);
    mBtnUpdate.update(win);
}

void RoutesPage::draw(sf::RenderTarget &target) const
{
    target.draw(mTitle);
    target.draw(mLblRouteId);
    target.draw(mLblDeparture);
    target.draw(mLblArrival);
    target.draw(mLblDistance);
    target.draw(mLblDuration);
    target.draw(mLblNote);

    target.draw(mTbRouteId);
    target.draw(mTbDeparture);
    target.draw(mTbArrival);
    target.draw(mTbDistance);
    target.draw(mTbDuration);
    target.draw(mTbNote);

    target.draw(mBtnAdd);
    target.draw(mBtnUpdate);
}

// -------------------- form data --------------------
void RoutesPage::setForm(const RouteForm &f)
{
    mTbRouteId.setText(f.routeId);
    mTbDeparture.setText(f.departure);
    mTbArrival.setText(f.arrival);
    mTbDistance.setText(f.distance);
    mTbDuration.setText(f.duration);
    mTbNote.setText(f.note);
}

RoutesPage::RouteForm RoutesPage::getForm() const
{
    return {
        mTbRouteId.getText(),
        mTbDeparture.getText(),
        mTbArrival.getText(),
        mTbDistance.getText(),
        mTbDuration.getText(),
        mTbNote.getText()};
}

// -------------------- layout helpers --------------------
void RoutesPage::layout_()
{
    const float availLeft = mOriginTL.x + mMarginLR;
    const float availTop = mOriginTL.y + mMarginTB;
    const float availW = std::max(0.f, mContentW - 2.f * mMarginLR);

    const float colWidth = mEditSize.x;
    const float totalW = colWidth * 2.f + mColGap;
    const float startX = availLeft + std::max(0.f, (availW - totalW) * 0.5f);

    const float colLeftCX = startX - 30.f + colWidth * 0.5f;
    const float colRightCX = startX + colWidth + mColGap + colWidth * 0.5f;

    float y = availTop;
    mTitle.setPosition(startX - 30.f, y);
    y += mTitle.getLocalBounds().height + mTitleGap;

    // Hàng 1: Mã tuyến | Điểm đi
    mCRouteId = {colLeftCX, y + 50.f + mEditSize.y * 0.5f};
    mCDeparture = {colRightCX, y + 50.f + mEditSize.y * 0.5f};
    mTbRouteId.setPosition(mCRouteId);
    mTbDeparture.setPosition(mCDeparture);
    placeLabelAbove_(mLblRouteId, mCRouteId);
    placeLabelAbove_(mLblDeparture, mCDeparture);
    y += mEditSize.y + mRowGap;

    // Hàng 2: Điểm đến | Khoảng cách
    mCArrival = {colLeftCX, y + 50.f + mEditSize.y * 0.5f};
    mCDistance = {colRightCX, y + 50.f + mEditSize.y * 0.5f};
    mTbArrival.setPosition(mCArrival);
    mTbDistance.setPosition(mCDistance);
    placeLabelAbove_(mLblArrival, mCArrival);
    placeLabelAbove_(mLblDistance, mCDistance);
    y += mEditSize.y + mRowGap;

    // Hàng 3: Thời gian bay | Ghi chú
    mCDuration = {colLeftCX, y + 50.f + mEditSize.y * 0.5f};
    mCNote = {colRightCX, y + 50.f + mEditSize.y * 0.5f};
    mTbDuration.setPosition(mCDuration);
    mTbNote.setPosition(mCNote);
    placeLabelAbove_(mLblDuration, mCDuration);
    placeLabelAbove_(mLblNote, mCNote);
    y += mEditSize.y + mRowGap;

    // Buttons
    const float midX = (colLeftCX + colRightCX) * 0.5f;
    mBtnAdd.setPosition({midX, y + 50.f + mBtnSize.y * 0.5f});
    y += mBtnSize.y + 18.f;
    mBtnUpdate.setPosition({midX, y + 50.f + mBtnSize.y * 0.5f});
}

void RoutesPage::placeLabelAbove_(sf::Text &label, const sf::Vector2f &tbCenter) const
{
    const float left = tbCenter.x - mEditSize.x * 0.5f;
    const float top = tbCenter.y - mEditSize.y * 0.5f - mLabelDy - 4.f;
    label.setPosition(left, top);
}

void RoutesPage::wireActions_()
{
    mBtnAdd.setOnAction([this]()
                        {
        if (onSubmitAdd) onSubmitAdd(getForm()); });
    mBtnUpdate.setOnAction([this]()
                           {
        if (onSubmitUpdate) onSubmitUpdate(getForm()); });
}
