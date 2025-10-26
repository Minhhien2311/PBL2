#include <SFML/Graphics.hpp>
#include <iostream>
#include <string> // Để lưu trữ username/password

int main()
{
    // --- 1. Khởi tạo Cửa sổ ---
    const unsigned int WINDOW_WIDTH = 800;  // Chiều rộng cửa sổ (thay đổi nếu cần)
    const unsigned int WINDOW_HEIGHT = 600; // Chiều cao cửa sổ (thay đổi nếu cần)
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Admin Login");
    window.setFramerateLimit(60); // Giới hạn FPS

    // --- 2. Tải Tài Nguyên ---
    // Font chữ
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Mulish-Bold.ttf"))
    { // Đường dẫn tương đối từ thư mục builds
        std::cerr << "Error loading font!" << std::endl;
        return -1; // Thoát nếu không tải được font
    }

    // Texture hình nền
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/icon/welcomestate.png"))
    { // Đường dẫn tương đối
        std::cerr << "Error loading background texture!" << std::endl;
        return -1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    // Scale hình nền để vừa cửa sổ (nếu cần)
    backgroundSprite.setScale(
        (float)WINDOW_WIDTH / backgroundTexture.getSize().x,
        (float)WINDOW_HEIGHT / backgroundTexture.getSize().y);

    // Texture icon admin
    sf::Texture iconTexture;
    if (!iconTexture.loadFromFile("assets/icon/AdminLogin.png"))
    { // Đường dẫn tương đối
        std::cerr << "Error loading icon texture!" << std::endl;
        return -1;
    }
    sf::Sprite iconSprite(iconTexture);
    // Đặt vị trí cho icon (ví dụ: căn giữa phía trên)
    iconSprite.setOrigin(iconTexture.getSize().x / 2.0f, iconTexture.getSize().y / 2.0f); // Đặt gốc ở giữa icon
    iconSprite.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT * 0.2f);                    // Ước lượng vị trí

    // --- 3. Tạo Các Thành Phần Giao Diện ---

    // Tiêu đề "Admin Login"
    sf::Text titleText("Admin Login", font, 30); // Text, Font, Cỡ chữ
    titleText.setFillColor(sf::Color::White);
    // Căn giữa tiêu đề bên dưới icon
    sf::FloatRect textRect = titleText.getLocalBounds();
    titleText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    titleText.setPosition(iconSprite.getPosition().x, iconSprite.getPosition().y + iconTexture.getSize().y / 1.5f + 20.f);

    // Ô nhập Username
    sf::RectangleShape usernameBox(sf::Vector2f(300, 40));  // Kích thước (width, height)
    usernameBox.setFillColor(sf::Color(255, 255, 255, 50)); // Màu trắng hơi trong suốt
    usernameBox.setOutlineThickness(1);
    usernameBox.setOutlineColor(sf::Color::White);
    usernameBox.setOrigin(usernameBox.getSize().x / 2.0f, usernameBox.getSize().y / 2.0f);
    usernameBox.setPosition(WINDOW_WIDTH / 2.0f, titleText.getPosition().y + 80.f); // Vị trí dưới tiêu đề

    sf::Text usernameLabel("Username", font, 16);
    usernameLabel.setFillColor(sf::Color(255, 255, 255, 150)); // Màu trắng xám
    usernameLabel.setPosition(usernameBox.getPosition().x - usernameBox.getSize().x / 2.0f + 10.f,
                              usernameBox.getPosition().y - usernameBox.getSize().y / 2.0f - 25.f); // Phía trên ô nhập

    sf::Text usernameInputText("", font, 18); // Text nhập vào
    usernameInputText.setFillColor(sf::Color::White);
    usernameInputText.setPosition(usernameBox.getPosition().x - usernameBox.getSize().x / 2.0f + 10.f,
                                  usernameBox.getPosition().y - usernameInputText.getCharacterSize() / 1.5f);

    // Ô nhập Password (tương tự Username)
    sf::RectangleShape passwordBox(usernameBox.getSize()); // Cùng kích thước
    passwordBox.setFillColor(usernameBox.getFillColor());
    passwordBox.setOutlineThickness(usernameBox.getOutlineThickness());
    passwordBox.setOutlineColor(usernameBox.getOutlineColor());
    passwordBox.setOrigin(passwordBox.getSize().x / 2.0f, passwordBox.getSize().y / 2.0f);
    passwordBox.setPosition(WINDOW_WIDTH / 2.0f, usernameBox.getPosition().y + 90.f); // Dưới ô username

    sf::Text passwordLabel("Password", font, 16);
    passwordLabel.setFillColor(usernameLabel.getFillColor());
    passwordLabel.setPosition(passwordBox.getPosition().x - passwordBox.getSize().x / 2.0f + 10.f,
                              passwordBox.getPosition().y - passwordBox.getSize().y / 2.0f - 25.f);

    sf::Text passwordInputText("", font, 18);
    passwordInputText.setFillColor(sf::Color::White);
    passwordInputText.setPosition(passwordBox.getPosition().x - passwordBox.getSize().x / 2.0f + 10.f,
                                  passwordBox.getPosition().y - passwordInputText.getCharacterSize() / 1.5f);

    // Nút Login
    sf::RectangleShape loginButton(sf::Vector2f(150, 45));
    loginButton.setFillColor(sf::Color(0, 120, 215)); // Màu xanh dương
    loginButton.setOrigin(loginButton.getSize().x / 2.0f, loginButton.getSize().y / 2.0f);
    loginButton.setPosition(WINDOW_WIDTH / 2.0f, passwordBox.getPosition().y + 80.f); // Dưới ô password

    sf::Text loginButtonText("Login", font, 20);
    loginButtonText.setFillColor(sf::Color::White);
    textRect = loginButtonText.getLocalBounds();
    loginButtonText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    loginButtonText.setPosition(loginButton.getPosition());

    // --- 4. Biến cho Logic Nhập Liệu ---
    std::string usernameString;
    std::string passwordString;
    bool usernameSelected = false;
    bool passwordSelected = false;
    std::string displayedPassword; // Để hiển thị dấu *

    // --- 5. Vòng lặp chính (Game Loop) ---
    while (window.isOpen())
    {
        // --- Xử lý sự kiện ---
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // Chọn ô nhập liệu khi click chuột
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

                    // Kiểm tra click vào ô Username
                    if (usernameBox.getGlobalBounds().contains(mousePos))
                    {
                        usernameSelected = true;
                        passwordSelected = false;
                        usernameBox.setOutlineColor(sf::Color::Cyan); // Đổi màu viền khi chọn
                        passwordBox.setOutlineColor(sf::Color::White);
                    }
                    // Kiểm tra click vào ô Password
                    else if (passwordBox.getGlobalBounds().contains(mousePos))
                    {
                        usernameSelected = false;
                        passwordSelected = true;
                        usernameBox.setOutlineColor(sf::Color::White);
                        passwordBox.setOutlineColor(sf::Color::Cyan); // Đổi màu viền khi chọn
                    }
                    // Kiểm tra click vào nút Login
                    else if (loginButton.getGlobalBounds().contains(mousePos))
                    {
                        std::cout << "Login Clicked!" << std::endl;
                        std::cout << "Username: " << usernameString << std::endl;
                        std::cout << "Password: " << passwordString << std::endl;
                        // !! Ở đây bạn sẽ gọi hàm kiểm tra đăng nhập !!
                        // if (checkLogin(usernameString, passwordString)) { ... }
                    }
                    else
                    {
                        usernameSelected = false;
                        passwordSelected = false;
                        usernameBox.setOutlineColor(sf::Color::White);
                        passwordBox.setOutlineColor(sf::Color::White);
                    }
                }
            }

            // Xử lý nhập văn bản
            if (event.type == sf::Event::TextEntered)
            {
                if (usernameSelected)
                {
                    // Xử lý Backspace (ASCII = 8)
                    if (event.text.unicode == 8 && !usernameString.empty())
                    {
                        usernameString.pop_back();
                    }
                    // Chỉ nhận ký tự ASCII in được (tránh các phím control)
                    else if (event.text.unicode >= 32 && event.text.unicode < 128)
                    {
                        usernameString += static_cast<char>(event.text.unicode);
                    }
                    usernameInputText.setString(usernameString);
                }
                else if (passwordSelected)
                {
                    if (event.text.unicode == 8 && !passwordString.empty())
                    {
                        passwordString.pop_back();
                    }
                    else if (event.text.unicode >= 32 && event.text.unicode < 128)
                    {
                        passwordString += static_cast<char>(event.text.unicode);
                    }
                    // Cập nhật chuỗi hiển thị password thành dấu *
                    displayedPassword = "";
                    for (size_t i = 0; i < passwordString.length(); ++i)
                    {
                        displayedPassword += '*';
                    }
                    passwordInputText.setString(displayedPassword);
                }
            }
        }

        // --- Cập nhật (Nếu có animation, hiệu ứng...) ---
        // Ví dụ: Hiệu ứng hover cho nút (thay đổi màu nhẹ)
        sf::Vector2f currentMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (loginButton.getGlobalBounds().contains(currentMousePos))
        {
            loginButton.setFillColor(sf::Color(0, 100, 180)); // Màu đậm hơn khi hover
        }
        else
        {
            loginButton.setFillColor(sf::Color(0, 120, 215)); // Màu gốc
        }

        // --- Vẽ lên cửa sổ ---
        window.clear(); // Xóa frame cũ

        // Vẽ các đối tượng (thứ tự vẽ quan trọng)
        window.draw(backgroundSprite);
        window.draw(iconSprite);
        window.draw(titleText);

        window.draw(usernameLabel);
        window.draw(usernameBox);
        window.draw(usernameInputText); // Vẽ text nhập lên trên box

        window.draw(passwordLabel);
        window.draw(passwordBox);
        window.draw(passwordInputText); // Vẽ text nhập lên trên box

        window.draw(loginButton);
        window.draw(loginButtonText); // Vẽ text nút lên trên nền nút

        window.display(); // Hiển thị frame mới
    }

    return 0;
}