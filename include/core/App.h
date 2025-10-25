#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include <stack>
#include <memory>
#include <map>
#include "UI/states/State.h" // Cần tạo file này

// Forward declaration
namespace UI { class State; }

class App {
public:
    App();
    void run();

    void pushState(std::unique_ptr<UI::State> state);
    void popState();
    void changeState(std::unique_ptr<UI::State> state);
    UI::State* peekState();

    sf::RenderWindow& getWindow();
    sf::Font& getFont(const std::string& fontName); // Hàm để lấy font

private:
    void loadFonts(); // Hàm tải các font cần thiết
    void handleEvents();
    void update(sf::Time dt);
    void render();

    sf::RenderWindow mWindow;
    std::stack<std::unique_ptr<UI::State>> mStates;

    // Quản lý font đơn giản
    std::map<std::string, sf::Font> mFontManager;

    // Các hằng số (có thể đưa ra file config)
    const unsigned int WINDOW_WIDTH = 800;
    const unsigned int WINDOW_HEIGHT = 600;
    const std::string WINDOW_TITLE = "PBL2 Flight Booking";
};

#endif // APP_H
