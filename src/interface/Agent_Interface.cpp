#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include "C:/PBL2/include/main_state.h"
#include "C:/PBL2/include/core/AccountManager.h"
#include "C:/PBL2/include/core/FlightManager.h"
#include "C:/PBL2/include/entities/Flight.h"
#include "C:/PBL2/include/DSA/DynamicArray.h" 
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

// using namespace ftxui;

// class FlightList {
// public:
//     int scroll_position = 0;
//     int visible_rows = 25;
//     std::vector<std::vector<std::string>> flight_data;
//     FlightManager& flight_manager;
//     Component container;       // Container chính
//     Component table_component; // Component chứa bảng cuộn được
//     Component reload_button;   // Nút làm mới

//     FlightList(FlightManager& manager) : flight_manager(manager) {
//         reload_button = Button("     [ Làm mới danh sách ]", [&] {
//             this->LoadData();
//             this->scroll_position = 0; 
//         });

//         auto table_renderer = Renderer([&] {
//             Elements rows;
//             int total = flight_data.size();
//             int end_index = std::min(scroll_position + visible_rows, total);

//             // --- Tiêu đề bảng ---
//             auto header = hbox({
//                 text("ID tuyến bay") | bold | center | size(WIDTH, EQUAL, 15),
//                 separator(),
//                 text("Mã tuyến bay") | bold | center | size(WIDTH, EQUAL, 15),
//                 separator(),
//                 text("Hãng bay") | bold | center | size(WIDTH, EQUAL, 25),
//                 separator(),
//                 text("Sân bay đi") | bold | center | size(WIDTH, EQUAL, 10),
//                 separator(),
//                 text("Sân bay đến") | bold | center | size(WIDTH, EQUAL, 11),
//             });
//             rows.push_back(header);
//             rows.push_back(separator());

//             // --- Dòng dữ liệu ---
//             for (int i = scroll_position; i < end_index; i++) {
//                 const auto& f = flight_data[i];
//                 rows.push_back(hbox({
//                     text(f[0]) | center | size(WIDTH, EQUAL, 15),
//                     text(f[1]) | center | size(WIDTH, EQUAL, 18),
//                     text(f[2]) | center | size(WIDTH, EQUAL, 25),
//                     text(f[3]) | center | size(WIDTH, EQUAL, 11),
//                     text(f[4]) | center | size(WIDTH, EQUAL, 10),
//                 }));
//             }

//             // --- Thanh trạng thái ---
//             rows.push_back(separator());
//             rows.push_back(text("Dùng chuột để cuộn — Tổng số chuyến: " + std::to_string(total)) | dim | center);

//             return vbox(std::move(rows)) | border | size(HEIGHT, LESS_THAN, 35);
//         });

//         table_component = CatchEvent(table_renderer, [&](Event event) {
//             int total = flight_data.size();
//             if (event.is_mouse()) {
//                 if (event.mouse().button == Mouse::WheelDown) {
//                     if (scroll_position + visible_rows < total) scroll_position++;
//                     return true;
//                 }
//                 if (event.mouse().button == Mouse::WheelUp) {
//                     if (scroll_position > 0) scroll_position--;
//                     return true;
//                 }
//             }
//             return false;
//         });

//         container = Container::Vertical({
//             table_component, // Component bảng
//             reload_button  | size(WIDTH, EQUAL, 35) | center // Component nút
//         });
        
//         LoadData(); 
//     }

//     Element Render() { 
//         return vbox({
//             text("DANH SÁCH TUYẾN BAY") | bold | center,
//             table_component->Render() | center,
//             separator(),
//             reload_button->Render()
//         });
//     }

// private:
//     void LoadData() {
//         flight_data.clear();
//         const auto& flights = flight_manager.getAllFlights();
//         for (int i = 0; i < flights.size(); i++) {
//             Flight* f = flights[i];
//             if (!f) continue;
//             std::stringstream ss(f->toRecordLine());
//             std::string col;
//             std::vector<std::string> cols;
//             while (std::getline(ss, col, '|'))
//                 cols.push_back(col);
//             if (cols.size() < 5)
//                 cols.resize(5, "");
//             flight_data.push_back(cols);
//         }
//     }
// };


// class FlightInstanceList {
// public:
//     int scroll_position = 0;
//     int visible_rows = 25;
//     std::vector<std::vector<std::string>> flight_instance_data;
//     FlightManager& flight_instance_manager;
//     Component container;       // Container chính
//     Component table_component; // Component chứa bảng cuộn được
//     Component reload_button;   // Nút làm mới

//     FlightInstanceList(FlightManager& manager) : flight_instance_manager(manager) {
//         reload_button = Button("     [ Làm mới danh sách ]", [&] {
//             this->LoadData();
//             this->scroll_position = 0; 
//         });

