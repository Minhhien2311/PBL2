#include "core/App.h"
#include "UI/states/LoginState.h" // State đầu tiên cần chạy
#include <iostream>

App::App() : mWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE) {
    mWindow.setFramerateLimit(60);
    loadFonts(); // Tải font ngay khi khởi tạo App

    // Khởi tạo state đầu tiên (LoginState)
    pushState(std::make_unique<UI::LoginState>(*this));
}

void App::loadFonts() {
    // Tải các font bạn đã cung cấp
    sf::Font MulishRegular;
    if (!MulishRegular.loadFromFile("assets/fonts/Mulish-Regular.ttf")) {
        std::cerr << "Error loading Mulish-Regular.ttf" << std::endl;
        // Có thể throw exception hoặc xử lý lỗi khác
    }
    mFontManager["MulishRegular"] = MulishRegular;

    sf::Font MulishBold;
    if (!MulishBold.loadFromFile("assets/fonts/Mulish-Bold.ttf")) {
         std::cerr << "Error loading Mulish-Bold.ttf" << std::endl;
    }
    mFontManager["MulishBold"] = MulishBold;

    // Tải thêm các font khác nếu cần (Montserrat, v.v.)
    // sf::Font montserratRegular;
    // if (!montserratRegular.loadFromFile("assets/fonts/Montserrat-Regular.ttf")) { ... }
    // mFontManager["MontserratRegular"] = montserratRegular;
}

sf::Font& App::getFont(const std::string& fontName) {
    // Hàm này trả về font đã tải, cần kiểm tra key tồn tại nếu muốn an toàn hơn
    return mFontManager.at(fontName);
}


void App::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        sf::Time dt = clock.restart();
        handleEvents();
        update(dt);
        render();
    }
}

void App::handleEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        // Cho state hiện tại xử lý event trước
        if (peekState()) {
            peekState()->handleInput(event);
        }

        // Xử lý sự kiện chung của App (ví dụ: đóng cửa sổ)
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        }
    }
}

void App::update(sf::Time dt) {
    if (peekState()) {
        peekState()->update(dt);
    }
}

void App::render() {
    mWindow.clear();
    if (peekState()) {
        // Sử dụng draw của RenderTarget, không cần RenderStates ở đây
        mWindow.draw(*peekState());
    }
    mWindow.display();
}

void App::pushState(std::unique_ptr<UI::State> state) {
    mStates.push(std::move(state));
}

void App::popState() {
    if (!mStates.empty()) {
        mStates.pop();
    }
}

void App::changeState(std::unique_ptr<UI::State> state) {
    popState();
    pushState(std::move(state));
}

UI::State* App::peekState() {
    if (mStates.empty()) {
        return nullptr;
    }
    return mStates.top().get();
}

sf::RenderWindow& App::getWindow() {
    return mWindow;
}
