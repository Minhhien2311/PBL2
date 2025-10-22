#include "C:/PBL2/include/core/BookingManager.h"
#include "C:/PBL2/include/core/FlightManager.h"
#include "C:/PBL2/include/entities/FlightInstance.h" // Cần để gọi bookSeats/releaseSeats
#include "C:/PBL2/include/utils/Time.h"             // Cần để lấy ngày giờ
#include <fstream>
#include <string>

// --- Constructor ---
BookingManager::BookingManager(const std::string& bookingsFilePath, const std::string& ticketsFilePath) {
    this->loadBookingsFromFile(bookingsFilePath);
    this->loadTicketsFromFile(ticketsFilePath);
}

// --- Hàm trợ giúp nội bộ: Tải dữ liệu ---

void BookingManager::loadBookingsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // Dùng fromRecordLine của Booking đã được đơn giản hóa
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
                // Dùng fromRecordLine của Ticket đã được đơn giản hóa
                Ticket ticketOnStack = Ticket::fromRecordLine(line);
                this->allTickets.push_back(new Ticket(ticketOnStack));
            }
        }
        file.close();
    }
}

// --- Chức năng Lưu trữ (Persistence) ---

bool BookingManager::saveDataToFiles(const std::string& bookingsFilePath, const std::string& ticketsFilePath) const {
    // 1. Lưu danh sách Booking (lịch sử)
    std::ofstream bookingsFile(bookingsFilePath);
    if (!bookingsFile.is_open()) return false;
    for (size_t i = 0; i < allBookings.size(); ++i) {
        bookingsFile << allBookings[i]->toRecordLine() << "\n";
    }
    bookingsFile.close();

    // 2. Lưu danh sách Ticket (vé có trạng thái)
    std::ofstream ticketsFile(ticketsFilePath);
    if (!ticketsFile.is_open()) return false;
    for (size_t i = 0; i < allTickets.size(); ++i) {
        ticketsFile << allTickets[i]->toRecordLine() << "\n";
    }
    ticketsFile.close();

    return true;
}

// --- NGHIỆP VỤ 1: BÁN VÉ ---

Ticket* BookingManager::createBookingAndIssueTicket( FlightManager& flightManager,
                                                     const std::string& flightInstanceId,
                                                     const std::string& passengerId,
                                                     BookingClass bookingClass,
                                                     double baseFare)
{
    // 1. Kiểm tra đầu vào
    if (flightInstanceId.empty() || passengerId.empty() || baseFare < 0) {
        return nullptr;
    }

    // 2. Tìm "kho hàng" (chuyến bay)
    FlightInstance* instance = flightManager.findInstanceById(flightInstanceId);
    if (instance == nullptr) {
        return nullptr; // Chuyến bay không tồn tại
    }

    // 3. Kiểm tra và TRỪ GHẾ
    SeatClass seatClassToBook = (bookingClass == BookingClass::Economy) 
                                ? SeatClass::Economy 
                                : SeatClass::Business;
    
    // Gọi hàm của FlightInstance để trừ ghế
    if (!instance->bookSeats(seatClassToBook, 1)) {
        return nullptr; // Hết ghế
    }

    // --- Nếu trừ ghế thành công, tiến hành tạo hóa đơn và vé ---

    // 4. TẠO BOOKING (Hóa đơn lịch sử)
    std::string currentDate = utils::Time::formatLocal(utils::Time::nowUtc(), "%Y-%m-%d %H:%M:%S");
    
    // Dùng constructor Booking đã đơn giản hóa (không status, không seatId, không discount)
    Booking* newBooking = new Booking(flightInstanceId, passengerId, currentDate, bookingClass, baseFare);
    this->allBookings.push_back(newBooking);

    // 5. TẠO TICKET (Vé điện tử)
    std::string classStr = (newBooking->getClass() == BookingClass::Economy) ? "Economy" : "Business";

    // Dùng constructor Ticket đã đơn giản hóa
    Ticket* newTicket = new Ticket(
        newBooking->getBookingId(),
        newBooking->getPassengerId(),
        newBooking->getFlightInstanceId(),
        classStr,
        currentDate,
        newBooking->getBaseFare(),
        TicketStatus::Active
    );
    
    this->allTickets.push_back(newTicket);

    // 6. Trả về vé vừa tạo
    return newTicket;
}

// --- NGHIỆP VỤ 2: HỦY VÉ ---

bool BookingManager::cancelTicket(FlightManager& flightManager, const std::string& ticketId) {
    // 1. Tìm vé
    Ticket* ticket = findTicketById(ticketId);
    if (ticket == nullptr) {
        return false; // Không tìm thấy vé
    }

    // 2. Chỉ hủy vé đang 'Active'
    if (ticket->getStatus() != TicketStatus::Active) {
        return false; // Vé đã bị hủy rồi
    }

    // 3. Tìm Booking gốc (để lấy thông tin chuyến bay và hạng vé)
    Booking* booking = findBookingById(ticket->getBookingId());
    if (booking == nullptr) {
        // Lỗi dữ liệu: Có vé mà không có booking
        return false; 
    }

    // 4. HOÀN TRẢ GHẾ
    FlightInstance* instance = flightManager.findInstanceById(booking->getFlightInstanceId());
    if (instance != nullptr) {
        // Xác định hạng ghế để hoàn trả
        SeatClass seatClassToRelease = (booking->getClass() == BookingClass::Economy)
                                       ? SeatClass::Economy
                                       : SeatClass::Business;
        
        // Gọi hàm của FlightInstance để cộng 1 ghế trở lại
        instance->releaseSeats(seatClassToRelease, 1);
    }
    // (Nếu instance == nullptr, tức chuyến bay đã bị xóa, ta vẫn cho hủy vé)

    // 5. Cập nhật trạng thái Ticket -> Cancelled
    ticket->setStatus(TicketStatus::Cancelled);
    
    return true;
}


// --- CÁC HÀM TÌM KIẾM ---

Booking* BookingManager::findBookingById(const std::string& bookingId) {
    for (size_t i = 0; i < allBookings.size(); ++i) {
        if (allBookings[i]->getBookingId() == bookingId) {
            return allBookings[i];
        }
    }
    return nullptr;
}

Ticket* BookingManager::findTicketById(const std::string& ticketId) {
    for (size_t i = 0; i < allTickets.size(); ++i) {
        if (allTickets[i]->getTicketNumber() == ticketId) {
            return allTickets[i];
        }
    }
    return nullptr;
}