#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include "C:/PBL2/include/core/AccountManager.h"
#include "C:/PBL2/include/main_state.h"
#include "C:/PBL2/include/core/BookingManager.h"
#include "C:/PBL2/include/core/FlightManager.h"
#include "C:/PBL2/include/entities/Booking.h"
#include "C:/PBL2/include/DSA/DynamicArray.h" 
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

// ======================================
// ========= BOOKING INTERFACE ===========
// ======================================

// ------ I. Danh sách tuyến bay -----------
class BookingList {
public:
    int scroll_position = 0;
    int visible_rows = 25;
    std::vector<std::vector<std::string>> booking_data;
    BookingManager& booking_manager;
    Component container;       // Container chính
    Component table_component; // Component chứa bảng cuộn được
    Component reload_button;   // Nút làm mới

    BookingList(BookingManager& manager) : booking_manager(manager) {
        reload_button = Button("     [ Làm mới danh sách ]", [&] {
            this->LoadData();
            this->scroll_position = 0; 
        });

        auto table_renderer = Renderer([&] {
            Elements rows;
            int total = booking_data.size();
            int end_index = std::min(scroll_position + visible_rows, total);

            // --- Tiêu đề bảng ---
            auto header = hbox({
                text("ID Đặt vé") | bold | center | size(WIDTH, EQUAL, 15),
                separator(),
                text("ID chuyến bay") | bold | center | size(WIDTH, EQUAL, 15),
                separator(),
                text("ID khách hàng") | bold | center | size(WIDTH, EQUAL, 20),
                separator(),
                text("Thời gian đặt vé") | bold | center | size(WIDTH, EQUAL, 30),
                separator(),
                text("Hạng vé") | bold | center | size(WIDTH, EQUAL, 11),
                separator(),
                text("Giá vé") | bold | center | size(WIDTH, EQUAL, 13),
                separator(),
                text("Trạng thái") | bold | center | size(WIDTH, EQUAL, 11),
            });
            rows.push_back(header);
            rows.push_back(separator());

            // --- Dòng dữ liệu ---
            for (int i = scroll_position; i < end_index; i++) {
                const auto& f = booking_data[i];
                rows.push_back(hbox({
                    text(f[0]) | center | size(WIDTH, EQUAL, 15),
                    text(f[1]) | center | size(WIDTH, EQUAL, 14),
                    text(f[2]) | center | size(WIDTH, EQUAL, 27),
                    text(f[3]) | center | size(WIDTH, EQUAL, 26),
                    text(f[4]) | center | size(WIDTH, EQUAL, 13),
                    text(f[5]) | center | size(WIDTH, EQUAL, 15),
                    text(f[6]) | center | size(WIDTH, EQUAL, 10),
                }));
            }

            // --- Thanh trạng thái ---
            rows.push_back(separator());
            rows.push_back(text("Dùng chuột để cuộn — Tổng số khách hàng: " + std::to_string(total)) | dim | center);

            return vbox(std::move(rows)) | border | size(HEIGHT, LESS_THAN, 35);
        });

        table_component = CatchEvent(table_renderer, [&](Event event) {
            int total = booking_data.size();
            if (event.is_mouse()) {
                if (event.mouse().button == Mouse::WheelDown) {
                    if (scroll_position + visible_rows < total) scroll_position++;
                    return true;
                }
                if (event.mouse().button == Mouse::WheelUp) {
                    if (scroll_position > 0) scroll_position--;
                    return true;
                }
            }
            return false;
        });

        container = Container::Vertical({
            table_component, // Component bảng
            reload_button  | size(WIDTH, EQUAL, 35) | center // Component nút
        });
        
        LoadData(); 
    }

    Element Render() { 
        return vbox({
            text("DANH SÁCH KHÁCH HÀNG") | bold | center,
            table_component->Render() | center,
            separator(),
            reload_button->Render()
        });
    }

private:
    void LoadData() {
        booking_data.clear();
        const auto& booking = booking_manager.getAllBookings();
        for (int i = 0; i < booking.size(); i++) {
            Booking* b = booking[i];
            if (!b) continue;
            std::stringstream ss(b->toRecordLine());
            std::string col;
            std::vector<std::string> cols;
            while (std::getline(ss, col, '|'))
                cols.push_back(col);
            if (cols.size() < 7)
                cols.resize(7, "");
            booking_data.push_back(cols);
        }
    }
};

class AddBooking{
    public:
        // std::string bookingId;         // Khóa nội bộ duy nhất
        // std::string flightInstanceId;  // Chuyến bay cụ thể
        std::string passengerId;
        // std::string bookingDate;       // Ngày giờ tạo
        BookingClass bookingClass;
        // int baseFare;       
        std::string thong_bao = "";

        BookingManager& booking_manager;
        FlightManager& flight_manager;
        FlightInstance* list_instance;

        //phục vụ tìm kiếm
        std::string departure_arrival;
        std::string departureTimetoFind;
        std::string instanceIdtoFind;
        std::string FaretoFind;

        Component container;

