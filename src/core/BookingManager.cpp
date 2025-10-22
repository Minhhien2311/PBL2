// #include <fstream>
// #include <string>
// #include "C:/PBL2/include/utils/Time.h"             // Cần để lấy ngày giờ
// #include "C:/PBL2/include/core/BookingManager.h"
// #include "C:/PBL2/include/core/FlightManager.h"     // Cần cho cả create và cancel
// #include "C:/PBL2/include/entities/FlightInstance.h" // Cần để gọi releaseSeats

// // --- Constructor ---
// BookingManager::BookingManager(const std::string& bookingsFilePath, const std::string& ticketsFilePath) {
//     this->loadBookingsFromFile(bookingsFilePath);
//     this->loadTicketsFromFile(ticketsFilePath);
// }

// // --- Hàm trợ giúp nội bộ ---

// void BookingManager::loadBookingsFromFile(const std::string& filePath) {
//     std::ifstream file(filePath);
//     std::string line;
//     if (file.is_open()) {
//         while (std::getline(file, line)) {
//             if (!line.empty()) {
//                 Booking bookingOnStack = Booking::fromRecordLine(line);
//                 this->allBookings.push_back(new Booking(bookingOnStack));
//             }
//         }
//         file.close();
//     }
// }

// void BookingManager::loadTicketsFromFile(const std::string& filePath) {
//     std::ifstream file(filePath);
//     std::string line;
//     if (file.is_open()) {
//         while (std::getline(file, line)) {
//             if (!line.empty()) {
//                 Ticket ticketOnStack = Ticket::fromRecordLine(line);
//                 this->allTickets.push_back(new Ticket(ticketOnStack));
//             }
//         }
//         file.close();
//     }
// }

// // --- Chức năng Lưu trữ (Persistence) ---

// bool BookingManager::saveDataToFiles(const std::string& bookingsFilePath, const std::string& ticketsFilePath) const {
//     // 1. Lưu danh sách Booking
//     std::ofstream bookingsFile(bookingsFilePath);
//     if (!bookingsFile.is_open()) return false;
//     for (size_t i = 0; i < allBookings.size(); ++i) {
//         if (i < allBookings.size()) {
//             bookingsFile << allBookings[i]->toRecordLine() << "\n";
//         }
//     }
//     bookingsFile.close();

//     // 2. Lưu danh sách Ticket
//     std::ofstream ticketsFile(ticketsFilePath);
//     if (!ticketsFile.is_open()) return false;
//     for (std::size_t i = 0; i < allTickets.size(); ++i) {
//         ticketsFile << allTickets[i]->toRecordLine() << "\n";
//     }
//     ticketsFile.close();

//     return true;
// }

// // --- Nghiệp vụ chính ---

// Booking* BookingManager::createNewBooking( FlightManager& flightManager,
//                                            const std::string& flightInstanceId,
//                                            const std::string& passengerId,
//                                            BookingClass bookingClass,
//                                            double baseFare)
// {
//     // 1. Kiểm tra đầu vào
//     if (flightInstanceId.empty() || passengerId.empty() || baseFare < 0) {
//         return nullptr;
//     }

//     // 2. Tìm chuyến bay
//     FlightInstance* instance = flightManager.findInstanceById(flightInstanceId);
//     if (instance == nullptr) {
//         return nullptr; // Chuyến bay không tồn tại
//     }

//     // 3. Kiểm tra và trừ ghế
//     SeatClass seatClassToBook = (bookingClass == BookingClass::Economy) 
//                                 ? SeatClass::Economy 
//                                 : SeatClass::Business;
    
//     if (!instance->bookSeats(seatClassToBook, 1)) {
//         return nullptr; // Hết ghế
//     }

//     // 4. Tạo đối tượng Booking
//     std::string currentDate = utils::Time::formatLocal(utils::Time::nowUtc(), "%Y-%m-%d %H:%M:%S");
//     Booking* newBooking = new Booking(flightInstanceId, passengerId, currentDate, bookingClass, baseFare);
//     this->allBookings.push_back(newBooking);

//     // 5. Trả về
//     return newBooking;
// }

