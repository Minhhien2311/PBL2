#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "C:/PBL2/include/core/AccountManager.h"
#include "C:/PBL2/include/main_state.h"
#include "C:/PBL2/include/core/FlightManager.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

FlightManager flight_manager("C:/PBL2/data/flights.txt","C:/PBL2/data/flight_instances.txt");

// --- Giao diện cho màn hình "Thêm tuyến bay" ---
class AddFlightScreen {
public:
    // Trạng thái (dữ liệu) của màn hình này
    std::string flight_number;
    std::string airline;
    std::string departure_airport;
    std::string arrival_airport;
    std::string thong_bao = "";

    // Component chính chứa các thành phần con
    Component container;

    AddFlightScreen() {
        // Tạo các component con
        Component input_flight_number = Input(&flight_number, "Nhập mã chuyến bay (VD:VN123)");
        Component input_airline = Input(&airline, "Nhập hãng (VD: Vietnam Airline)");
        Component input_departure_airport = Input(&departure_airport, "Nhập nơi đi (VD: HAN)");
        Component input_arrival_airport = Input(&arrival_airport, "Nhập nơi đến (VD: DAN)");

        auto them_button = Button("Thêm", [&] {
            // Logic khi nhấn nút "Thêm"
            bool new_flight = flight_manager.createNewFlight(flight_number,airline,departure_airport,arrival_airport);

            if (new_flight){
                thong_bao = "Thêm thành công tuyến bay " + flight_number + "|" + departure_airport + " - " + arrival_airport +"!";
                // Xóa dữ liệu cũ
                flight_number = "";
                airline = "";
                departure_airport = "";
                arrival_airport = "";
            }
            else
                thong_bao = "Không thể thêm tuyến bay trống hoặc đã tồn tại!";
            
        });

        container = Container::Vertical({
            input_flight_number,
            input_airline,
            input_departure_airport,
            input_arrival_airport,
            them_button
        });
    }

    // Hàm vẽ giao diện cho màn hình này
    Element Render() {
        return vbox({
            // Bảng nhập liệu
            gridbox({
                {text(" Mã chuyến bay ") | bold,separator(), text(" Hãng bay ") | bold,separator(), text(" Sân bay đi ") | bold,separator(), text(" Sân bay đến ") | bold},
                {
                    // Lấy component con từ container để render
                    container->ChildAt(0)->Render(), // input_flight_number
                    separator(),
                    container->ChildAt(1)->Render(), // input_airline
                    separator(),
                    container->ChildAt(2)->Render(), // input_departure_airport
                    separator(),
                    container->ChildAt(3)->Render(), // input_arrival_airport
                },
            }) | border | flex,
            separator(),
            // Nút "Thêm"
            container->ChildAt(4)->Render() | center, // them_button
            separator(),
            // Thông báo
            text(thong_bao) | color(Color::Green) | center
        });
    }
};

// --- Giao diện cho màn hình "Thêm chuyến bay" ---
class AddFlightInstacneScreen {
public:
    // Trạng thái (dữ liệu) của màn hình này
    std::string flight_number;
    std::string airline;
    std::string departure_airport;
    std::string arrival_airport;
    std::string thong_bao = "";

    // Component chính chứa các thành phần con
    Component container;

    AddFlightScreen() {
        // Tạo các component con
        Component input_flight_number = Input(&flight_number, "Nhập mã chuyến bay (VD:VN123)");
        Component input_airline = Input(&airline, "Nhập hãng (VD: Vietnam Airline)");
        Component input_departure_airport = Input(&departure_airport, "Nhập nơi đi (VD: HAN)");
        Component input_arrival_airport = Input(&arrival_airport, "Nhập nơi đến (VD: DAN)");

        auto them_button = Button("Thêm", [&] {
            // Logic khi nhấn nút "Thêm"
            bool new_flight = flight_manager.createNewFlight(flight_number,airline,departure_airport,arrival_airport);

            if (new_flight){
                thong_bao = "Thêm thành công tuyến bay " + flight_number + "|" + departure_airport + " - " + arrival_airport +"!";
                // Xóa dữ liệu cũ
                flight_number = "";
                airline = "";
                departure_airport = "";
                arrival_airport = "";
            }
            else
                thong_bao = "Không thể thêm tuyến bay trống hoặc đã tồn tại!";
            
        });

        container = Container::Vertical({
            input_flight_number,
            input_airline,
            input_departure_airport,
            input_arrival_airport,
            them_button
        });
    }

