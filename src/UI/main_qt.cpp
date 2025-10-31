#include <QApplication>
#include "Application.h" 

// 1. Include các manager từ core logic của bạn
#include "core/AccountManager.h" 
#include "core/FlightManager.h"  // <--- Đảm bảo tên file đúng (số ít)
#include "core/BookingManager.h"
#include "core/ReportManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // QSS (Giữ nguyên)
    QString qss =
        "QWidget { background: #F7FAFF; color: #333; } " 
        "QMainWindow{background:#F7FAFF;} "
        "QFrame#Sidebar{background:#0E3B7C;} "
        "QLabel#AppTitle{color:#123B7A;font-weight:700;font-size:24px;} "
        "QFrame#LoginBox{border: 1px solid #C9D6EB; border-radius: 8px; background: white; color: #333;}"
        "QPushButton{border:1px solid #7AA0D4;border-radius:8px;padding:8px 14px;background:#5B86C6;color:white;font-weight:600;}"
        "QPushButton:hover{background:#6B97D8;} "
        "QLineEdit{border:1px solid #C9D6EB;border-radius:8px;padding:6px 10px;background:white; color: #333;}"
        "QLineEdit::placeholder { color: #999; }" 
        "QListWidget{border:none;color:white;background:transparent;}"
        "QListWidget::item{padding:8px 10px;}"
        "QListWidget::item:selected{background:#1C4E99;border-radius:6px;} "
        "QTableView { border: 1px solid #C9D6EB; gridline-color: #E0E7F1; }"
        "QHeaderView::section { background-color: #EBF1F9; padding: 8px; border: none; font-weight: 600; color: #123B7A; }"
        ;
    app.setStyleSheet(qss);

    // --- LOGIC KHỞI ĐỘNG ---

    // 2. Tạo các Manager logic
    // (Lưu ý: bạn cần cung cấp đúng đường dẫn file cho constructor)
    AccountManager accountManager("data/admins.txt", "data/agents.txt");
    FlightManager flightManager("data/flights.txt", "data/flight_instances.txt");
    BookingManager bookingManager("data/bookings.txt", nullptr); // (Tạm thời truyền nullptr cho FlightRule)
    ReportManager reportManager(accountManager, bookingManager);

    // 3. Tạo cửa sổ Application và "truyền" các manager vào
    Application w(&accountManager, &flightManager, &bookingManager, &reportManager);

    w.show();      
    return app.exec();
}

