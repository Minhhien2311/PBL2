#include "C:/PBL2/include/interface/Login_Interface.h"
#include "C:/PBL2/include/main_state.h" // Để thay đổi trạng thái
#include "C:/PBL2/include/entities/Account.h"
#include "C:/PBL2/include/entities/Flight.h"
#include "C:/PBL2/include/core/AccountManager.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <string>

using namespace ftxui;

void ShowLoginScreen() {
    auto screen = ScreenInteractive::TerminalOutput();
    AccountManager account_manager("C:/PBL2/data/admins.txt","C:/PBL2/data/agents.txt");

    // --- Biến trạng thái cho giao diện ---
    std::string user_name;
    std::string user_password;
    std::string login_message = "";
    int password_mode = 0; // 0 = ẩn (password), 1 = hiện (text)

    // --- Components ---

    // 1. Input cho tên đăng nhập
    Component user_name_input = Input(&user_name, "Nhap ten dang nhap");

    // 2. Hai phiên bản của Input mật khẩu (ẩn và hiện)
    Component password_input_hidden = Input(&user_password, "Nhap mat khau", {.password = true});
    Component password_input_visible = Input(&user_password, "Nhap mat khau", {.password = false});

    // 3. Container::Tab để chuyển đổi giữa hai phiên bản Input trên
    auto password_container = Container::Tab(
        {
            password_input_hidden,  // Trang 0
            password_input_visible, // Trang 1
        },
        &password_mode // Biến điều khiển việc hiển thị trang nào
    );

    // 4. Nút "Hiện/Ẩn" để thay đổi giá trị của password_mode
    auto eye_button = Button("👁 ", [&] {
        password_mode = 1 - password_mode; // Lật giữa 0 và 1
    });

    // 5. Nút đăng nhập
    auto login_button = Button("Dang nhap", [&] {
        bool log = account_manager.login(user_name,user_password);

        if (log) {
            login_message = "Dang nhap thanh cong!";
            if (account_manager.getCurrentUser()->getRole() == Role::Admin) {
                current_screen = ApplicationScreen::AdminMenu;
            } else {
                current_screen = ApplicationScreen::AgentMenu;
            }
            screen.Exit();
        } else {
            login_message = "Ten dang nhap hoac mat khau khong chinh xac!";
        }
    });

    // Container chính chứa tất cả các component có thể tương tác (để điều hướng bằng bàn phím)
    auto main_container = Container::Vertical({
        user_name_input,
        password_container,
        eye_button,
        login_button,
    });
    
    // Renderer để vẽ giao diện
    auto renderer = Renderer(main_container, [&] {
        return vbox({
            text("HE THONG QUAN LY BAN VE MAY BAY") | bold | center,
            separator(),
            window(text("Thong tin dang nhap"),
                // Dùng gridbox để căn chỉnh các dòng cho đẹp
                gridbox({
                    {text(" Ten dang nhap: ") | align_right, user_name_input->Render()},
                    {text(" Mat khau:      ") | align_right, hbox({
                        // Đặt Input và nút Hiện/Ẩn trên cùng một dòng
                        password_container->Render() | flex,
                        separatorEmpty(),
                        eye_button->Render()
                    })},
                }) | size(WIDTH, EQUAL, 50)
            ),
            separator(),
            login_button->Render() | center,
            separator(),
            text(login_message) | color(Color::Red) | center
        }) | border | center;
    });

    screen.Loop(renderer);
}