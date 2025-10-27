#ifndef STATE_H
#define STATE_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Utf.hpp>
#include <memory>

// Forward declaration
class App;

namespace UI
{
    // Lớp cơ sở trừu tượng cho các trạng thái (màn hình) của ứng dụng
    // Kế thừa từ sf::Drawable để App có thể gọi window.draw(*state)
    class State : public sf::Drawable
    {
    public:
        // Virtual destructor rất quan trọng cho lớp cơ sở có hàm virtual
        virtual ~State() = default;

        // Hàm ảo thuần túy -> lớp kế thừa BẮT BUỘC phải định nghĩa lại
        virtual void handleInput(sf::Event &event) = 0;
        virtual void update(sf::Time dt) = 0;
        // Hàm draw kế thừa từ sf::Drawable, cũng cần định nghĩa lại
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override = 0;

        virtual void onAttach() {}

    protected:
        // Constructor được bảo vệ để chỉ lớp con gọi được
        State(App &app) : mApp(app) {}

        // Tham chiếu đến đối tượng App chính để State có thể tương tác
        // (ví dụ: chuyển State, lấy tài nguyên)
        App &mApp;
    };

} // namespace UI

#endif // STATE_H