// /*
// Xuất vé từ một Booking đã 'Confirmed'.
//  */
// Ticket* BookingManager::issueTicketForBooking(const std::string& bookingId) {
//     // 1. Tìm booking
//     Booking* booking = findBookingById(bookingId);
//     if (booking == nullptr) {
//         return nullptr; // Không tìm thấy booking
//     }

//     // 2. Kiểm tra trạng thái: Chỉ xuất vé khi đang 'Confirmed'
//     if (booking->getStatus() != BookingStatus::Confirmed) {
//         return nullptr; // Không thể xuất vé từ trạng thái này
//     }

//     // 3. Lấy ngày giờ xuất vé
//     std::string issueDateTime = utils::Time::formatLocal(utils::Time::nowUtc(), "%Y-%m-%d %H:%M:%S");

//     // 4. Chuyển đổi BookingClass (enum) sang chuỗi
//     std::string classStr = (booking->getClass() == BookingClass::Economy) ? "Economy" : "Business";

//     // 5. Tạo đối tượng Ticket mới (là "ảnh chụp" của booking tại thời điểm xuất)
//     Ticket* newTicket = new Ticket(
//         booking->getBookingId(),
//         booking->getPassengerId(),
//         booking->getFlightInstanceId(),
//         booking->getSeatId(),
//         classStr,
//         issueDateTime,
//         booking->getBaseFare(),
//         0.0, // Giả sử discount = 0 (lấy từ booking->getDiscount() nếu có)
//         booking->getTotalAmount(),
//         TicketStatus::Active // Vé mới luôn ở trạng thái Active
//     );

//     // 6. Thêm vé mới vào danh sách quản lý
//     this->allTickets.push_back(newTicket);

//     // 7. Cập nhật trạng thái booking sang 'Issued'
//     booking->setStatus(BookingStatus::Issued);

//     // 8. Trả về vé vừa tạo
//     return newTicket;
// }

// /*
// Hủy một Booking
//  */
// bool BookingManager::cancelBooking(FlightManager& flightManager, const std::string& bookingId, const std::string& reason) {
//     // 1. Tìm booking
//     Booking* booking = findBookingById(bookingId);
//     if (booking == nullptr) {
//         return false; // Không tìm thấy
//     }

//     // 2. Kiểm tra xem booking có được phép hủy không
//     // (Logic này nằm trong chính đối tượng Booking)
//     if (!booking->canBeCancelled()) {
//         // Không thể hủy (ví dụ: đã bay, đã hủy trước đó, hoặc đã Issued)
//         return false;
//     }

//     // 3. NGHIỆP VỤ QUAN TRỌNG: Hoàn trả ghế
//     FlightInstance* instance = flightManager.findInstanceById(booking->getFlightInstanceId());
//     if (instance != nullptr) {
//         // Xác định hạng ghế để hoàn trả
//         SeatClass seatClassToRelease = (booking->getClass() == BookingClass::Economy)
//                                        ? SeatClass::Economy
//                                        : SeatClass::Business;
        
//         // Gọi hàm của FlightInstance để cộng 1 ghế trở lại
//         instance->releaseSeats(seatClassToRelease, 1);
//     }
//     // (Nếu instance == nullptr, tức là chuyến bay đã bị xóa?
//     // Vẫn cho phép hủy booking nhưng không hoàn trả ghế được)

//     // 4. Cập nhật trạng thái Booking
//     booking->setStatus(BookingStatus::Cancelled);
//     booking->setCancelReason(reason);

//     return true;
// }


// // --- CÁC HÀM TÌM KIẾM ---

// Booking* BookingManager::findBookingById(const std::string& bookingId) {
//     for (size_t i = 0; i < allBookings.size(); ++i) {
//         if (allBookings[i]->getBookingId() == bookingId) {
//             return allBookings[i];
//         }
//     }
//     return nullptr;
// }

// Ticket* BookingManager::findTicketById(const std::string& ticketId) {
//     for (size_t i = 0; i < allTickets.size(); ++i) {
//         if (allTickets[i]->getTicketNumber() == ticketId) {
//             return allTickets[i];
//         }
//     }
//     return nullptr;
// }