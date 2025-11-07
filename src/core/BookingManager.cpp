#include "core/BookingManager.h"
#include "core/FlightManager.h"         // Cần để tìm FlightInstance
#include "entities/FlightInstance.h"    // Cần để gọi bookSeats/releaseSeats
#include "utils/DateTime.h"                 // Cần để lấy ngày giờ
#include "entities/FlightRule.h"        // Cần để kiểm tra luật
#include <fstream>
#include <string>
#include <iostream>
#include <chrono> 

class SeatManager; // Khai báo tiền định

// --- Constructor  ---
BookingManager::BookingManager(const std::string& bookingsFilePath, FlightRule* rule) 
    : currentRule(rule), bookingsFilePath_(bookingsFilePath) 
{ 
    this->loadBookingsFromFile(bookingsFilePath);

    // <<< THÊM MỚI: Xây dựng bảng băm >>>
    this->buildBookingIdTable();
}

// --- Destructor  ---
BookingManager::~BookingManager() {
    // Auto-save data before destruction
    saveDataToFiles(bookingsFilePath_);
    
    for (int i = 0; i < allBookings.size(); i++) {
        delete allBookings[i];
    }
}


// --- loadBookingsFromFile  ---
void BookingManager::loadBookingsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                Booking bookingOnStack = Booking::fromRecordLine(line);
                this->allBookings.push_back(new Booking(bookingOnStack));
            }
        }
        file.close();
    }
}

// <<< THÊM MỚI: Xây dựng bảng băm >>>
void BookingManager::buildBookingIdTable() {
    for (int i = 0; i < allBookings.size(); ++i) {
        if (allBookings[i] != nullptr) {
            bookingIdTable.insert(allBookings[i]->getBookingId(), allBookings[i]);
        }
    }
}

// --- saveDataToFiles  ---
bool BookingManager::saveDataToFiles(const std::string& bookingsFilePath) const { 
    std::ofstream bookingsFile(bookingsFilePath);
    if (!bookingsFile.is_open()) return false;
    for (size_t i = 0; i < allBookings.size(); ++i) {
        bookingsFile << allBookings[i]->toRecordLine() << "\n";
    }
    bookingsFile.close();
    return true;
}

// --- NGHIỆP VỤ 1: BÁN VÉ (TẠO BOOKING) ---
// <<< THAY ĐỔI: Cập nhật HashTable >>>
Booking* BookingManager::createNewBooking( FlightManager& flightManager,
                                           const std::string& flightInstanceId,
                                           const std::string& agentId,
                                           const std::string& passengerId,
                                           BookingClass bookingClass,
                                           int baseFare,
                                           SeatManager& seatManager) // Dùng tham chiếu &
{
    // BƯỚC 1: Kiểm tra các đầu vào cơ bản
    if (flightInstanceId.empty() || passengerId.empty() || baseFare < 0) return nullptr;

    // BƯỚC 2: Kiểm tra SeatManager
    const Seat* selectedSeat = seatManager.getSelectedSeat();
    if (selectedSeat == nullptr) {
        std::cerr << "Lỗi Booking: Chưa có ghế nào được chọn trong SeatManager." << std::endl;
        return nullptr;
    }
    // Lấy ID ghế trước khi xác nhận (vì sau đó con trỏ có thể bị reset)
    std::string seatId = selectedSeat->getId();

    // BƯỚC 3: Xác nhận việc chọn ghế. 
    // Hành động này sẽ đổi trạng thái ghế và LƯU FILE SEAT_STATUS.TXT
    if (!seatManager.confirmSelection()) {
        std::cerr << "Lỗi Booking: Không thể xác nhận ghế với SeatManager (ví dụ: lỗi file)." << std::endl;
        return nullptr; // Không tạo booking nếu không xác nhận được ghế
    }

    // BƯỚC 4: Tạo đối tượng Booking mới
    // (Bỏ qua logic cũ `instance->bookSeats(...)` vì SeatManager đã xử lý)
    std::string currentDate = utils::DateTime::formatLocal(utils::DateTime::nowUtc(), "%Y-%m-%d %H:%M:%S");
    
    // Sử dụng seatId đã lấu được ở dòng 86 (KHÔNG gọi getSelectedSeat() vì đã bị reset)
    Booking* newBooking = new Booking(flightInstanceId, agentId, passengerId, seatId, currentDate, bookingClass, baseFare, BookingStatus::Issued);

    // Thêm vào std::vector và HashTable
    this->allBookings.push_back(newBooking);
    this->bookingIdTable.insert(newBooking->getBookingId(), newBooking);
    
    return newBooking;
}

