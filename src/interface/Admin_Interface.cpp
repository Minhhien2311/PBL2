#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include "C:/PBL2/include/core/AccountManager.h"
#include "C:/PBL2/include/main_state.h"
#include "C:/PBL2/include/core/FlightManager.h"
#include "C:/PBL2/include/entities/Flight.h"
#include "C:/PBL2/include/DSA/DynamicArray.h" 
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

// ======================================
// ========= FLIGHT INTERFACE ===========
// ======================================

// ------ I. Danh sách tuyến bay -----------
class FlightList {
public:
    int scroll_position = 0;
    int visible_rows = 25;
    std::vector<std::vector<std::string>> flight_data;
    FlightManager& flight_manager;
    Component container;       // Container chính
    Component table_component; // Component chứa bảng cuộn được
    Component reload_button;   // Nút làm mới

    FlightList(FlightManager& manager) : flight_manager(manager) {
        reload_button = Button("     [ Làm mới danh sách ]", [&] {
            this->LoadData();
            this->scroll_position = 0; 
        });

        auto table_renderer = Renderer([&] {
            Elements rows;
            int total = flight_data.size();
            int end_index = std::min(scroll_position + visible_rows, total);

            // --- Tiêu đề bảng ---
            auto header = hbox({
                text("ID tuyến bay") | bold | center | size(WIDTH, EQUAL, 15),
                separator(),
                text("Mã tuyến bay") | bold | center | size(WIDTH, EQUAL, 15),
                separator(),
                text("Hãng bay") | bold | center | size(WIDTH, EQUAL, 25),
                separator(),
                text("Sân bay đi") | bold | center | size(WIDTH, EQUAL, 10),
                separator(),
                text("Sân bay đến") | bold | center | size(WIDTH, EQUAL, 11),
            });
            rows.push_back(header);
            rows.push_back(separator());

            // --- Dòng dữ liệu ---
            for (int i = scroll_position; i < end_index; i++) {
                const auto& f = flight_data[i];
                rows.push_back(hbox({
                    text(f[0]) | center | size(WIDTH, EQUAL, 15),
                    text(f[1]) | center | size(WIDTH, EQUAL, 18),
                    text(f[2]) | center | size(WIDTH, EQUAL, 25),
                    text(f[3]) | center | size(WIDTH, EQUAL, 11),
                    text(f[4]) | center | size(WIDTH, EQUAL, 10),
                }));
            }

            // --- Thanh trạng thái ---
            rows.push_back(separator());
            rows.push_back(text("Dùng chuột để cuộn — Tổng số chuyến: " + std::to_string(total)) | dim | center);

            return vbox(std::move(rows)) | border | size(HEIGHT, LESS_THAN, 35);
        });

        table_component = CatchEvent(table_renderer, [&](Event event) {
            int total = flight_data.size();
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
            text("DANH SÁCH TUYẾN BAY") | bold | center,
            table_component->Render() | center,
            separator(),
            reload_button->Render()
        });
    }

private:
    void LoadData() {
        flight_data.clear();
        const auto& flights = flight_manager.getAllFlights();
        for (int i = 0; i < flights.size(); i++) {
            Flight* f = flights[i];
            if (!f) continue;
            std::stringstream ss(f->toRecordLine());
            std::string col;
            std::vector<std::string> cols;
            while (std::getline(ss, col, '|'))
                cols.push_back(col);
            if (cols.size() < 5)
                cols.resize(5, "");
            flight_data.push_back(cols);
        }
    }
};

// --- II. Thêm tuyến bay ---
class AddFlightScreen {
public:
    // Trạng thái (dữ liệu) của màn hình này
    std::string flightID;
    std::string airline;
    std::string departure;
    std::string arrival;
    std::string thong_bao = "";
    FlightManager& flight_manager;

    // Component chính chứa các thành phần con
    Component container;

