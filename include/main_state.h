#pragma once

// Enum định nghĩa tất cả các "màn hình chính" có thể có trong ứng dụng
enum class ApplicationScreen {
    //Login_Interface
    Login,
    //Admin_Interface
    AdminMenu,
    //Agent_Interface
    AgentMenu,
    //Thoat chuong trinh
    Exit,
};

// Biến toàn cục để theo dõi màn hình hiện tại
// Dùng 'extern' để báo cho các file .cpp khác biết rằng biến này tồn tại ở đâu đó
extern ApplicationScreen current_screen;