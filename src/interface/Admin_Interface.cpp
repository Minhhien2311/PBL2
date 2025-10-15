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
    std::string flightID;
    std::string departureIso;
    std::string arrivalIso;
    std::string economyTotal;
    std::string thong_bao = "";

    // Component chính chứa các thành phần con
    Component container;

    AddFlightScreen() {
        // Tạo các component con
        Component input_flightID = Input(&flightID, "Nhập mã chuyến bay (VD:VN123)");
        Component input_departureIso = Input(&departureIso, "Nhập hãng (VD: Vietnam departureIso)");
        Component input_arrivalIso = Input(&arrivalIso, "Nhập nơi đi (VD: HAN)");
        Component input_economyTotal = Input(&economyTotal, "Nhập nơi đến (VD: DAN)");

        auto them_button = Button("Thêm", [&] {
            // Logic khi nhấn nút "Thêm"
            bool new_flight = flight_manager.createNewFlight(flightID,departureIso,arrivalIso,economyTotal);

            if (new_flight){
                thong_bao = "Thêm thành công tuyến bay " + flightID + "|" + arrivalIso + " - " + economyTotal +"!";
                // Xóa dữ liệu cũ
                flightID = "";
                departureIso = "";
                arrivalIso = "";
                economyTotal = "";
            }
            else
                thong_bao = "Không thể thêm tuyến bay trống hoặc đã tồn tại!";
            
        });

        container = Container::Vertical({
            input_flightID,
            input_departureIso,
            input_arrivalIso,
            input_economyTotal,
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
                    container->ChildAt(0)->Render(), // input_flightID
                    separator(),
                    container->ChildAt(1)->Render(), // input_departureIso
                    separator(),
                    container->ChildAt(2)->Render(), // input_arrivalIso
                    separator(),
                    container->ChildAt(3)->Render(), // input_economyTotal
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
class AddFlightInstanceScreen {
public:
    // Trạng thái (dữ liệu) của màn hình này
    std::string flightID;
    std::string departureIso;
    std::string arrivalIso;
    std::string economyTotal;
    std::string businessTotal;
    std::string fareEconomy;
    std::string fareBusiness;
    std::string thong_bao = "";

    // Component chính chứa các thành phần con
    Component container;

    AddFlightInstanceScreen() {
        // Tạo các component con
        Component input_flightID = Input(&flightID, "Nhập ID tuyến bay (VD: 1)");
        Component input_departureIso = Input(&departureIso, "Nhập giờ khởi hành (YYYY-MM-DDTHH:MM:SSZ)");
        Component input_arrivalIso = Input(&arrivalIso, "Nhập giờ hạ cánh (YYYY-MM-DDTHH:MM:SSZ)");
        Component input_economyTotal = Input(&economyTotal, "Nhập số ghế hạng phổ thông (VD: 120)");
        Component input_businessTotal = Input(&businessTotal, "Nhập số ghế hạng thương gia (VD: 40)");
        Component input_fareEconomy = Input(&fareEconomy, "Nhập giá vé phổ thông (VD: 1500000)");
        Component input_fareBusiness = Input(&fareBusiness, "Nhập giá vé thương gia (VD: 3500000)");

        auto them_button = Button("Thêm", [&] {
            // Logic khi nhấn nút "Thêm"
            bool new_flight_instance = flight_manager.createNewInstance(flightID,
                                                                        departureIso,
                                                                        arrivalIso,
                                                                        std::stoi(economyTotal),
                                                                        std::stoi(businessTotal),
                                                                        std::stoi(fareEconomy),
                                                                        std::stoi(fareBusiness));

            if (new_flight_instance){
                thong_bao = "Thêm thành công chuyến bay " + flightID + " !";
                // Xóa dữ liệu cũ
                flightID = "";
                departureIso = "";
                arrivalIso = "";
                economyTotal = "";
                businessTotal = "";
                fareEconomy = "";
                fareBusiness = "";
            }
            else
                thong_bao = "Không thể thêm chuyến bay trống hoặc đã tồn tại!";
            
        });

        container = Container::Vertical({
            input_flightID,
            input_departureIso,
            input_arrivalIso,
            input_economyTotal,
            input_businessTotal,
            input_fareEconomy,
            input_fareBusiness,
            them_button
        });
    }

    // Hàm vẽ giao diện cho màn hình này
    Element Render() {
        return vbox({
            // Bảng nhập liệu
            gridbox({
                {text(" ID tuyến bay ") | bold,separator(), text(" Giờ khởi hành ") | bold,separator(), text(" Giờ hạ cánh ") | bold,separator(), 
                text(" Số ghế phổ thông ") | bold, separator(),text(" Số ghế thương gia ") | bold,separator(), text(" Giá vé phổ thông ") | bold, separator(),
                text(" Giá vé thương gia ") | bold},
                {
                    // Lấy component con từ container để render
                    container->ChildAt(0)->Render(), // input_flightID
                    separator(),
                    container->ChildAt(1)->Render(), // input_departureIso
                    separator(),
                    container->ChildAt(2)->Render(), // input_arrivalIso
                    separator(),
                    container->ChildAt(3)->Render(), // input_economyTotal
                    separator(),
                    container->ChildAt(4)->Render(), // input_businessTotal
                    separator(),
                    container->ChildAt(5)->Render(), // input_fareEconomy
                    separator(),
                    container->ChildAt(6)->Render(), // input_faceBusiness
                },
            }) | border | flex,
            separator(),
            // Nút "Thêm"
            container->ChildAt(7)->Render() | center, // them_button
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
        "Thêm chuyến bay"
        "Xóa tuyến bay",
        "Xóa chuyến bay"
        "Thêm đại lý",
    };

    // --- CÁC COMPONENT TƯƠNG TÁC ---
    // Tạo component các cho màn hình
    AddFlightScreen add_flight_screen;
    AddFlightInstanceScreen add_flight_instance_screen;
    // (Sau này sẽ tạo các screen khác tương tự)

    // Component Menu để quản lý các nút chức năng
    auto menu = Menu(&menu_tabs, &selected_tab, MenuOption::Vertical());

    // Component nút Đăng xuất
    auto logout_button = Button("Đăng xuất", [&]{
        current_screen = ApplicationScreen::Login;
    });

    //Component nút Thoát
    auto exit_button = Button("Thoát", screen.ExitLoopClosure());

    // Container chính gom nhóm tất cả các phần có thể tương tác
    // Điều này quan trọng để điều hướng bằng phím Tab
    auto main_container = Container::Vertical({
        menu,
        add_flight_screen.container, // Thêm container của màn hình con vào
        add_flight_instance_screen.container,
        exit_button,
        logout_button,
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
                right_content = text("Giao diện Danh sách chuyến bay") | center;
                break;
            case 1:
                right_content = text("Giao diện Danh sách đại lý") | center;
                break;
            case 2:
                right_content = add_flight_screen.Render();
                break;
            case 3:
                right_content = add_flight_instance_screen.Render();
                break;
            case 4:
                right_content = text("Giao diện xóa tuyến bay") | center;
                break;
            case 5:
                right_content = text("Giao diện xóa chuyến bay") | center;
                break;
            case 6:
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