//         auto table_renderer = Renderer([&] {
//             Elements rows;
//             int total = flight_instance_data.size();
//             int end_index = std::min(scroll_position + visible_rows, total);

//             // --- Tiêu đề bảng ---
//             auto header = hbox({
//                 text("ID chuyến") | bold | center | size(WIDTH, EQUAL, 14),
//                 separator(),
//                 text("ID tuyến") | bold | center | size(WIDTH, EQUAL, 13),
//                 separator(),
//                 text("Ngày đi") | bold | center | size(WIDTH, EQUAL, 16),
//                 separator(),
//                 text("Giờ đi") | bold | center | size(WIDTH, EQUAL, 10),
//                 separator(),
//                 text("Ngày đến") | bold | center | size(WIDTH, EQUAL, 21),
//                 separator(),
//                 text("Giờ đến") | bold | center | size(WIDTH, EQUAL, 11),
//                 separator(),
//                 text("Ghế phổ thông") | bold | center | size(WIDTH, EQUAL, 22),
//                 separator(),
//                 text("Trống") | bold | center | size(WIDTH, EQUAL, 14),
//                 separator(),
//                 text("Giá vé") | bold | center | size(WIDTH, EQUAL, 24),
//                 separator(),
//                 text("Ghế thương gia") | bold | center | size(WIDTH, EQUAL, 20),
//                 separator(),
//                 text("Trống") | bold | center | size(WIDTH, EQUAL, 11),
//                 separator(),
//                 text("Giá vé") | bold | center | size(WIDTH, EQUAL, 24),
//             });
//             rows.push_back(header);
//             rows.push_back(separator());

//             // --- Dòng dữ liệu ---
//             for (int i = scroll_position; i < end_index; i++) {
//                 const auto& f = flight_instance_data[i];
//                 rows.push_back(hbox({
//                     text(f[0]) | size(WIDTH, EQUAL, 14), //id chuyen
//                     text(f[1])  | center | size(WIDTH, EQUAL, 14), //id tuyen
//                     text(f[2])  | center | size(WIDTH, EQUAL, 18), // ngay di
//                     text(f[3])  | center | size(WIDTH, EQUAL, 12), // gio di
//                     text(f[4])  | center | size(WIDTH, EQUAL, 22), // ngay den
//                     text(f[5])  | center | size(WIDTH, EQUAL, 13), // gio den
//                     text(f[6])  | center | size(WIDTH, EQUAL, 23), // ghe pho thong
//                     text(f[7]) | center | size(WIDTH, EQUAL, 16),  // pho thong trong
//                     text(f[10]) | center |size(WIDTH, EQUAL, 25), //gia ve pho thong
//                     text(f[8])  | center |size(WIDTH, EQUAL, 22), // ghe thuong gia
//                     text(f[9])  | center |size(WIDTH, EQUAL, 12), // thuong gia trong
//                     text(f[11]) | center | size(WIDTH, EQUAL, 25), //gia ve thuong gia
//                 }));
//             }

//             // --- Thanh trạng thái ---
//             rows.push_back(separator());
//             rows.push_back(text("Dùng chuột để cuộn — Tổng số chuyến: " + std::to_string(total)) | dim | center);

//             return vbox(std::move(rows)) | border | size(HEIGHT, LESS_THAN, 35);
//         });

//         table_component = CatchEvent(table_renderer, [&](Event event) {
//             int total = flight_instance_data.size();
//             if (event.is_mouse()) {
//                 if (event.mouse().button == Mouse::WheelDown) {
//                     if (scroll_position + visible_rows < total) scroll_position++;
//                     return true;
//                 }
//                 if (event.mouse().button == Mouse::WheelUp) {
//                     if (scroll_position > 0) scroll_position--;
//                     return true;
//                 }
//             }
//             return false;
//         });

//         container = Container::Vertical({
//             table_component, // Component bảng
//             reload_button  | size(WIDTH, EQUAL, 35) | center // Component nút
//         });
        
//         LoadData(); 
//     }

//     Element Render() { 
//         return vbox({
//             text("DANH SÁCH CHUYẾN BAY") | bold | center,
//             table_component->Render() | center,
//             separator(),
//             reload_button->Render()
//         });
//     }

// private:
//     void LoadData() {
//         flight_instance_data.clear();
//         const auto& flights_instances = flight_instance_manager.getAllInstances();
//         for (int i = 0; i < flights_instances.size(); i++) {
//             FlightInstance* f = flights_instances[i];
//             if (!f) continue;
//             std::stringstream ss(f->toRecordLine());
//             std::string col;
//             std::vector<std::string> cols;
//             while (std::getline(ss, col, '|'))
//                 cols.push_back(col);
//             if (cols.size() < 12)
//                 cols.resize(12, "");
//             flight_instance_data.push_back(cols);
//         }
//     }
// };

