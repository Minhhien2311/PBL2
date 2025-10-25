#include "UI/states/LoginState.h"
#include "core/App.h" // Để lấy window và font
#include <iostream> // Tạm thời để in debug

namespace UI {

LoginState::LoginState(App& app)
    : State(app),
      mFontRegular(app.getFont("PoppinsRegular")), // Lấy font từ App
      mFontBold(app.getFont("PoppinsBold")) {
    initUI();
}

void LoginState::initUI() {
    // Tải Texture (tương tự như trong main.cpp cũ)
    if (!mBackgroundTexture.loadFromFile("assets/icon/welcomestate.png")) {
        std::cerr << "Error loading background texture in LoginState!" << std::endl;
    }
    mBackgroundSprite.setTexture(mBackgroundTexture);
    // Scale nền cho vừa cửa sổ
    float scaleX = (float)mApp.getWindow().getSize().x / mBackgroundTexture.getSize().x;
    float scaleY = (float)mApp.getWindow().getSize().y / mBackgroundTexture.getSize().y;
    mBackgroundSprite.setScale(scaleX, scaleY);


    if (!mIconTexture.loadFromFile("assets/icon/AdminLogin.png")) {
        std::cerr << "Error loading icon texture in LoginState!" << std::endl;
    }
    mIconSprite.setTexture(mIconTexture);
    mIconSprite.setOrigin(mIconTexture.getSize().x / 2.0f, mIconTexture.getSize().y / 2.0f);
    mIconSprite.setPosition(mApp.getWindow().getSize().x / 2.0f, mApp.getWindow().getSize().y * 0.2f);

    // --- Thiết lập các thành phần UI ---
    // Tiêu đề
    mTitleText.setFont(mFontBold); // Dùng font Poppins Bold
    mTitleText.setString("Admin Login");
    mTitleText.setCharacterSize(30);
    mTitleText.setFillColor(sf::Color::White);
    sf::FloatRect textRect = mTitleText.getLocalBounds();
    mTitleText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    mTitleText.setPosition(mIconSprite.getPosition().x, mIconSprite.getPosition().y + mIconTexture.getSize().y / 1.5f + 20.f);

    // Labels
    mUsernameLabel.setFont(mFontRegular); // Dùng font Poppins Regular
    mUsernameLabel.setString("Username");
    mUsernameLabel.setCharacterSize(16);
    mUsernameLabel.setFillColor(sf::Color(255, 255, 255, 180)); // Trắng hơi mờ

    mPasswordLabel.setFont(mFontRegular);
    mPasswordLabel.setString("Password");
    mPasswordLabel.setCharacterSize(16);
    mPasswordLabel.setFillColor(mUsernameLabel.getFillColor());

    // Text Boxes (Dùng lớp TextBox mới)
    sf::Vector2f boxSize(300, 40);
    mUsernameBox.setSize(boxSize);
    mUsernameBox.setPosition({mApp.getWindow().getSize().x / 2.0f, mTitleText.getPosition().y + 80.f});
    mUsernameBox.setFont(mFontRegular);
    mUsernameBox.setPlaceholder("Enter username"); // Placeholder text
    mUsernameBox.setCharacterSize(18);

    mPasswordBox.setSize(boxSize);
    mPasswordBox.setPosition({mApp.getWindow().getSize().x / 2.0f, mUsernameBox.getPosition().y + 90.f});
    mPasswordBox.setFont(mFontRegular);
    mPasswordBox.setPlaceholder("Enter password");
    mPasswordBox.setCharacterSize(18);
    mPasswordBox.setPasswordMode(true); // Bật chế độ mật khẩu (hiển thị *)

    // Đặt vị trí Labels phía trên TextBoxes
    mUsernameLabel.setPosition(mUsernameBox.getPosition().x - boxSize.x / 2.0f + 5.f,
                               mUsernameBox.getPosition().y - boxSize.y / 2.0f - 25.f);
    mPasswordLabel.setPosition(mPasswordBox.getPosition().x - boxSize.x / 2.0f + 5.f,
                               mPasswordBox.getPosition().y - boxSize.y / 2.0f - 25.f);


    // Button (Dùng lớp Button mới)
    mLoginButton.setSize({150, 45});
    mLoginButton.setPosition({mApp.getWindow().getSize().x / 2.0f, mPasswordBox.getPosition().y + 80.f});
    mLoginButton.setText("Login", mFontBold, 20); // Dùng Poppins Bold cho nút
    mLoginButton.setBackgroundColor(sf::Color(0, 120, 215));
    mLoginButton.setTextColor(sf::Color::White);
    // Gán hành động khi nhấn nút (sử dụng lambda function)
    mLoginButton.setOnAction([this]() {
        handleLogin();
    });
}

void LoginState::handleInput(sf::Event& event) {
    // Chuyển sự kiện cho các components
    mUsernameBox.handleEvent(event, mApp.getWindow());
    mPasswordBox.handleEvent(event, mApp.getWindow());
    mLoginButton.handleEvent(event, mApp.getWindow());

    // Có thể thêm xử lý phím Enter để kích hoạt nút Login
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
         handleLogin();
    }
}

void LoginState::update(sf::Time dt) {
    // Cập nhật trạng thái của các components (nếu cần, ví dụ hiệu ứng)
    mUsernameBox.update(dt);
    mPasswordBox.update(dt);
    mLoginButton.update(mApp.getWindow()); // Cho Button kiểm tra hover
}

void LoginState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mBackgroundSprite, states);
    target.draw(mIconSprite, states);
    target.draw(mTitleText, states);

    target.draw(mUsernameLabel, states);
    target.draw(mUsernameBox, states); // TextBox tự vẽ text bên trong

    target.draw(mPasswordLabel, states);
    target.draw(mPasswordBox, states); // TextBox tự vẽ text bên trong

    target.draw(mLoginButton, states); // Button tự vẽ nền và text
}

void LoginState::handleLogin() {
    std::string username = mUsernameBox.getText();
    std::string password = mPasswordBox.getText();

    std::cout << "Attempting login..." << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Password: " << password << std::endl; // Thực tế không nên in password

    // --- !! Logic kiểm tra đăng nhập thực tế sẽ ở đây !! ---
    // Ví dụ: gọi hàm từ AccountManager
    // AccountManager accountMgr; // Nên được truyền vào hoặc quản lý bởi App
    // if (accountMgr.validateAdmin(username, password)) {
    //     std::cout << "Login successful!" << std::endl;
    //     // mApp.changeState(std::make_unique<DashboardState>(mApp)); // Chuyển state
    // } else {
    //     std::cout << "Login failed!" << std::endl;
    //     // Hiển thị thông báo lỗi trên UI
    // }
}

} // namespace UI
