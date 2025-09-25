#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <string>
#include "../entities/Account.h" 
// NOTE: Header này hiện KHÔNG dùng tới Account.
// -> Có thể bỏ include này khỏi .h để giảm phụ thuộc; nếu cần thì include trong .cpp.

class MenuManager {
public:
    // [UI ENTRY] Menu chính của ứng dụng.
    // In ra các nhóm chức năng top-level (Đăng nhập/Quản lý chuyến/Đặt vé/Báo cáo/Cài đặt/Thoát).
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : clearScreen() -> showBanner() -> liệt kê mục -> gọi getChoice(min,max).
    static void showMainMenu();

    // [UI ENTRY] Menu dành riêng cho Admin sau khi đăng nhập.
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : Hiển thị các tác vụ Admin (quản lý chuyến, quy tắc, khuyến mãi, người dùng, báo cáo...).
    static void showAdminMenu();

    // [UI ENTRY] Menu dành cho Agent sau khi đăng nhập.
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : Tác vụ của Agent (tìm chuyến, giữ ghế, xuất/đổi/hủy vé, tra cứu PNR...).
    static void showAgentMenu();

    // [UI ENTRY] Menu đăng nhập/đăng xuất/đổi mật khẩu.
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : In lựa chọn -> nhận username/password (getInput) -> ủy quyền cho AuthService (ở lớp khác).
    // LƯU Ý : KHÔNG kiểm tra mật khẩu tại đây; chỉ giao tiếp UI. Không in thông tin nhạy cảm ra màn hình.
    static void showLoginMenu();

    // [UI ENTRY] Menu quản lý chuyến bay.
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : CRUD Flight/FlightInstance/Seat; điều hướng sang controller/service tương ứng.
    // LƯU Ý : Không nhúng nghiệp vụ (pricing/seat-inventory) trong lớp Menu.
    static void showFlightManagementMenu();

    // [UI ENTRY] Menu quản lý đặt chỗ/PNR/vé.
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : Tìm chuyến -> tạo Booking (HOLD) -> Issue/Cancel/Change/Upgrade.
    // LƯU Ý : Hỏi confirm trước khi hủy/void; in thông tin PNR/Ticket gọn, có mask dữ liệu nhạy cảm.
    static void showBookingManagementMenu();

    // [UI ENTRY] Menu báo cáo.
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : Chọn loại báo cáo (doanh thu theo ngày/đại lý/chuyến; log hoạt động...).
    // LƯU Ý : Báo cáo thường là dữ liệu tính ra; Menu chỉ gọi ReportingService rồi hiển thị kết quả.
    static void showReportMenu();

    // [UI ENTRY] Menu quản lý người dùng (Admin).
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : Tạo/sửa/xóa tài khoản, reset mật khẩu, khóa/mở khóa.
    // LƯU Ý : Yêu cầu quyền Admin; kiểm tra role trước khi cho thao tác.
    static void showUserManagementMenu();
    
    // [UTIL UI] Xóa màn hình.
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : Ở .cpp: trên Windows có thể dùng system("cls"); POSIX: system("clear") hoặc ANSI "\x1B[2J\x1B[H".
    // LƯU Ý : Tránh system() nếu có thể; ưu tiên mã ANSI để portable và an toàn hơn.
    static void clearScreen();

    // [UTIL UI] In banner/logo ứng dụng.
    // INPUT : (none)
    // OUTPUT: (none)
    // LOGIC : In ASCII art/tiêu đề, có thể kèm phiên bản, người đang đăng nhập.
    static void showBanner();

    // [UTIL UI] In thông báo lỗi nổi bật.
    // INPUT : message (nội dung lỗi)
    // OUTPUT: (none)
    // LƯU Ý : Dùng một chuẩn định dạng thống nhất (ví dụ thêm tiền tố "[ERROR]" và màu nếu terminal hỗ trợ).
    static void showErrorMessage(const std::string& message);

    // [UTIL UI] In thông báo thành công.
    // INPUT : message
    // OUTPUT: (none)
    // LƯU Ý : Giữ thông điệp ngắn gọn; có thể kèm “nhấn phím bất kỳ để tiếp tục”.
    static void showSuccessMessage(const std::string& message);

    // [UTIL UI] In thông tin chung/ghi chú.
    // INPUT : message
    // OUTPUT: (none)
    // LƯU Ý : Khác với lỗi/thành công về màu/tiền tố (ví dụ "[INFO]").
    static void showInfoMessage(const std::string& message);
    
    // [INPUT] Đọc lựa chọn số nguyên trong khoảng [min, max].
    // INPUT : min, max (ranh giới hợp lệ)
    // RETURN: int (giá trị hợp lệ trong [min, max])
    // LOGIC : lặp đến khi nhập đúng; nếu nhập không phải số -> xóa trạng thái lỗi cin + bỏ phần dư dòng.
    // LƯU Ý : Tránh treo vòng lặp với input rác; in lại prompt khi sai.
    static int getChoice(int min, int max);

    // [INPUT] Đọc chuỗi có khoảng trắng.
    // INPUT : prompt (nhãn hỏi)
    // RETURN: std::string (toàn bộ dòng người dùng nhập, có thể cần trim)
    // LOGIC : In prompt -> std::getline(cin, s).
    // LƯU Ý : Nếu trước đó dùng >>, nhớ xử lý newline còn thừa (cin.ignore).
    static std::string getInput(const std::string& prompt);

    // [INPUT] Đọc số thực (double).
    // INPUT : prompt
    // RETURN: double (giá trị hợp lệ)
    // LOGIC : lặp đến khi parse được double; xử lý locale nếu cần.
    // LƯU Ý : Báo lỗi rõ ràng khi người dùng gõ chữ; không chấp nhận NaN/inf.
    static double getDoubleInput(const std::string& prompt);

    // [INPUT] Đọc số nguyên.
    // INPUT : prompt
    // RETURN: int (giá trị hợp lệ)
    // LOGIC : như getDoubleInput nhưng dùng long long/int kiểm soát tràn.
    // LƯU Ý : Có thể giới hạn miền (vd. >=0) tùy ngữ cảnh ở nơi gọi.
    static int getIntegerInput(const std::string& prompt);
};

#endif // MENUMANAGER_H
