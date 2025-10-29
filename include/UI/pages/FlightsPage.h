#pragma once
// ======================================================================
// UI/pages/FlightsPage.h
//
// Trang "Quản lý chuyến bay" (hiển thị trong AdminShellState).
// - Bố cục: tiêu đề + hàng tìm kiếm (ID, lộ trình, ngày khởi hành)
// - Bảng kết quả (vẽ giả lập) + 3 nút Thêm / Sửa / Xóa.
// - TextBox và Button định vị theo CENTER.
// - Callback bắn về AdminShell để xử lý.
//
// Public API:
//   FlightsPage(const sf::Font& uiFont, sf::Vector2f originTopLeft, float contentWidth);
//   void handleEvent(const sf::Event&, const sf::RenderWindow&);
//   void update(const sf::RenderWindow&);
//   void draw(sf::RenderTarget&) const;
//
// ======================================================================

#include <SFML/Graphics.hpp>
#include <functional>
#include "UI/components/TextBox.h"
#include "UI/components/Button.h"

class FlightsPage
{
public:
    struct SearchForm
    {
        sf::String flightId;
        sf::String departure;
        sf::String arrival;
        sf::String date;
    };

public:
    FlightsPage(const sf::Font &uiFont, sf::Vector2f originTopLeft, float contentWidth);

    void handleEvent(const sf::Event &ev, const sf::RenderWindow &win);
    void update(const sf::RenderWindow &win);
    void draw(sf::RenderTarget &target) const;

    void setForm(const SearchForm &f);
    SearchForm getForm() const;

    // Callback cho hành động
    std::function<void(const SearchForm &)> onSearchById;
    std::function<void(const SearchForm &)> onSearchByRoute;
    std::function<void(const SearchForm &)> onSearchByDate;
    std::function<void()> onAddFlight;
    std::function<void()> onEditFlight;
    std::function<void()> onDeleteFlight;

private:
    const sf::Font *mFont = nullptr;
    sf::Vector2f mOriginTL{0.f, 0.f};
    float mContentW = 0.f;

    // Layout constants
    sf::Vector2f mEditSize{240.f, 40.f};
    sf::Vector2f mBtnSize{180.f, 46.f};
    float mRowGap = 32.f;
    float mTitleGap = 16.f;
    float mMarginLR = 24.f;
    float mMarginTB = 24.f;
    float mLabelDy = 20.f;

    // UI elements
    sf::Text mTitle;
    sf::Text mLblId, mLblFrom, mLblTo, mLblDate, mLblTable;

    TextBox mTbId, mTbFrom, mTbTo, mTbDate;

    Button mBtnSearchId, mBtnSearchRoute, mBtnSearchDate;
    Button mBtnAdd, mBtnEdit, mBtnDelete;

    // Bảng (giả lập bằng khung)
    sf::RectangleShape mTableBorder;
    std::vector<sf::Text> mTableHeaders;

private:
    void layout_();
    void placeLabelAbove_(sf::Text &label, const sf::Vector2f &tbCenter) const;
    void wireActions_();
};