    AddFlightScreen(FlightManager& flight_manager) : flight_manager(flight_manager){
        // Tạo các component con
        Component input_flightID = Input(&flightID, "(VD:VN123)");
        Component input_airline = Input(&airline, "(VD: Vietnam Airline)");
        Component input_departure = Input(&departure, "(VD: HAN)");
        Component input_arrival = Input(&arrival, "(VD: DAN)");

        auto them_button = Button("Thêm ", [&] {
            // Logic khi nhấn nút "Thêm"
            bool new_flight = flight_manager.createNewFlight(flightID,airline,departure,arrival);
            if (new_flight){
                if (flight_manager.saveFlightsToFiles("C:/PBL2/data/flights.txt") == 1) 
                    thong_bao = "Thêm thành công tuyến bay " + flightID + "|" + departure + " - " + arrival +"!";
                else
                    thong_bao = "Xảy ra lỗi trong quá trình thêm tuyến bay!";
                // Xóa dữ liệu cũ
                flightID = "";
                airline = "";
                departure = "";
                arrival = "";
            }
            else
                thong_bao = "Không thể thêm tuyến bay trống hoặc đã tồn tại!";
            
        });

        container = Container::Vertical({
            input_flightID,
            input_airline,
            input_departure,
            input_arrival,
            them_button
        });
    }

    Element Render() {
        return vbox({
            text("THÊM TUYẾN BAY MỚI") | bold | center | color(Color::Green),
            separator(),
            vbox({
                hbox({ text("Mã tuyến bay:  "), container->ChildAt(0)->Render() }) | borderLight,
                hbox({ text("Hãng bay:      "), container->ChildAt(1)->Render() }) | borderLight,
                hbox({ text("Sân bay đi:    "), container->ChildAt(2)->Render() }) | borderLight,
                hbox({ text("Sân bay đến:   "), container->ChildAt(3)->Render() }) | borderLight,
            }) | size(WIDTH, EQUAL, 60),
            separator(),
            container->ChildAt(4)->Render() | center,
            separator(),
            text(thong_bao) | center | 
                (thong_bao.find("thành công") != std::string::npos ? color(Color::Green) : color(Color::Red))
        }) | border | size(WIDTH, EQUAL, 70) | center;
    }
};

// --------- III. Tìm kiếm tuyến bay ----------------


// ===============================================
// ========= FLIGHT INSTANCE INTERFACE ===========
// ===============================================

// -------- I. Danh sách chuyến bay --------------

class FlightInstanceList {
public:
    int scroll_position = 0;
    int visible_rows = 25;
    std::vector<std::vector<std::string>> flight_instance_data;
    FlightManager& flight_instance_manager;
    Component container;       // Container chính
    Component table_component; // Component chứa bảng cuộn được
    Component reload_button;   // Nút làm mới

    FlightInstanceList(FlightManager& manager) : flight_instance_manager(manager) {
        reload_button = Button("     [ Làm mới danh sách ]", [&] {
            this->LoadData();
            this->scroll_position = 0; 
        });

        auto table_renderer = Renderer([&] {
            Elements rows;
            int total = flight_instance_data.size();
            int end_index = std::min(scroll_position + visible_rows, total);

            // --- Tiêu đề bảng ---
            auto header = hbox({
                text("ID chuyến") | bold | center | size(WIDTH, EQUAL, 14),
                separator(),
                text("ID tuyến") | bold | center | size(WIDTH, EQUAL, 13),
                separator(),
                text("Ngày đi") | bold | center | size(WIDTH, EQUAL, 16),
                separator(),
                text("Giờ đi") | bold | center | size(WIDTH, EQUAL, 10),
                separator(),
                text("Ngày đến") | bold | center | size(WIDTH, EQUAL, 21),
                separator(),
                text("Giờ đến") | bold | center | size(WIDTH, EQUAL, 11),
                separator(),
                text("Ghế phổ thông") | bold | center | size(WIDTH, EQUAL, 22),
                separator(),
                text("Trống") | bold | center | size(WIDTH, EQUAL, 14),
                separator(),
                text("Giá vé") | bold | center | size(WIDTH, EQUAL, 24),
                separator(),
                text("Ghế thương gia") | bold | center | size(WIDTH, EQUAL, 20),
                separator(),
                text("Trống") | bold | center | size(WIDTH, EQUAL, 11),
                separator(),
                text("Giá vé") | bold | center | size(WIDTH, EQUAL, 24),
            });
            rows.push_back(header);
            rows.push_back(separator());

            // --- Dòng dữ liệu ---
            for (int i = scroll_position; i < end_index; i++) {
                const auto& f = flight_instance_data[i];
                rows.push_back(hbox({
                    text(f[0]) | size(WIDTH, EQUAL, 15), //id chuyen
                    text(f[1])  | center | size(WIDTH, EQUAL, 14), //id tuyen
                    text(f[2])  | center | size(WIDTH, EQUAL, 18), // ngay di
                    text(f[3])  | center | size(WIDTH, EQUAL, 12), // gio di
                    text(f[4])  | center | size(WIDTH, EQUAL, 22), // ngay den
                    text(f[5])  | center | size(WIDTH, EQUAL, 13), // gio den
                    text(f[6])  | center | size(WIDTH, EQUAL, 23), // ghe pho thong
                    text(f[7]) | center | size(WIDTH, EQUAL, 16),  // pho thong trong
                    text(f[10]) | center |size(WIDTH, EQUAL, 25), //gia ve pho thong
                    text(f[8])  | center |size(WIDTH, EQUAL, 22), // ghe thuong gia
                    text(f[9])  | center |size(WIDTH, EQUAL, 12), // thuong gia trong
                    text(f[11]) | center | size(WIDTH, EQUAL, 25), //gia ve thuong gia
                }));
            }

            // --- Thanh trạng thái ---
            rows.push_back(separator());
            rows.push_back(text("Dùng chuột để cuộn — Tổng số chuyến: " + std::to_string(total)) | dim | center);

            return vbox(std::move(rows)) | border | size(HEIGHT, LESS_THAN, 35);
        });

        table_component = CatchEvent(table_renderer, [&](Event event) {
            int total = flight_instance_data.size();
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
            text("DANH SÁCH CHUYẾN BAY") | bold | center,
            table_component->Render() | center,
            separator(),
            reload_button->Render()
        });
    }

private:
    void LoadData() {
        flight_instance_data.clear();
        const auto& flights_instances = flight_instance_manager.getAllInstances();
        for (int i = 0; i < flights_instances.size(); i++) {
            FlightInstance* f = flights_instances[i];
            if (!f) continue;
            std::stringstream ss(f->toRecordLine());
            std::string col;
            std::vector<std::string> cols;
            while (std::getline(ss, col, '|'))
                cols.push_back(col);
            if (cols.size() < 12)
                cols.resize(12, "");
            flight_instance_data.push_back(cols);
        }
    }
};

