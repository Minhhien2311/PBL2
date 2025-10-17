#pragma once

// Enum định nghĩa tất cả các "màn hình" có thể có trong ứng dụng
enum class ApplicationScreen {
    Login,
    //Admin_Interface
    AdminMenu,
    // AdminAddFlight,
    // AdminRemoveFlight,
    // AdminAddAgent,
    // AdminRemoveAgent,
    // Agent_Interface
    AgentMenu,
    //
    Exit, // Trạng thái để thoát chương trình
};

// Biến toàn cục để theo dõi màn hình hiện tại
// Dùng 'extern' để báo cho các file .cpp khác biết rằng biến này tồn tại ở đâu đó
extern ApplicationScreen current_screen;