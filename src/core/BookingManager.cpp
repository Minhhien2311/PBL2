// #include "C:/PBL2/include/core/BookingManager.h"
// #include "C:/PBL2/include/core/FlightManager.h"         // Cần để tìm FlightInstance
// #include "C:/PBL2/include/entities/FlightInstance.h"    // Cần để gọi bookSeats/releaseSeats
// #include "C:/PBL2/include/utils/Time.h"                 // Cần để lấy ngày giờ
// #include "C:/PBL2/include/entities/FlightRule.h"        // Cần để kiểm tra luật
// #include <fstream>
// #include <string>
// #include <chrono> 

// // --- Constructor  ---
// BookingManager::BookingManager(const std::string& bookingsFilePath, FlightRule* rule) 
//     : currentRule(rule) 
// { 
//     this->loadBookingsFromFile(bookingsFilePath);

//     // <<< THÊM MỚI: Xây dựng bảng băm >>>
//     this->buildBookingIdTable();
// }

// // --- Destructor  ---
// BookingManager::~BookingManager() {
//     for (int i = 0; i < allBookings.size(); i++) {
//         delete allBookings[i];
//     }
// }


// // --- loadBookingsFromFile  ---
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

// // <<< THÊM MỚI: Xây dựng bảng băm >>>
// void BookingManager::buildBookingIdTable() {
//     for (int i = 0; i < allBookings.size(); ++i) {
//         if (allBookings[i] != nullptr) {
//             bookingIdTable.insert(allBookings[i]->getBookingId(), allBookings[i]);
//         }
//     }
// }

// // --- saveDataToFiles  ---
// bool BookingManager::saveDataToFiles(const std::string& bookingsFilePath) const { 
//     std::ofstream bookingsFile(bookingsFilePath);
//     if (!bookingsFile.is_open()) return false;
//     for (size_t i = 0; i < allBookings.size(); ++i) {
//         bookingsFile << allBookings[i]->toRecordLine() << "\n";
//     }
//     bookingsFile.close();
//     return true;
// }

// // --- NGHIỆP VỤ 1: BÁN VÉ (TẠO BOOKING) ---
// // <<< THAY ĐỔI: Cập nhật HashTable >>>
// Booking* BookingManager::createNewBooking( FlightManager& flightManager,
//                                            const std::string& flightInstanceId,
//                                            const std::string& passengerId,
//                                            BookingClass bookingClass,
//                                            double baseFare)
// {
//     // ... (logic kiểm tra và trừ ghế giữ nguyên) ...
//     if (flightInstanceId.empty() || passengerId.empty() || baseFare < 0) return nullptr;
//     FlightInstance* instance = flightManager.findInstanceById(flightInstanceId);
//     if (instance == nullptr) return nullptr; 
//     SeatClass seatClassToBook = (bookingClass == BookingClass::Economy) 
//                                 ? SeatClass::Economy 
//                                 : SeatClass::Business;
//     if (!instance->bookSeats(seatClassToBook, 1)) return nullptr; 

//     std::string currentDate = utils::Time::formatLocal(utils::Time::nowUtc(), "%Y-%m-%d %H:%M:%S");
//     Booking* newBooking = new Booking(flightInstanceId, passengerId, currentDate, bookingClass, baseFare, BookingStatus::Issued);
    
//     // Thêm vào DynamicArray
//     this->allBookings.push_back(newBooking);
    
//     // <<< THÊM MỚI: Cập nhật bảng băm >>>
//     this->bookingIdTable.insert(newBooking->getBookingId(), newBooking);
    
//     return newBooking;
// }

// // --- NGHIỆP VỤ 2: HỦY BOOKING (ĐÃ CẬP NHẬT LOGIC THỜI GIAN) ---

// bool BookingManager::cancelBooking(FlightManager& flightManager, const std::string& bookingId) {
    
//     // BƯỚC 1 : Tìm booking để hủy
//     Booking* booking = findBookingById(bookingId);
//     // TH không tìm thấy booking
//     if (booking == nullptr) {
//         return false; 
//     }
//     // Trường hợp booking đã bị hủy
//     if (booking->getStatus() != BookingStatus::Issued) {
//         return false; 
//     }

//     // BƯỚC 2 : Kiểm tra luật
//     // Không tìm thấy luật
//     if (currentRule == nullptr) {
//         return false; 
//     }
//     // Nếu hệ thống không cho hủy
//     if (!currentRule->isCancelAllowed()) {
//         return false; 
//     }

//     // BƯỚC 3: KIỂM TRA THỜI GIAN
//     FlightInstance* instance = flightManager.findInstanceById(booking->getFlightInstanceId());
//     if (instance == nullptr) {
//         // Chuyến bay đã bị xóa, vẫn cho hủy
//     } else {
//         // --- BẮT ĐẦU SỬA ---
//         // Lấy thời gian bay (định dạng mới)
//         std::string depDate = instance->getDepartureDate();
//         std::string depTime = instance->getDepartureTime();
        
//         // Chuyển đổi sang time_point bằng hàm helper mới
//         // (Hàm này đã có trong utils/Time.cpp)
//         auto departureTime = utils::Time::fromDmYHm(depDate, depTime);
//         // --- KẾT THÚC SỬA ---

//         auto now = utils::Time::nowUtc(); // Lấy thời gian hiện tại

//         // Tính số giờ còn lại (logic này giờ đã đúng, không còn lỗi timezone)
//         auto duration = std::chrono::duration_cast<std::chrono::hours>(departureTime - now);
//         int hoursUntilDeparture = duration.count();

//         // Áp dụng quy tắc
//         if (!currentRule->isCancellable(hoursUntilDeparture)) {
//             return false; // Hủy quá sát giờ bay hoặc chuyến bay đã cất cánh
//         }

//         // NẾU HỢP LỆ -> HOÀN TRẢ GHẾ
//         SeatClass seatClassToRelease = (booking->getClass() == BookingClass::Economy)
//                                        ? SeatClass::Economy
//                                        : SeatClass::Business;
//         instance->releaseSeats(seatClassToRelease, 1);
//     }

//     // BƯỚC 4 
//     booking->setStatus(BookingStatus::Cancelled);
    
//     return true; // Hủy thành công
// }


// // --- CÁC HÀM TÌM KIẾM  ---

// Booking* BookingManager::findBookingById(const std::string& bookingId) {
//     for (size_t i = 0; i < allBookings.size(); ++i) {
//         if (allBookings[i]->getBookingId() == bookingId) {
//             return allBookings[i];
//         }
//     }
//     return nullptr; // Không tìm thấy
// }

// // --- Lấy dữ liệu  ---

// const DynamicArray<Booking*>& BookingManager::getAllBookings() const {
//     return this->allBookings;
// }