// ------- II. Thêm chuyến bay  -----------
class AddFlightInstanceScreen {
public:
    // Trạng thái (dữ liệu) của màn hình này
    std::string flightID;
    std::string departureDate;
    std::string departureTime;
    std::string arrivalDate;
    std::string arrivalTime;
    std::string economyTotal;
    std::string businessTotal;
    std::string fareEconomy;
    std::string fareBusiness;
    std::string thong_bao = "";
    FlightManager& flight_manager;

    // Component chính chứa các thành phần con
    Component container;

    AddFlightInstanceScreen(FlightManager& flight_manager) : flight_manager(flight_manager){
        // Tạo các component con
        Component input_flightID = Input(&flightID, "(VD: FI-001)");
        Component input_departureDate = Input(&departureDate, "(DD/MM/YYYY)");
        Component input_departureTime = Input(&departureTime, "(TT/MM)");
        Component input_arrivalDate = Input(&arrivalDate, "(DD/MM/YYYY)");
        Component input_arrivalTime = Input(&arrivalTime, "(TT/MM)");
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
                                                                        departureDate,
                                                                        departureTime,
                                                                        arrivalDate,
                                                                        arrivalTime,
                                                                        std::stoi(economyTotal),
                                                                        std::stoi(businessTotal),
                                                                        std::stoi(fareEconomy),
                                                                        std::stoi(fareBusiness));

                if (new_flight_instance){
                    if (flight_manager.saveInstancesToFiles("C:/PBL2/data/flight_instances.txt"))
                        thong_bao = "Thêm thành công chuyến bay " + flightID + " !";
                    else 
                        thong_bao = "Xảy ra lỗi trong quá trình thêm chuyến bay!";
                    // Xóa dữ liệu cũ
                    flightID = "";
                    departureDate,
                    departureTime,
                    arrivalDate,
                    arrivalTime,
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
            input_departureDate,
            input_departureTime,
            input_arrivalDate,
            input_arrivalTime,
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
            text("THÊM CHUYẾN BAY MỚI") | bold | center | color(Color::Green),
            separator(),
            vbox({
                hbox({ text("ID tuyến bay:        "), container->ChildAt(0)->Render() }) | borderLight,
                hbox({ text("Ngày khởi hành:      "), container->ChildAt(1)->Render() }) | borderLight,
                hbox({ text("Giờ khởi hành:       "), container->ChildAt(2)->Render() }) | borderLight,
                hbox({ text("Ngày hạ cánh:        "), container->ChildAt(3)->Render() }) | borderLight,
                hbox({ text("Giờ hạ cánh:         "), container->ChildAt(4)->Render() }) | borderLight,
                hbox({ text("Ghế phổ thông:       "), container->ChildAt(5)->Render() }) | borderLight,
                hbox({ text("Ghế thương gia:      "), container->ChildAt(6)->Render() }) | borderLight,
                hbox({ text("Giá vé phổ thông:    "), container->ChildAt(7)->Render() }) | borderLight,
                hbox({ text("Giá vé thương gia:   "), container->ChildAt(8)->Render() }) | borderLight,
            }) | size(WIDTH, EQUAL, 80),
            separator(),
            container->ChildAt(9)->Render() | center,
            separator(),
            text(thong_bao) | center | 
                (thong_bao.find("thành công") != std::string::npos ? color(Color::Green) : color(Color::Red))
        }) | border | size(WIDTH, EQUAL, 90) | center;
    }
};