// void ShowbookingMenu(AccountManager& account_manager, FlightManager& flight_manager) {
//     auto screen = ScreenInteractive::TerminalOutput();
//     std::string user_name = " 👤" + account_manager.getCurrentUser()->getFullName() + " - booking ";

//     // Trạng thái cho menu bên trái
//     int menu_selector = 0; 
//     // 0 = Main
//     // 1 = Flight
//     // 2 = Instance
//     // 3 = Booking

//     // Trạng thái cho nội dung bên phải
//     int content_selector = 0; 
//     // 0 = None (Màn hình trống)
//     // 1 = ListFlights, 2 = SearchFlight 
//     // 3 = ListInstances, 4 = SearchInstance
//     // 5 = ListBookings, 6 = AddBooking, 7 = CancelBooking

//     // --- CÁC COMPONENT NỘI DUNG (CHO BÊN PHẢI) ---

//     auto none_screen = Renderer([] {
//         return vbox({text("HỆ THỐNG QUẢN LÝ BÁN VÉ MÁY BAY") | bold | center | dim,
//         });
//     });
//     //Flight
//     FlightList flight_list(flight_manager);
//     // AddFlightScreen add_flight_screen(flight_manager);
//     auto search_flight_screen = Renderer([] { return text("Giao diện Tìm kiếm Tuyến bay") | center; });
    
//     //Flight Instance
//     FlightInstanceList flight_instance_list(flight_manager);
//     // AddFlightInstanceScreen add_flight_instance_screen(flight_manager);
//     auto search_instance_screen = Renderer([] { return text("Giao diện Tìm kiếm Chuyến bay") | center; });

//     // Booking
//     auto list_booking_screen = Renderer([] { return text("Giao diện Danh sách đặt vé") | center; });
//     auto add_booking_screen = Renderer([] { return text("Giao diện đặt vé") | center; });
//     auto cancel_booking_screen = Renderer([] { return text("Giao diện hủy đặt vé") | center; });
//     // auto search_flight_screen = Renderer([] { return text("Giao diện Tìm kiếm Tuyến bay") | center; });
//     // auto search_instance_screen = Renderer([] { return text("Giao diện Tìm kiếm Chuyến bay") | center; });

//     // --- 3. TẠO CÁC COMPONENT MENU (CHO BÊN TRÁI) ---
//     // Các biến 'selected_...' để theo dõi mục nào đang được tô đậm
//     int selected_main = 0;
//     int selected_flight = 0;
//     int selected_instance = 0;
//     int selected_booking = 0;

//     // === Menu Chính (Level 1) ===
//     std::vector<std::string> main_menu_entries = {
//         "TUYẾN BAY",
//         "CHUYẾN BAY",
//         "BOOKING",
//     };
//     auto main_menu_options = MenuOption::Vertical();
//     main_menu_options.on_change = [&] {
//         if (selected_main == 0) {
//                 menu_selector = 1;          // Chuyển sang menu Flight (index 1)
//                 content_selector = 1;      // Chuyển sang screen ListFlights (index 1)
//                 selected_flight = 0;       // Focus vào mục 0 của menu con
//         }
//         if (selected_main == 1) {
//                 menu_selector = 2;          // Chuyển sang menu Instance
//                 content_selector = 3;      // Chuyển sang screen ListInstances (index 3)
//                 selected_instance = 0;
//         }
//         if (selected_main == 2) {
//                 menu_selector = 3;          // Chuyển sang menu Booking
//                 content_selector = 5;      // Chuyển sang screen ListBookings (index 5)
//                 selected_booking = 0;
//         }
//     };
//     auto main_menu = Menu(&main_menu_entries, &selected_main, main_menu_options);

//     // === Menu Tuyến Bay (Level 2) ===
//     std::vector<std::string> flight_menu_entries = { "Danh sách tuyến bay", "Tìm kiếm tuyến bay" };
//     auto flight_menu_options = MenuOption::Vertical();
//     flight_menu_options.on_change = [&] {
//         if (selected_flight == 0) content_selector = 1; // ListFlights
//         if (selected_flight == 2) content_selector = 2; // SearchFlight
//     };
//     auto flight_menu = Menu(&flight_menu_entries, &selected_flight, flight_menu_options);
//     auto back_button_flight = Button("       < Quay lại", [&] { 
//         menu_selector = 0;   // Quay về menu Main (index 0)
//         content_selector = 0; // Quay về screen None (index 0)
//     });
//     auto flight_menu_group = Container::Vertical({ flight_menu, back_button_flight });

//     auto flight_menu_renderer = Renderer(flight_menu_group, [&] {
//         return vbox({
//             flight_menu->Render()| flex ,
//             separatorLight(),
//             back_button_flight->Render()
//         }) | size(HEIGHT, EQUAL, 40) | flex;
//     });

