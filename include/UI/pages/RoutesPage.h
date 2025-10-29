#pragma once
// ======================================================================
// UI/pages/RoutesPage.h
//
// Trang "Quản lý tuyến bay" (hiển thị trong AdminShellState).
// - Bố cục: tiêu đề + 2 cột × 3 hàng nhập liệu (Mã tuyến, Điểm đi, Điểm đến | Khoảng cách, Thời gian bay, Ghi chú)
// - Hai nút căn giữa: "Thêm tuyến bay" và "Cập nhật tuyến".
// - TextBox/Button định vị theo CENTER (giống AccountsPage).
// - Callback bắn về AdminShell để xử lý logic.
//
// Public API:
//   RoutesPage(const sf::Font& uiFont, sf::Vector2f originTopLeft, float contentWidth);
//   void handleEvent(const sf::Event&, const sf::RenderWindow&);
//   void update(const sf::RenderWindow&);
//   void draw(sf::RenderTarget&) const;
//
//   struct RouteForm { sf::String routeId, departure, arrival, distance, duration, note; };
//   void setForm(const RouteForm&);
//   RouteForm getForm() const;
//
//   std::function<void(const RouteForm&)> onSubmitAdd;
//   std::function<void(const RouteForm&)> onSubmitUpdate;
// ======================================================================

#include <SFML/Graphics.hpp>
#include <functional>
#include "UI/components/TextBox.h"
#include "UI/components/Button.h"

class RoutesPage
{
public:
    struct RouteForm
    {
        sf::String routeId;
        sf::String departure;
        sf::String arrival;
        sf::String distance;
        sf::String duration;
        sf::String note;
    };

public:
    RoutesPage(const sf::Font &uiFont, sf::Vector2f originTopLeft, float contentWidth);

    void handleEvent(const sf::Event &ev, const sf::RenderWindow &win);
    void update(const sf::RenderWindow &win);
    void draw(sf::RenderTarget &target) const;

    void setForm(const RouteForm &f);
    RouteForm getForm() const;

    std::function<void(const RouteForm &)> onSubmitAdd;
    std::function<void(const RouteForm &)> onSubmitUpdate;

private:
    const sf::Font *mFont = nullptr;

    sf::Vector2f mOriginTL{0.f, 0.f};
    float mContentW = 0.f;

    // Layout constants
    sf::Vector2f mEditSize{420.f, 40.f};
    sf::Vector2f mBtnSize{260.f, 52.f};
    float mColGap = 72.f;
    float mRowGap = 64.f;
    float mTitleGap = 16.f;
    float mLabelDy = 26.f;
    float mMarginLR = 24.f;
    float mMarginTB = 24.f;

    // UI elements
    sf::Text mTitle;
    sf::Text mLblRouteId, mLblDeparture, mLblArrival, mLblDistance, mLblDuration, mLblNote;

    TextBox mTbRouteId, mTbDeparture, mTbArrival, mTbDistance, mTbDuration, mTbNote;

    sf::Vector2f mCRouteId{}, mCDeparture{}, mCArrival{}, mCDistance{}, mCDuration{}, mCNote{};

    Button mBtnAdd;
    Button mBtnUpdate;

private:
    void layout_();
    void placeLabelAbove_(sf::Text &label, const sf::Vector2f &tbCenter) const;
    void wireActions_();
};
