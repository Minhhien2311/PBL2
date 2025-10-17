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
        Component input_flightID = Input(&flightID, "(VD:VN123)");
        Component input_departureIso = Input(&departureIso, "(VD: Vietnam Airline)");
        Component input_arrivalIso = Input(&arrivalIso, "(VD: HAN)");
        Component input_economyTotal = Input(&economyTotal, "(VD: DAN)");

        auto them_button = Button("Thêm ", [&] {
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
                {text(" Mã chuyến bay ") | bold |center,separator(), text(" Hãng bay ") | bold |center,separator(), text(" Sân bay đi ") | bold |center,separator(), text(" Sân bay đến ") | bold |center},
                {
                    // Lấy component con từ container để render
                    container->ChildAt(0)->Render() |center ,// input_flightID
                    separator(),
                    container->ChildAt(1)->Render() |center, // input_departureIso
                    separator(),
                    container->ChildAt(2)->Render() |center, // input_arrivalIso
                    separator(),
                    container->ChildAt(3)->Render() |center, // input_economyTotal
                },
            }) | border | size(WIDTH,EQUAL,66) | center,
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
        Component input_flightID = Input(&flightID, "(VD: 1)");
        Component input_departureIso = Input(&departureIso, "(YYYY-MM-DDTHH:MM:SSZ)");
        Component input_arrivalIso = Input(&arrivalIso, "(YYYY-MM-DDTHH:MM:SSZ)");
        Component input_economyTotal = Input(&economyTotal, "(VD: 120)");
        Component input_businessTotal = Input(&businessTotal, "(VD: 40)");
        Component input_fareEconomy = Input(&fareEconomy, "(VD: 1500000)");
        Component input_fareBusiness = Input(&fareBusiness, "(VD: 3500000)");

        auto them_button = Button("Thêm ", [&] {
            // Logic khi nhấn nút "Thêm"
            if (economyTotal.empty() || businessTotal.empty() || fareEconomy.empty() || fareBusiness.empty())
                thong_bao = "Không được để dữ liệu trống!";
            else{
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
            }
            
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
                {text(" ID tuyến bay ") | bold | center,separator(), text(" Giờ khởi hành ") | bold | center,separator(), text(" Giờ hạ cánh ") | bold | center,separator(), 
                text(" Số ghế phổ thông ") | bold | center, separator(),text(" Số ghế thương gia ") | bold | center,separator(), text(" Giá vé phổ thông ") | bold | center, separator(),
                text(" Giá vé thương gia ") | bold | center},
                {
                    // Lấy component con từ container để render
                    container->ChildAt(0)->Render() | center, // input_flightID
                    separator(),
                    container->ChildAt(1)->Render() | center, // input_departureIso
                    separator(),
                    container->ChildAt(2)->Render() | center, // input_arrivalIso
                    separator(),
                    container->ChildAt(3)->Render() | center, // input_economyTotal
                    separator(),
                    container->ChildAt(4)->Render() | center, // input_businessTotal
                    separator(),
                    container->ChildAt(5)->Render() | center, // input_fareEconomy
                    separator(),
                    container->ChildAt(6)->Render() | center, // input_faceBusiness
                },
            }) | border | size(WIDTH,EQUAL,140) | center, // flex
            separatorEmpty(),
            // Nút "Thêm"
            container->ChildAt(7)->Render() | center, // them_button
            separator(),
            // Thông báo
            text(thong_bao) | color(Color::Green) | center 
        });
    }
};


void ShowAdminMenu(AccountManager& account_manager){
    auto screen = ScreenInteractive::TerminalOutput();

    // --- BIẾN TRẠNG THÁI (STATE) ---
    int selected_tab = 0;
    std::string user_name = " 👤" + account_manager.getCurrentUser()->getFullName() + " - Admin ";
    
    std::vector<std::string> menu_tabs = {
        "Danh sách chuyến bay",
        "Danh sách đại lý",
        "Thêm tuyến bay",
        "Thêm chuyến bay",
        "Xóa tuyến bay",
        "Xóa chuyến bay",
        "Thêm đại lý",
    };

    // --- CÁC COMPONENT TƯƠNG TÁC ---
    AddFlightScreen add_flight_screen;
    AddFlightInstanceScreen add_flight_instance_screen;

    // Component Menu, nó sẽ thay đổi giá trị của `selected_tab`
    auto menu = Menu(&menu_tabs, &selected_tab, MenuOption::Vertical());

    // Container::Tab sẽ quản lý việc component con nào đang "active".
    // Nó cũng được điều khiển bởi cùng một biến `selected_tab`.
    auto tab_container = Container::Tab(
        {
            Container::Vertical({}), // Tab 0: Danh sách chuyến bay
            Container::Vertical({}), // Tab 1: Danh sách đại lý
            add_flight_screen.container,          // Tab 2: Thêm tuyến bay
            add_flight_instance_screen.container, // Tab 3: Thêm chuyến bay
            Container::Vertical({}), // Tab 4: Xóa tuyến bay
            Container::Vertical({}), // Tab 5: Xóa chuyến bay
            Container::Vertical({}), // Tab 6: Thêm đại lý
        },
        &selected_tab // Biến này điều khiển cả Menu và Tab container
    );

    auto logout_button = Button("Đăng xuất", [&]{
        account_manager.logout();
        current_screen = ApplicationScreen::Login;
        screen.Exit();
    });

    // Container chính chứa menu và tab_container để chúng có thể
    // phối hợp với nhau.
    auto main_container = Container::Horizontal({
        menu,
        tab_container,
        logout_button,
    });
    // Thêm nút logout vào một container riêng để nó tách biệt
    // khỏi luồng điều hướng Tab chính giữa menu và nội dung.

    // --- HỌA SĨ (RENDERER) ---
    // Phần Renderer vẫn giữ nguyên logic hiển thị dựa trên `selected_tab`.
    auto main_renderer = Renderer(main_container, [&] {
        // 1. Menu bên trái
        auto left_pane = vbox({
            menu->Render(),
            separatorEmpty() | flex,
            logout_button->Render()
        }) | size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 40) | border;

        // 2. Nội dung bên phải
        Element right_content;
        switch (selected_tab) {
            case 0: right_content = text("Giao diện Danh sách chuyến bay") | center; break;
            case 1: right_content = text("Giao diện Danh sách đại lý") | center; break;
            case 2: right_content = add_flight_screen.Render(); break;
            case 3: right_content = add_flight_instance_screen.Render(); break;
            case 4: right_content = text("Giao diện xóa tuyến bay") | center; break;
            case 5: right_content = text("Giao diện xóa chuyến bay") | center; break;
            case 6: right_content = text("Giao diện thêm đại lý") | center; break;
        }

        auto right_pane = vbox({
            vbox(right_content) | flex, // Dùng vbox() để cố định layout
            separator(),
        }) | border;

        // 3. Ghép 2 cột lại với nhau
        auto layout = hbox({
            left_pane,
            right_pane | flex,
        });

        // 4. Đặt toàn bộ layout vào một cửa sổ lớn
        return window(text(user_name), layout);
    });

    screen.Loop(main_renderer);
}