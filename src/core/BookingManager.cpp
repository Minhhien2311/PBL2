#include <fstream>
#include <string>
#include "C:/PBL2/include/utils/Time.h" // Sẽ cần để lấy ngày giờ khi xuất vé
#include "C:/PBL2/include/core/BookingManager.h"
#include "C:/PBL2/include/core/FlightManager.h"

// --- Constructor ---
BookingManager::BookingManager(const std::string& bookingsFilePath, const std::string& ticketsFilePath) {
    this->loadBookingsFromFile(bookingsFilePath);
    this->loadTicketsFromFile(ticketsFilePath);
}

// --- Hàm trợ giúp nội bộ ---

void BookingManager::loadBookingsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // <<< THAY ĐỔI: Tạo đối tượng trên heap bằng 'new'
                Booking bookingOnStack = Booking::fromRecordLine(line);
                this->allBookings.push_back(new Booking(bookingOnStack));
            }
        }
        file.close();
    }
}

void BookingManager::loadTicketsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // <<< THAY ĐỔI: Tạo đối tượng trên heap bằng 'new'
                Ticket ticketOnStack = Ticket::fromRecordLine(line);
                this->allTickets.push_back(new Ticket(ticketOnStack));
            }
        }
        file.close();
    }
}

// --- Chức năng Lưu trữ (Persistence) ---

bool BookingManager::saveDataToFiles(const std::string& bookingsFilePath, const std::string& ticketsFilePath) const {
    // Lưu danh sách Booking
    std::ofstream bookingsFile(bookingsFilePath);
    if (!bookingsFile.is_open()) return false;
    for (size_t i = 0; i < allBookings.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        bookingsFile << allBookings[i]->toRecordLine() << "\n";
    }
    bookingsFile.close();

    // Lưu danh sách Ticket
    std::ofstream ticketsFile(ticketsFilePath);
    if (!ticketsFile.is_open()) return false;
    for (size_t i = 0; i < allTickets.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        ticketsFile << allTickets[i]->toRecordLine() << "\n";
    }
    ticketsFile.close();

    return true;
}

// --- Nghiệp vụ chính ---

Booking* BookingManager::createNewBooking( FlightManager& flightManager,
                                           const std::string& pnr,
                                           const std::string& agentId,
                                           const std::string& flightInstanceId,
                                           const std::string& passengerId,
                                           BookingClass bookingClass,
                                           double baseFare)
{
    // 1. Kiểm tra đầu vào ("Guard Clauses")
    if (pnr.empty() || agentId.empty() || flightInstanceId.empty() || passengerId.empty() || baseFare < 0) {
        return nullptr; // Dữ liệu không hợp lệ
    }

    // 2. Tìm chuyến bay tương ứng
    FlightInstance* instance = flightManager.findInstanceById(flightInstanceId);
    if (instance == nullptr) {
        return nullptr; // Chuyến bay không tồn tại
    }

    // 3. Kiểm tra và đặt chỗ
    SeatClass seatClassToBook = (bookingClass == BookingClass::Economy) ? SeatClass::Economy : SeatClass::Business;
    
    // Gọi hàm bookSeats của FlightInstance. Hàm này sẽ tự động trừ số ghế nếu còn.
    if (!instance->bookSeats(seatClassToBook, 1)) {
        return nullptr; // Hết ghế
    }

    // 4. Nếu đặt chỗ thành công, tạo đối tượng Booking
    std::string currentDate = utils::Time::formatLocal(utils::Time::nowUtc(), "%Y-%m-%d %H:%M:%S");
    
    // <<< THAY ĐỔI: Tạo con trỏ mới bằng 'new'
    Booking* newBooking = new Booking(pnr, agentId, flightInstanceId, passengerId, currentDate, bookingClass, baseFare);
    this->allBookings.push_back(newBooking);

    // 5. Trả về con trỏ vừa tạo
    return newBooking;
}

// CÁC HÀM TÌM KIẾM

Booking* BookingManager::findBookingById(const std::string& bookingId) {
    for (size_t i = 0; i < allBookings.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        if (allBookings[i]->getBookingId() == bookingId) {
            // <<< THAY ĐỔI: Trả về con trỏ trực tiếp
            return allBookings[i];
        }
    }
    return nullptr; // Không tìm thấy
}

Booking* BookingManager::findBookingByPNR(const std::string& pnr) {
    for (size_t i = 0; i < allBookings.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        if (allBookings[i]->getPNR() == pnr) {
            // <<< THAY ĐỔI: Trả về con trỏ trực tiếp
            return allBookings[i];
        }
    }
    return nullptr; // Không tìm thấy
}

Ticket* BookingManager::findTicketById(const std::string& ticketId) {
    for (size_t i = 0; i < allTickets.size(); ++i) {
        // <<< THAY ĐỔI: Dùng toán tử -> cho con trỏ
        if (allTickets[i]->getTicketNumber() == ticketId) {
            // <<< THAY ĐỔI: Trả về con trỏ trực tiếp
            return allTickets[i];
        }
    }
    return nullptr; // Không tìm thấy
}