void ShowAdminMenu(AccountManager& account_manager, FlightManager& flight_manager){
    auto screen = ScreenInteractive::TerminalOutput();
    std::string user_name = " 👤" + account_manager.getCurrentUser()->getFullName() + " - Admin ";
    // --- ĐỊNH NGHĨA CÁC TRẠNG THÁI (DÙNG SỐ NGUYÊN) ---
    // Container::Tab bắt buộc phải dùng int* để điều khiển

    // Trạng thái cho menu bên trái
    int menu_selector = 0;
    // 0 = Main
    // 1 = Flight
    // 2 = Instance
    // 3 = Agent

    // Trạng thái cho nội dung bên phải
    int content_selector = 0; 
    // 0 = None (Màn hình trống)
    // 1 = ListFlights, 2 = AddFlight, 3 = SearchFlight
    // 4 = ListInstances, 5 = AddInstance, 6 = SearchInstance
    // 7 = ListAgents, 8 = AddAgent, 9 = SearchAgent

    // --- CÁC COMPONENT NỘI DUNG (CHO BÊN PHẢI) ---

    auto none_screen = Renderer([] {
        return vbox({text("HỆ THỐNG QUẢN LÝ BÁN VÉ MÁY BAY") | bold | center | dim,
        });
    });
    //Flight
    FlightList flight_list(flight_manager);
    AddFlightScreen add_flight_screen(flight_manager);
    auto search_flight_screen = Renderer([] { return text("Giao diện Tìm kiếm Tuyến bay") | center; });
    
    //Flight Instance
    FlightInstanceList flight_instance_list(flight_manager);
    AddFlightInstanceScreen add_flight_instance_screen(flight_manager);
    auto search_instance_screen = Renderer([] { return text("Giao diện Tìm kiếm Chuyến bay") | center; });

    //Agent
    auto list_agents_screen = Renderer([] { return text("Giao diện Danh sách Đại lý") | center; });
    auto add_agent_screen = Renderer([] { return text("Giao diện Thêm Đại lý") | center; });
    auto search_agent_screen = Renderer([] { return text("Giao diện Tìm kiếm Đại lý") | center; });


    // --- 3. TẠO CÁC COMPONENT MENU (CHO BÊN TRÁI) ---
    // Các biến 'selected_...' để theo dõi mục nào đang được tô đậm
    int selected_main = 0;
    int selected_flight = 0;
    int selected_instance = 0;
    int selected_agent = 0;

    // === Menu Chính (Level 1) ===
    std::vector<std::string> main_menu_entries = {
        "Quản lý tuyến bay",
        "Quản lý chuyến bay",
        "Quản lý đại lý",
    };
    auto main_menu_options = MenuOption::Vertical();
    main_menu_options.on_change = [&] {
        if (selected_main == 0) {
                menu_selector = 1;          // Chuyển sang menu Flight (index 1)
                content_selector = 1;      // Chuyển sang screen ListFlights (index 1)
                selected_flight = 0;       // Focus vào mục 0 của menu con
        }
        if (selected_main == 1) {
                menu_selector = 2;          // Chuyển sang menu Instance
                content_selector = 4;      // Chuyển sang screen ListInstances (index 4)
                selected_instance = 0;
        }
        if (selected_main == 2) {
                menu_selector = 3;          // Chuyển sang menu Agent
                content_selector = 7;      // Chuyển sang screen ListAgents (index 7)
                selected_agent = 0;
        }
    };
    auto main_menu = Menu(&main_menu_entries, &selected_main, main_menu_options);

    // === Menu Tuyến Bay (Level 2) ===
    std::vector<std::string> flight_menu_entries = { "Danh sách tuyến bay", "Thêm tuyến bay mới", "Tìm kiếm tuyến bay" };
    auto flight_menu_options = MenuOption::Vertical();
    flight_menu_options.on_change = [&] {
        if (selected_flight == 0) content_selector = 1; // ListFlights
        if (selected_flight == 1) content_selector = 2; // AddFlight
        if (selected_flight == 2) content_selector = 3; // SearchFlight
    };
    auto flight_menu = Menu(&flight_menu_entries, &selected_flight, flight_menu_options);
    auto back_button_flight = Button("       < Quay lại", [&] { 
        menu_selector = 0;   // Quay về menu Main (index 0)
        content_selector = 0; // Quay về screen None (index 0)
    });
    auto flight_menu_group = Container::Vertical({ flight_menu, back_button_flight });

    auto flight_menu_renderer = Renderer(flight_menu_group, [&] {
        return vbox({
            flight_menu->Render()| flex ,
            separatorLight(),
            back_button_flight->Render()
        }) | size(HEIGHT, EQUAL, 40) | flex;
    });

    // === Menu Chuyến Bay (Level 2) ===
    std::vector<std::string> instance_menu_entries = { "Danh sách chuyến bay", "Thêm chuyến bay mới", "Tìm kiếm chuyến bay" };
    auto instance_menu_options = MenuOption::Vertical();
    instance_menu_options.on_change = [&] {
        if (selected_instance == 0) content_selector = 4; // ListInstances
        if (selected_instance == 1) content_selector = 5; // AddInstance
        if (selected_instance == 2) content_selector = 6; // SearchInstance
    };
    auto instance_menu = Menu(&instance_menu_entries, &selected_instance, instance_menu_options);
    auto back_button_instance = Button("       < Quay lại", [&] { 
        menu_selector = 0; 
        content_selector = 0; 
    });

    auto instance_menu_group = Container::Vertical({ instance_menu, back_button_instance });
    auto instance_menu_renderer = Renderer(instance_menu_group, [&] {
        return vbox({
            instance_menu->Render()| flex ,
            separatorLight(),
            back_button_instance->Render()
        }) | size(HEIGHT, EQUAL, 40)| flex;
    });

    // === Menu Đại Lý (Level 2) ===
    std::vector<std::string> agent_menu_entries = { "Danh sách đại lý", "Thêm đại lý mới", "Tìm kiếm đại lý" };
    auto agent_menu_options = MenuOption::Vertical();
    agent_menu_options.on_change = [&] {
        if (selected_agent == 0) content_selector = 7; // ListAgents
        if (selected_agent == 1) content_selector = 8; // AddAgent
        if (selected_agent == 2) content_selector = 9; // SearchAgent
    };
    auto agent_menu = Menu(&agent_menu_entries, &selected_agent, agent_menu_options);
    auto back_button_agent = Button("       < Quay lại", [&] { 
        menu_selector = 0; 
        content_selector = 0; 
    });
    auto agent_menu_group = Container::Vertical({ agent_menu, back_button_agent });
    auto agent_menu_renderer = Renderer(agent_menu_group, [&] {
        return vbox({
            agent_menu->Render() | flex ,
            separatorLight(),
            back_button_agent->Render()
        }) | size(HEIGHT, EQUAL, 40) |flex;
    });

    // --- TẠO CÁC CONTAINER TAB ---

    // Container cho BÊN TRÁI (Menus)
    auto left_pane_container = Container::Tab(
        {
            main_menu,              // index 0
            flight_menu_renderer,   // index 1
            instance_menu_renderer, // index 2
            agent_menu_renderer,    // index 3
        },
        &menu_selector // Biến int này điều khiển menu nào đang active
    );

    // Container cho BÊN PHẢI (Content)
auto right_pane_container = Container::Tab(
{
        none_screen,                  // index 0

        flight_list.container,        // index 1
        Renderer(add_flight_screen.container, [&] { // index 2
          return add_flight_screen.Render();
        }),
        search_flight_screen,         // index 3

        flight_instance_list.container,        // index 4
        Renderer(add_flight_instance_screen.container, [&] { // index 5
                return add_flight_instance_screen.Render();
        }), 
        search_instance_screen,      // index 6

        list_agents_screen,          // index 7
        add_agent_screen,            // index 8
        search_agent_screen,         // index 9
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

    selected_main = -1;// trạng thái ban đầu

    screen.Loop(main_renderer);
}