        AddBooking(BookingManager& booking_manager, FlightManager& flight_manager) : booking_manager(booking_manager), flight_manager(flight_manager){
            Component input_flightID = Input(&instanceIdtoFind, "(VD: FI-001)");
            Component input_passengerID = Input(&passengerId, "(VD: PI-001)");

            //Các nút bấm
            auto tim_kiem_button = Button("Tìm kiếm", [&]{
                if(departure_arrival.empty() && departureTimetoFind.empty() && FaretoFind.empty())
                    list_instance = flight_manager.findInstanceById(instanceIdtoFind);
            });
            auto pho_thong_button = Button("Phổ thông", [&]{
                bookingClass = BookingClass::Economy;
            });
            auto thuong_gia_button = Button("Thương gia", [&]{
                bookingClass = BookingClass::Business;
            });

            auto bang_tim_kiem = gridbox(
                                        {},
                                        {});
        }

        Element Render(){
            return ;
        }
};

void ShowAgentMenu(AccountManager& account_manager, BookingManager& booking_manager, FlightManager& flight_manager){
    auto screen = ScreenInteractive::TerminalOutput();
    std::string user_name = " 👤" + account_manager.getCurrentUser()->getFullName() + " - Agent ";
    // --- ĐỊNH NGHĨA CÁC TRẠNG THÁI (DÙNG SỐ NGUYÊN) ---
    // Container::Tab bắt buộc phải dùng int* để điều khiển

    // Trạng thái cho menu bên trái
    int menu_selector = 0;
    // 0 = booking
    // 1 = Passengers
    // 2 = Profile

    // Trạng thái cho nội dung bên phải
    int content_selector = 0;
    // 0 = None (Màn hình trống)
    // 1 = Booking List , 2 = Booking(Search Instance, Book) , 3 = Cancel(Search booking list, Cancel)
    // 4 = ListPassengers, 5 = , 6 = 
    // 7 = My Information, 8 = Edit Profile , 9 = Account Manager

    // --- CÁC COMPONENT NỘI DUNG (CHO BÊN PHẢI) ---
    auto none_screen = Renderer([] {
        return vbox({text("HỆ THỐNG QUẢN LÝ BÁN VÉ MÁY BAY") | bold | center | dim,
        });
    });

    //Booking
    BookingList Booking_list(booking_manager);
    // auto list_booking_screen = Renderer([] { return text("Giao diện Danh sách đặt vé") | center; });
    auto add_booking_screen = Renderer([] { return text("Giao diện Đặt vé") | center; });
    auto cancel_booking_screen = Renderer([] { return text("Giao diện Hủy vé") | center; });

    //Passengers
    auto list_passenger_screen = Renderer([] { return text("Giao diện danh sách khách hàng"); });
    auto A_passenger_screen = Renderer([] { return text("Giao diện A khách hàng"); });
    auto B_passenger_screen = Renderer([] { return text("Giao diện B khách hàng"); });

    //Profile
    auto my_information_screen = Renderer([] { return text("Thông tin cá nhân"); });
    auto edit_profile_screen = Renderer([] { return text("Chỉnh sửa thông tin cá nhân"); });
    auto account_manager_screen = Renderer([] { return text("Quản lý tài khoản"); });

    // --- 3. TẠO CÁC COMPONENT MENU (CHO BÊN TRÁI) ---
    // Các biến 'selected_...' để theo dõi mục nào đang được tô đậm
    int selected_main = 0;
    int selected_booking = 0;
    int selected_passenger = 0;
    int selected_info = 0;

    // === Menu Chính (Level 1) ===
    std::vector<std::string> main_menu_entries = {
        "Đặt vé",
        "Quản lý khách hàng",
        "Thông tin cá nhân",
    };
    auto main_menu_options = MenuOption::Vertical();
    main_menu_options.on_change = [&] {
        if (selected_main == 0) {
                menu_selector = 1;          // Chuyển sang menu Booking (index 1)
                content_selector = 1;      // Chuyển sang screen Booking list  (index 1)
                selected_booking = 0;       // Focus vào mục 0 của menu con
        }
        if (selected_main == 1) {
                menu_selector = 2;          // Chuyển sang menu Passenger
                content_selector = 4;      // Chuyển sang screen PassengerList (index 4)
                selected_passenger = 0;
        }
        if (selected_main == 2) {
                menu_selector = 3;          // Chuyển sang menu Profile
                content_selector = 7;      // Chuyển sang screen My Information (index 7)
                selected_info = 0;
        }
    };
    auto main_menu = Menu(&main_menu_entries, &selected_main, main_menu_options);

    // === Menu Booking (Level 2) ===
    std::vector<std::string> booking_menu_entries = { "Danh sách vé đã đặt", "Đặt vé mới", "Hủy vé" };
    auto booking_menu_options = MenuOption::Vertical();
    booking_menu_options.on_change = [&] {
        if (selected_booking == 0) content_selector = 1; // List Booking
        if (selected_booking == 1) content_selector = 2; // Book
        if (selected_booking == 2) content_selector = 3; // Cancel
    };
    auto Booking_menu = Menu(&booking_menu_entries, &selected_booking, booking_menu_options);
    auto back_button_booking = Button("       < Quay lại", [&] { 
        menu_selector = 0;   // Quay về menu Main (index 0)
        content_selector = 0; // Quay về screen None (index 0)
    });
    auto booking_menu_group = Container::Vertical({ Booking_menu, back_button_booking });

    auto booking_menu_renderer = Renderer(booking_menu_group, [&] {
        return vbox({
            Booking_menu->Render()| flex ,
            separatorLight(),
            back_button_booking->Render()
        }) | size(HEIGHT, EQUAL, 40) | flex;
    });

    // === Menu Passenger (Level 2) ===
    std::vector<std::string> passenger_menu_entries = { "Danh sách khách hàng", "WWWWWWW", "WWWWWWW" };
    auto passenger_menu_options = MenuOption::Vertical();
    passenger_menu_options.on_change = [&] {
        if (selected_passenger == 0) content_selector = 4; // ListInstances
        if (selected_passenger == 1) content_selector = 5; // AddInstance
        if (selected_passenger == 2) content_selector = 6; // SearchInstance
    };
    auto passenger_menu = Menu(&passenger_menu_entries, &selected_passenger, passenger_menu_options);
    auto back_button_passenger = Button("       < Quay lại", [&] { 
        menu_selector = 0; 
        content_selector = 0; 
    });

    auto passenger_menu_group = Container::Vertical({ passenger_menu, back_button_passenger });
    auto passenger_menu_renderer = Renderer(passenger_menu_group, [&] {
        return vbox({
            passenger_menu->Render()| flex ,
            separatorLight(),
            back_button_passenger->Render()
        }) | size(HEIGHT, EQUAL, 40)| flex;
    });

    // === Menu Đại Lý (Level 2) ===
    std::vector<std::string> info_menu_entries = { "Thông tin cá nhân", "Chỉnh sửa thông tin", "Quản lý tài khoản" };
    auto info_menu_options = MenuOption::Vertical();
    info_menu_options.on_change = [&] {
        if (selected_info == 0) content_selector = 7; // ListAgents
        if (selected_info == 1) content_selector = 8; // AddAgent
        if (selected_info == 2) content_selector = 9; // SearchAgent
    };
    auto agent_menu = Menu(&info_menu_entries, &selected_info, info_menu_options);
    auto back_button_info = Button("       < Quay lại", [&] { 
        menu_selector = 0; 
        content_selector = 0; 
    });
    auto info_menu_group = Container::Vertical({ agent_menu, back_button_info });
    auto info_menu_renderer = Renderer(info_menu_group, [&] {
        return vbox({
            agent_menu->Render() | flex ,
            separatorLight(),
            back_button_info->Render()
        }) | size(HEIGHT, EQUAL, 40) |flex;
    });

    // --- TẠO CÁC CONTAINER TAB ---

    // Container cho BÊN TRÁI (Menus)
    auto left_pane_container = Container::Tab(
        {
            main_menu,              // index 0
            booking_menu_renderer,   // index 1
            passenger_menu_renderer, // index 2
            info_menu_renderer,    // index 3
        },
        &menu_selector // Biến int này điều khiển menu nào đang active
    );

    // Container cho BÊN PHẢI (Content)
auto right_pane_container = Container::Tab(
{
        none_screen,                  // index 0

        Booking_list.container,        // index 1
        // Renderer(add_Booking_screen.container, [&] { // index 2
        //   return add_Booking_screen.Render();
        // }),
        add_booking_screen,
        cancel_booking_screen,
        list_passenger_screen,
        A_passenger_screen,
        B_passenger_screen,
        my_information_screen,
        edit_profile_screen,
        account_manager_screen,
    },
    &content_selector
);

    // --- Nút Đăng xuất ---
    auto logout_button = Button("        Đăng xuất", [&]{
        account_manager.logout();
        current_screen = ApplicationScreen::Login;
        screen.Exit();
    });

    // Container nhóm cột trái và nút đăng xuất
    auto left_pane_group = Container::Vertical({
        left_pane_container,
        logout_button
    });

    // Container chính của toàn bộ màn hình
    // Nó tự động quản lý layout (trái-phải)
    auto main_container = Container::Horizontal({
        left_pane_group,
        right_pane_container, 
    });

    // --- HỌA SĨ (RENDERER) ---
    auto main_renderer = Renderer(main_container, [&] {
        // Menu bên trái
        auto left_pane = vbox({
                left_pane_container->Render(), // render Tab container
                separatorEmpty() | flex,
                logout_button->Render(),
        }) | size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 40) | border;

        // Nội dung bên phải
        auto right_pane = vbox({
                right_pane_container->Render() | flex |center, // render Tab container
        }) | border;

        // Ghép 2 cột
        auto layout = hbox({
                left_pane,
                right_pane | flex,
        });

        return window(text(user_name), layout);
    });

    selected_main = 2;// trạng thái ban đầu

    screen.Loop(main_renderer);
}