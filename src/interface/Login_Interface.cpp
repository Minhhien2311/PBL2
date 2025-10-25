// #include "C:/PBL2/include/interface/Login_Interface.h"
// #include "C:/PBL2/include/main_state.h" // Để thay đổi trạng thái
// #include "C:/PBL2/include/entities/Account.h"
// #include "C:/PBL2/include/entities/Flight.h"
// #include "C:/PBL2/include/core/AccountManager.h"
// #include "ftxui/component/component.hpp"
// #include "ftxui/component/screen_interactive.hpp"
// #include <string>

// using namespace ftxui;

// void ShowLoginScreen(AccountManager& account_manager) {
//     auto screen = ScreenInteractive::TerminalOutput();

//     // --- Biến trạng thái cho giao diện ---
//     std::string user_name;
//     std::string user_password;
//     std::string login_message = "";
//     int password_mode = 0; // 0 = ẩn (password), 1 = hiện (text)

//     // --- Components ---

//     // 1. Input cho tên đăng nhập
//     Component user_name_input = Input(&user_name, "Nhập tên đăng nhập");

//     // 2. Hai phiên bản của Input mật khẩu (ẩn và hiện)
//     Component password_input_hidden = Input(&user_password, "Nhập mật khẩu", {.password = true});
//     Component password_input_visible = Input(&user_password, "Nhập mật khẩu", {.password = false});

//     // 3. Container::Tab để chuyển đổi giữa hai phiên bản Input trên
//     auto password_container = Container::Tab(
//         {
//             password_input_hidden,  // Trang 0
//             password_input_visible, // Trang 1
//         },
//         &password_mode // Biến điều khiển việc hiển thị trang nào
//     );

//     // 4. Nút "Hiện/Ẩn" để thay đổi giá trị của password_mode
//     auto eye_button = Button("👁 ", [&] {
//         password_mode = 1 - password_mode; // Lật giữa 0 và 1
//     });

//     // 5. Nút đăng nhập
//     auto login_button = Button("Đăng nhập", [&] {
//         bool log = account_manager.login(user_name,user_password);

//         if (log) {
//             login_message = "Đăng nhập thành công!";
//             if (account_manager.getCurrentUser()->getRole() == Role::Admin) {
//                 current_screen = ApplicationScreen::AdminMenu;
//             } else {
//                 current_screen = ApplicationScreen::AgentMenu;
//             }
//             screen.Exit();
//         } else {
//             login_message = "Tên đăng nhập hoặc mật khẩu không chính xác!";
//         }
//     });

//     // Container chính chứa tất cả các component có thể tương tác (để điều hướng bằng bàn phím)
//     auto main_container = Container::Vertical({
//         user_name_input,
//         password_container,
//         eye_button,
//         login_button,
//     });
    
//     // Renderer để vẽ giao diện
//     auto renderer = Renderer(main_container, [&] {
//         return vbox({
//             text("HỆ THỐNG QUẢN LÝ BÁN VÉ MÁY BAY") | bold | center,
//             separator(),
//             window(text("Thông tin đăng nhập"),
//                 // Dùng gridbox để căn chỉnh các dòng cho đẹp
//                 gridbox({
//                     {text(" Tên đăng nhập: ") | align_right, user_name_input->Render() | size(WIDTH, EQUAL, 22)},
//                     {text(" Mật khẩu:      ") | align_right, hbox({
//                         // Đặt Input và nút Hiện/Ẩn trên cùng một dòng
//                         hbox(password_container->Render()) | size(WIDTH, EQUAL, 20), // |flex
//                         separatorEmpty(),
//                         eye_button->Render()
//                     })},
//                 })
//             ),
//             separator(),
//             login_button->Render() | center,
//             separator(),
//             text(login_message) | center | (login_message.find("thành công") != std::string::npos ? color(Color::Green) : color(Color::Red))
//         }) | border | center;
//     });

//     screen.Loop(renderer);
// }