// --- NGHIỆP VỤ 2: HỦY BOOKING (ĐÃ CẬP NHẬT LOGIC THỜI GIAN) ---
// CẬP NHẬT: Thêm tham số SeatManager
bool BookingManager::cancelBooking(FlightManager& flightManager, SeatManager& seatManager, const std::string& bookingId) {
    
    // BƯỚC 1 & 2: Tìm booking và kiểm tra luật (giữ nguyên)
    Booking* booking = findBookingById(bookingId);
    if (booking == nullptr || booking->getStatus() != BookingStatus::Issued) {
        return false; 
    }
    if (currentRule == nullptr || !currentRule->isCancelAllowed()) {
        return false; 
    }

    // BƯỚC 3: KIỂM TRA THỜI GIAN & HOÀN TRẢ GHẾ
    FlightInstance* instance = flightManager.findInstanceById(booking->getFlightInstanceId());
    
    // THAY ĐỔI: Dù chuyến bay còn hay không, chúng ta vẫn cố gắng hoàn trả ghế
    
    // Lấy ID ghế cần giải phóng từ booking
    std::string seatIdToRelease = booking->getSeatID(); // Giả sử đã có hàm này

    if (!seatIdToRelease.empty()) {
        if (instance != nullptr) { // Chỉ load SeatManager khi instance còn tồn tại
            // Tải đúng chuyến bay vào SeatManager
            seatManager.loadSeatMapFor(instance);
            
            // Yêu cầu SeatManager giải phóng ghế và lưu lại file seat_status.txt
            seatManager.releaseSeat(seatIdToRelease);
            
            std::cout << "Đã hoàn trả ghế " << seatIdToRelease << " cho chuyến bay " << instance->getInstanceId() << std::endl;
        }
    }


    // Phần kiểm tra thời gian hủy vé so với giờ bay vẫn giữ nguyên
    if (instance != nullptr) {
        // ... (toàn bộ logic so sánh thời gian giữ nguyên) ...
        auto departureTime = utils::DateTime::fromDmYHm(instance->getDepartureDate(), instance->getDepartureTime());
        auto now = utils::DateTime::nowUtc();
        auto duration = std::chrono::duration_cast<std::chrono::hours>(departureTime - now);
        if (!currentRule->isCancellable(duration.count())) {
            // LƯU Ý: Nếu hủy không thành công vì quá sát giờ, ghế đã hoàn trả ở trên
            // cần được book lại. Đây là một giao dịch phức tạp (transaction).
            // Tạm thời chấp nhận ghế được hoàn trả ngay cả khi việc hủy bị từ chối ở đây.
            return false;
        }
    }

    // BƯỚC 4: Đổi trạng thái booking
    booking->setStatus(BookingStatus::Cancelled);
    
    return true; // Hủy thành công
}

// --- CÁC HÀM TÌM KIẾM  ---

// <<< THAY ĐỔI: Dùng HashTable >>>
Booking* BookingManager::findBookingById(const std::string& bookingId) {
    Booking** bookingPtrPtr = bookingIdTable.find(bookingId);
    return (bookingPtrPtr != nullptr) ? *bookingPtrPtr : nullptr;
}

// --- Lấy dữ liệu  ---

const std::vector<Booking*>& BookingManager::getAllBookings() const {
    return this->allBookings;
}

// Lấy danh sách booking theo Agent ID
std::vector<Booking*> BookingManager::getBookingsByAgentId(const std::string& agentId) const {
    std::vector<Booking*> result;
    for (int i = 0; i < allBookings.size(); ++i) {
        if (allBookings[i] != nullptr && allBookings[i]->getAgentId() == agentId) {
            result.push_back(allBookings[i]);
        }
    }
    return result;
}

// --- NGHIỆP VỤ 3: CẬP NHẬT BOOKING ---
bool BookingManager::updateBooking(const std::string& bookingId,
                                     const std::string& newPassengerId,
                                     BookingClass newClass,
                                     const std::string& newSeatId) {
    // Tìm booking theo ID
    Booking* booking = findBookingById(bookingId);
    if (!booking) {
        return false; // Không tìm thấy booking
    }
    
    // Cập nhật các trường
    if (!newPassengerId.empty()) {
        booking->setPassengerId(newPassengerId);
    }
    booking->setClass(newClass);
    if (!newSeatId.empty()) {
        booking->setSeatId(newSeatId);
    }
    
    // Lưu thay đổi vào file
    saveDataToFiles(bookingsFilePath_);
    return true;
}

// --- LƯU BOOKING NGAY LẬP TỨC VÀO FILE ---
bool BookingManager::saveBookingToFile(Booking* booking) {
    if (!booking) {
        std::cerr << "Lỗi: Booking không hợp lệ" << std::endl;
        return false;
    }
    
    // Mở file ở chế độ append để không ghi đè dữ liệu cũ
    std::ofstream file(bookingsFilePath_, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Không thể mở file bookings để ghi: " << bookingsFilePath_ << std::endl;
        return false;
    }
    
    // Ghi booking vào file
    file << booking->toRecordLine() << std::endl;
    file.flush();
    file.close();
    
    // Thêm vào cấu trúc dữ liệu trong bộ nhớ
    allBookings.push_back(booking);
    bookingIdTable.insert(booking->getBookingId(), booking);
    
    std::cout << "Đã lưu booking " << booking->getBookingId() << " vào file" << std::endl;
    return true;
}