//     // === Menu Chuyến Bay (Level 2) ===
//     std::vector<std::string> instance_menu_entries = { "Danh sách chuyến bay", "Tìm kiếm chuyến bay" };
//     auto instance_menu_options = MenuOption::Vertical();
//     instance_menu_options.on_change = [&] {
//         if (selected_instance == 0) content_selector = 3; // ListInstances
//         if (selected_instance == 2) content_selector = 4; // SearchInstance
//     };
//     auto instance_menu = Menu(&instance_menu_entries, &selected_instance, instance_menu_options);
//     auto back_button_instance = Button("       < Quay lại", [&] { 
//         menu_selector = 0; 
//         content_selector = 0; 
//     });

//     auto instance_menu_group = Container::Vertical({ instance_menu, back_button_instance });
//     auto instance_menu_renderer = Renderer(instance_menu_group, [&] {
//         return vbox({
//             instance_menu->Render()| flex ,
//             separatorLight(),
//             back_button_instance->Render()
//         }) | size(HEIGHT, EQUAL, 40)| flex;
//     });

//     // === Menu Đại Lý (Level 2) ===
//     std::vector<std::string> booking_menu_entries = { "Danh sách đặt vé", "Đặt vé", "Hủy vé" };
//     auto booking_menu_options = MenuOption::Vertical();
//     booking_menu_options.on_change = [&] {
//         if (selected_booking == 0) content_selector = 5; // ListBookings
//         if (selected_booking == 1) content_selector = 6; // AddBooking
//         if (selected_booking == 2) content_selector = 7; // CancelBooking
//     };
//     auto booking_menu = Menu(&booking_menu_entries, &selected_booking, booking_menu_options);
//     auto back_button_booking = Button("       < Quay lại", [&] { 
//         menu_selector = 0; 
//         content_selector = 0; 
//     });
//     auto booking_menu_group = Container::Vertical({ booking_menu, back_button_booking });
//     auto booking_menu_renderer = Renderer(booking_menu_group, [&] {
//         return vbox({
//             booking_menu->Render() | flex ,
//             separatorLight(),
//             back_button_booking->Render()
//         }) | size(HEIGHT, EQUAL, 40) |flex;
//     });

//     // --- TẠO CÁC CONTAINER TAB ---

//     // Container cho BÊN TRÁI (Menus)
//     auto left_pane_container = Container::Tab(
//         {
//             main_menu,              // index 0
//             flight_menu_renderer,   // index 1
//             instance_menu_renderer, // index 2
//             booking_menu_renderer,    // index 3
//         },
//         &menu_selector // Biến int này điều khiển menu nào đang active
//     );

//     // Container cho BÊN PHẢI (Content)
// auto right_pane_container = Container::Tab(
// {
//         none_screen,                  // index 0

//         flight_list.container,        // index 1
//                     // Renderer(add_flight_screen.container, [&] { // index 2
//                     //   return add_flight_screen.Render();
//                     // }),
//         search_flight_screen,         // index 2

//         flight_instance_list.container,        // index 3
//                     // Renderer(add_flight_instance_screen.container, [&] { // index 5
//                     //         return add_flight_instance_screen.Render();
//                     // }), 
//         search_instance_screen,      // index 4

//         list_booking_screen,          // index 5
//         add_booking_screen,            // index 6
//         cancel_booking_screen,         // index 7
//     },
//     &content_selector
// );

//     // --- Nút Đăng xuất ---
//     auto logout_button = Button("        Đăng xuất", [&]{
//         account_manager.logout();
//         current_screen = ApplicationScreen::Login;
//         screen.Exit();
//     });

//     // Container nhóm cột trái và nút đăng xuất
//     auto left_pane_group = Container::Vertical({
//         left_pane_container,
//         logout_button
//     });

//     // Container chính của toàn bộ màn hình
//     // Nó tự động quản lý layout (trái-phải)
//     auto main_container = Container::Horizontal({
//         left_pane_group,
//         right_pane_container, 
//     });

//     // --- HỌA SĨ (RENDERER) ---
//     auto main_renderer = Renderer(main_container, [&] {
//         // Menu bên trái
//         auto left_pane = vbox({
//                 left_pane_container->Render(), // render Tab container
//                 separatorEmpty() | flex,
//                 logout_button->Render(),
//         }) | size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 40) | border;

//         // Nội dung bên phải
//         auto right_pane = vbox({
//                 right_pane_container->Render() | flex |center, // render Tab container
//         }) | border;

//         // Ghép 2 cột
//         auto layout = hbox({
//                 left_pane,
//                 right_pane | flex,
//         });
//
//         return window(text(user_name), layout);
//     });
//
//     selected_main = -1;// trạng thái ban đầu

//     screen.Loop(main_renderer);
// }

void ShowbookingMenu(AccountManager& account_manager, FlightManager& flight_manager){
    
}