    // Hàm vẽ giao diện cho màn hình này
    Element Render() {
        return vbox({
            // Bảng nhập liệu
            gridbox({
                {text(" Mã chuyến bay ") | bold,separator(), text(" Hãng bay ") | bold,separator(), text(" Sân bay đi ") | bold,separator(), text(" Sân bay đến ") | bold},
                {
                    // Lấy component con từ container để render
                    container->ChildAt(0)->Render(), // input_flight_number
                    separator(),
                    container->ChildAt(1)->Render(), // input_airline
                    separator(),
                    container->ChildAt(2)->Render(), // input_departure_airport
                    separator(),
                    container->ChildAt(3)->Render(), // input_arrival_airport
                },
            }) | border | flex,
            separator(),
            // Nút "Thêm"
            container->ChildAt(4)->Render() | center, // them_button
            separator(),
            // Thông báo
            text(thong_bao) | color(Color::Green) | center
        });
    }
};


void ShowAdminMenu(){
    auto screen = ScreenInteractive::TerminalOutput();
    AccountManager account_manager("C:/PBL2/data/admins.txt","C:/PBL2:/data/agents.txt");

    // --- BIẾN TRẠNG THÁI (STATE) ---
    int selected_tab = 0; // 0: Thêm chuyến bay, 1: Xóa chuyến bay, ...
    std::string user_name = " 👤" + account_manager.getCurrentUser()->getFullName() + " - Admin ";
    // int total_flights = 6;

    // Danh sách các tab trong menu
    std::vector<std::string> menu_tabs = {
        "Danh sách chuyến bay",
        "Danh sách đại lý",
        "Thêm tuyến bay",
        "Xóa tuyến bay",
        "Thêm Agent",
    };

    // --- CÁC COMPONENT TƯƠNG TÁC ---
    // Tạo component cho màn hình "Thêm chuyến bay"
    AddFlightScreen add_flight_screen;
    // (Sau này sẽ tạo các screen khác tương tự)

    // Component Menu để quản lý các nút chức năng
    auto menu = Menu(&menu_tabs, &selected_tab, MenuOption::Vertical());

    // Component nút Đăng xuất
    auto logout_button = Button("Đăng xuất", [&]{
        current_screen = ApplicationScreen::Login;
    });

    auto exit_button = Button("Thoát", screen.ExitLoopClosure());

    // Container chính gom nhóm tất cả các phần có thể tương tác
    // Điều này quan trọng để điều hướng bằng phím Tab
    auto main_container = Container::Vertical({
        menu,
        add_flight_screen.container, // Thêm container của màn hình con vào
        logout_button,
        exit_button,
    });


    // --- HỌA SĨ (RENDERER) ---
    // Đây là phần quan trọng nhất, nó sẽ vẽ toàn bộ giao diện dựa trên
    // các biến trạng thái ở trên.
    auto main_renderer = Renderer(main_container, [&] {
        // 1. Menu bên trái
        auto left_pane = vbox({
            menu->Render(),
            separatorEmpty() | flex, // Dùng flex để đẩy nút Đăng xuất xuống dưới
            exit_button->Render(),
            logout_button->Render()
        }) | size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 20) | border; // Cố định chiều rộng của menu

        // 2. Nội dung bên phải
        Element right_content;
        // Dựa vào `selected_tab` để quyết định vẽ gì
        switch (selected_tab) {
            case 0:
                right_content = text("Giao diện Danh sách chuyén bay") | center;
                break;
            case 1:
                right_content = text("Giao diện Danh sách đại lý") | center;
                break;
            case 2:
                right_content = add_flight_screen.Render();
                break;
            case 3:
                right_content = text("Giao diện Xóa chuyến bay") | center;
                break;
            case 4:
                right_content = text("Giao diện thêm đại lý") | center;
                break;
        }

        auto right_pane = vbox({
            right_content | flex, // Dùng flex để đẩy footer xuống dưới
            separator(),
            // text("Số chuyến bay hiện có : " + std::to_string(total_flights))
        }) | border;

        // 3. Ghép 2 cột lại với nhau
        auto layout = hbox({
            left_pane,
            right_pane | flex// Dùng flex để cột phải lấp đầy không gian còn lại
        });

        // 4. Đặt toàn bộ layout vào một cửa sổ lớn
        return window(text(user_name), layout) | size(WIDTH, EQUAL, 130);
    });

    screen.Loop(main_renderer);

}