#include "C:/PBL2/include/core/BookingManager.h"
#include "C:/PBL2/include/core/FlightManager.h"       // Cần để tìm FlightInstance
#include "C:/PBL2/include/entities/FlightInstance.h" // Cần để gọi bookSeats/releaseSeats
#include "C:/PBL2/include/utils/Time.h"             // Cần để lấy ngày giờ
#include <fstream>
#include <string>

// --- Constructor ---
BookingManager::BookingManager(const std::string& bookingsFilePath) { // Chỉ còn 1 path
    this->loadBookingsFromFile(bookingsFilePath);
}

// --- Destructor ---
BookingManager::~BookingManager() {
    // Dọn dẹp bộ nhớ heap đã cấp phát cho các đối tượng Booking
    for (int i = 0; i < allBookings.size(); i++) {
        delete allBookings[i];
    }
    // DynamicArray sẽ tự dọn dẹp mảng 'data' của nó
}


// --- Hàm trợ giúp nội bộ: Tải dữ liệu ---

void BookingManager::loadBookingsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // Dùng fromRecordLine của Booking đã được tái cấu trúc
                Booking bookingOnStack = Booking::fromRecordLine(line);
                this->allBookings.push_back(new Booking(bookingOnStack));
            }
        }
        file.close();
    }
}

// --- Chức năng Lưu trữ (Persistence) ---

bool BookingManager::saveDataToFiles(const std::string& bookingsFilePath) const { 
    // Chỉ lưu danh sách Booking
    std::ofstream bookingsFile(bookingsFilePath);
    if (!bookingsFile.is_open()) return false;
    for (size_t i = 0; i < allBookings.size(); ++i) {
        bookingsFile << allBookings[i]->toRecordLine() << "\n";
    }
    bookingsFile.close();
    return true;
}

// --- NGHIỆP VỤ 1: BÁN VÉ (TẠO BOOKING) ---

Booking* BookingManager::createNewBooking( FlightManager& flightManager,
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
    
    // Gọi hàm của FlightInstance để trừ ghế (đã có trong code của bạn)
    if (!instance->bookSeats(seatClassToBook, 1)) {
        return nullptr; // Hết ghế
    }

    // --- Nếu trừ ghế thành công, tiến hành tạo Booking ---

    // 4. TẠO BOOKING (với status mặc định là Issued)
    std::string currentDate = utils::Time::formatLocal(utils::Time::nowUtc(), "%Y-%m-%d %H:%M:%S");
    
    Booking* newBooking = new Booking(
        flightInstanceId, 
        passengerId, 
        currentDate, 
        bookingClass, 
        baseFare,
        BookingStatus::Issued // Trạng thái mặc định
    );
    
    // Thêm vào mảng RAM
    this->allBookings.push_back(newBooking);

    // 5. Trả về booking vừa tạo (để TUI có thể load lại)
    return newBooking;
}

// --- NGHIỆP VỤ 2: HỦY BOOKING ---

bool BookingManager::cancelBooking(FlightManager& flightManager, const std::string& bookingId) {
    // 1. Tìm Booking (dùng vòng lặp for tuyến tính như bạn yêu cầu)
    Booking* booking = findBookingById(bookingId);
    if (booking == nullptr) {
        return false; // Không tìm thấy booking
    }

    // 2. Chỉ hủy booking đang 'Issued'
    if (booking->getStatus() != BookingStatus::Issued) {
        return false; // Booking này đã bị hủy rồi
    }

    // 3. HOÀN TRẢ GHẾ
    FlightInstance* instance = flightManager.findInstanceById(booking->getFlightInstanceId());
    if (instance != nullptr) {
        // Xác định hạng ghế để hoàn trả
        SeatClass seatClassToRelease = (booking->getClass() == BookingClass::Economy)
                                       ? SeatClass::Economy
                                       : SeatClass::Business;
        
        // Gọi hàm của FlightInstance để cộng 1 ghế trở lại
        instance->releaseSeats(seatClassToRelease, 1);
    }
    // (Nếu instance == nullptr, tức chuyến bay đã bị xóa, ta vẫn cho hủy booking)

    // 4. Cập nhật trạng thái Booking -> Cancelled
    booking->setStatus(BookingStatus::Cancelled);
    
    return true;
}


// --- CÁC HÀM TÌM KIẾM ---

Booking* BookingManager::findBookingById(const std::string& bookingId) {
    // Thuật toán tìm kiếm tuyến tính (vòng lặp for)
    for (size_t i = 0; i < allBookings.size(); ++i) {
        if (allBookings[i]->getBookingId() == bookingId) {
            return allBookings[i];
        }
    }
    return nullptr; // Không tìm thấy
}

// --- Lấy dữ liệu ---

const DynamicArray<Booking*>& BookingManager::getAllBookings() const {
    return this->allBookings;
}