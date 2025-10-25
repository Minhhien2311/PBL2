// #include "C:/PBL2/include/core/MenuManager.h"
// #include "C:/PBL2/include/main_state.h"
// ApplicationScreen current_screen = ApplicationScreen::Login;

// int main(){
//     app();
// }

// app/main.cpp
#include <iostream>

#ifdef UI_BACKEND_SFML
  #include <SFML/Graphics.hpp>  // Test include: nếu biên dịch được là include OK
#endif

int main() {
#ifdef UI_BACKEND_SFML
    // Mở 1 cửa sổ nhỏ để test SFML
    sf::RenderWindow window(sf::VideoMode(480, 320), "SFML OK");
    sf::CircleShape dot(40.f);              // Hình tròn
    dot.setPosition(200.f, 120.f);

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
        }
        window.clear();                      // Xoá nền
        window.draw(dot);                    // Vẽ hình tròn
        window.display();                    // Lật buffer
    }
#else
    std::cout << "FTXUI mode (SFML OFF)\n";
#endif
    return 0;
}
