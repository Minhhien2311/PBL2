#pragma once
// ======================================================================
// UI/pages/AccountsPage.h
//
// Trang "Quản lý tài khoản" (nội dung bên phải của AdminShellState).
// - Bố cục: tiêu đề + 2 cột × 3 hàng ô nhập (ID, Tên TK, Chức vụ | Họ tên, SĐT, Email)
// - Hai nút căn giữa: "Cập nhập thông tin" và "Đổi mật khẩu".
// - TextBox/Button định vị theo CENTER (đúng API của bạn), Label theo TOP-LEFT.
// - Không truy cập AccountManager trực tiếp; bắn callback để AdminShell xử lý.
//
// Public API:
//   AccountsPage(const sf::Font& uiFont, sf::Vector2f originTopLeft, float contentWidth);
//   void handleEvent(const sf::Event&, const sf::RenderWindow&);
//   void update(const sf::RenderWindow&);
//   void draw(sf::RenderTarget&) const;
//
//   // Dữ liệu form (sf::String để giữ Unicode an toàn):
//   struct AccountForm { sf::String id, fullName, username, phone, role, email; };
//   void setForm(const AccountForm&);
//   AccountForm getForm() const;
//
//   // Callback để AdminShell gắn:
//   std::function<void(const AccountForm&)> onSubmitUpdate;
//   std::function<void()>                   onChangePassword;
//
// ======================================================================

#include <SFML/Graphics.hpp>
#include <functional>

#include "UI/components/TextBox.h"
#include "UI/components/Button.h"

class AccountsPage
{
public:
    struct AccountForm
    {
        sf::String id;
        sf::String fullName;
        sf::String username;
        sf::String phone;
        sf::String role;
        sf::String email;
    };

public:
    // ctor: nhận font UI, gốc trái-trên vùng content, và bề rộng content
    AccountsPage(const sf::Font &uiFont, sf::Vector2f originTopLeft, float contentWidth);

    // ===== Vòng đời trang =====
    void handleEvent(const sf::Event &ev, const sf::RenderWindow &win); // nhận event chuột/phím
    void update(const sf::RenderWindow &win);                           // hover/caret blink, v.v.
    void draw(sf::RenderTarget &target) const;                          // vẽ toàn bộ trang

    // ===== Dữ liệu form =====
    void setForm(const AccountForm &f); // nạp dữ liệu vào 6 ô
    AccountForm getForm() const;        // lấy dữ liệu đang hiển thị

    // ===== Callback hành động =====
    std::function<void(const AccountForm &)> onSubmitUpdate; // "Cập nhập thông tin"
    std::function<void()> onChangePassword;                  // "Đổi mật khẩu"

    // ===== Chế độ chỉ đọc =====
    void setReadOnly(bool v) { mReadOnly = v; }
    bool isReadOnly() const { return mReadOnly; }

private:
    // ===== Tài nguyên & thông số layout =====
    const sf::Font *mFont = nullptr;

    sf::Vector2f mOriginTL{0.f, 0.f}; // gốc trái-trên vùng content
    float mContentW = 0.f;            // bề rộng vùng content

    // Kích thước control & khoảng cách
    sf::Vector2f mEditSize{420.f, 40.f};
    sf::Vector2f mBtnSize{260.f, 52.f};
    float mColGap = 72.f;   // khoảng cách giữa 2 cột
    float mRowGap = 64.f;   // khoảng cách giữa các hàng
    float mTitleGap = 16.f; // khoảng cách từ tiêu đề đến hàng đầu
    float mLabelDy = 26.f;  // nhãn đặt cao hơn ô nhập 26px
    float mMarginLR = 24.f; // lề trái-phải bên trong content
    float mMarginTB = 24.f; // lề trên-dưới bên trong content

private:
    bool mReadOnly = true; // BẬT mặc định nếu bạn muốn chỉ hiển thị

    // ===== Tiêu đề & Nhãn =====
    sf::Text mTitle;
    sf::Text mLblId, mLblFullName, mLblUsername, mLblPhone, mLblRole, mLblEmail;

    // ===== 6 TextBox (CENTER-based) =====
    TextBox mTbId, mTbFullName, mTbUsername, mTbPhone, mTbRole, mTbEmail;

    // Lưu tâm của từng ô để đặt nhãn TOP-LEFT theo đúng cạnh trái ô
    sf::Vector2f mCId{}, mCFullName{}, mCUsername{}, mCPhone{}, mCRole{}, mCEmail{};

    // ===== 2 Button (CENTER-based) =====
    Button mBtnUpdate;
    Button mBtnChangePw;

private:
    // Tính toán & áp vị trí (gọi trong ctor)
    void layout_();

    // Đặt nhãn theo TOP-LEFT dựa trên center của textbox
    void placeLabelAbove_(sf::Text &label, const sf::Vector2f &tbCenter) const;

    // Gắn hành vi cho 2 nút
    void wireActions_();
};
