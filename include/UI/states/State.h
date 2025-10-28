#ifndef UI_STATES_STATE_H
#define UI_STATES_STATE_H
// ============================================================
// UI/states/State.h
// Base class cho mọi màn hình (State) trong ứng dụng.
// - Cung cấp khung xử lý sự kiện, cập nhật, vẽ (chuẩn SFML).
// - onAttach() để khởi tạo khi State vừa được set vào App.
// - relayout(windowSize) để cập nhật lại layout khi cửa sổ thay đổi.
// - Kế thừa sf::Drawable để App vẽ: mWindow.draw(*mState).
//
// SỬA LỖI QUAN TRỌNG:
// - Forward-declare đúng App ở GLOBAL namespace (::App), KHÔNG phải UI::App.
// - Member mApp có kiểu ::App&.
//
// KẾT LUẬN: Base thống nhất chữ ký và ràng buộc đúng với App.
// ============================================================

#include <SFML/Graphics.hpp>

class App; // <-- App ở GLOBAL namespace

namespace UI
{

    class State : public sf::Drawable
    {
    public:
        explicit State(::App &app);
        virtual ~State();

        // Gọi ngay sau khi State được set vào App (đã có window/font sẵn sàng)
        virtual void onAttach();

        // Xử lý input (sự kiện) từ App
        virtual void handleInput(sf::Event &e) = 0;

        // Cập nhật logic theo thời gian (dt từ App loop)
        virtual void update(sf::Time dt) = 0;

        // Gọi khi kích thước cửa sổ thay đổi để bố trí lại layout (mặc định no-op)
        virtual void relayout(sf::Vector2u windowSize);

    protected:
        ::App &mApp; // <-- ĐÃ CHỈNH: tham chiếu tới App ở GLOBAL namespace

        // Vẽ state (chuẩn SFML): App sẽ gọi mWindow.draw(*this).
        // State con phải override hàm này để vẽ nội dung.
        void draw(sf::RenderTarget &target, sf::RenderStates states) const override = 0;
    };

} // namespace UI

#endif // UI_STATES_STATE_H
