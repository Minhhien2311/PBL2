#include "core/BookingManager.h"
#include "core/FlightManager.h"         // Cần để tìm Flight (trước là FlightInstance)
#include "entities/Flight.h"            // Cần để gọi bookSeats/releaseSeats (trước là FlightInstance)
#include "utils/DateTime.h"             // Cần để lấy ngày giờ
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

    // Xây dựng bảng băm sau khi nạp dữ liệu
    this->buildBookingIdTable();
    this->buildPassengerIdTable();
}

// --- Destructor  ---
BookingManager::~BookingManager() {
    // Tự động lưu dữ liệu trước khi xóa
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

void BookingManager::buildBookingIdTable() {
    for (int i = 0; i < allBookings.size(); ++i) {
        if (allBookings[i] != nullptr) {
            bookingIdTable.insert(allBookings[i]->getBookingId(), allBookings[i]);
        }
    }
}

void BookingManager::buildPassengerIdTable() {
    for (int i = 0; i < allBookings.size(); ++i) {
        if (allBookings[i] != nullptr) {
            const std::string& passengerId = allBookings[i]->getPassengerId();
            std::vector<Booking*>* bookingsForPassenger = passengerIdTable.find(passengerId);
            if (bookingsForPassenger == nullptr) {
                std::vector<Booking*> newList;
                newList.push_back(allBookings[i]);
                passengerIdTable.insert(passengerId, newList);
            } else {
                bookingsForPassenger->push_back(allBookings[i]);
            }
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
Booking* BookingManager::createNewBooking( FlightManager& flightManager,
                                       const std::string& flightId, // <-- Đã đổi
                                       const std::string& agentId,
                                       const std::string& passengerId,
                                       BookingClass bookingClass,
                                       int baseFare,
                                       SeatManager& seatManager) // Dùng tham chiếu &
{
    // BƯỚC 1: Kiểm tra các đầu vào cơ bản
    if (flightId.empty() || passengerId.empty() || baseFare < 0) return nullptr; // <-- Đã đổi

    Flight* flight = flightManager.findFlightById(flightId);
    if (flight->getAvailableSeats() == 0) {
        std::cerr << "Lỗi Booking: Chuyến bay đã hết chỗ." << std::endl;
        return nullptr;
    }

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

    flight->setAvailableSeats(flight->getAvailableSeats() - 1); // Cập nhật số ghế trống

    // BƯỚC 4: Tạo đối tượng Booking mới
    std::string currentDate = utils::DateTime::formatLocal(utils::DateTime::nowUtc(), "%Y-%m-%d %H:%M:%S");
    
    // Sử dụng seatId đã lấu được ở dòng 86 (KHÔNG gọi getSelectedSeat() vì đã bị reset)
    Booking* newBooking = new Booking(flightId, agentId, passengerId, seatId, currentDate, bookingClass, baseFare, BookingStatus::Issued); // <-- Đã đổi

    // Thêm vào std::vector và HashTable
    this->allBookings.push_back(newBooking);
    this->bookingIdTable.insert(newBooking->getBookingId(), newBooking);
    
    return newBooking;
}

// --- NGHIỆP VỤ 2: HỦY BOOKING (ĐÃ CẬP NHẬT LOGIC THỜI GIAN) ---
bool BookingManager::cancelBooking(FlightManager& flightManager, SeatManager& seatManager, const std::string& bookingId) {
    
    // Bước 1: Tìm và xác nhận đặt chỗ
    Booking* booking = findBookingById(bookingId);
    if (booking == nullptr || booking->getStatus() != BookingStatus::Issued) {
        return false; 
    }
    
    // Bước 2: Kiểm tra các quy tắc hủy đặt phòng
    if (currentRule == nullptr || !currentRule->isCancelAllowed()) {
        return false; 
    }
    
    // Bước 3: Nhận chuyến bay
    Flight* flight = flightManager.findFlightById(booking->getFlightId());
    
    // Bước 4: KIỂM TRA GIỚI HẠN THỜI GIAN TRƯỚC
    if (flight != nullptr) {
        auto departureTime = utils::DateTime::fromDmYHm(
            flight->getDepartureDate(),
            flight->getDepartureTime()
        );
        auto now = utils::DateTime::nowUtc();
        auto duration = std::chrono::duration_cast<std::chrono::hours>(
            departureTime - now
        );
        
        if (!currentRule->isCancellable(duration.count())) {
            std::cerr << "Cannot cancel: Too close to departure time" << std::endl;
            return false;   // Thất bại ngay từ đầu - chưa có sự thay đổi chỗ ngồi nào được thực hiện.
        }
    }
    
    // Bước 5: Bây giờ hãy thả ghế ra (đã kiểm tra thời gian thành công)
    std::string seatIdToRelease = booking->getSeatID();
    bool seatReleased = false;
    
    if (!seatIdToRelease.empty() && flight != nullptr) {
        if (seatManager.loadSeatMapFor(flight)) {
            if (seatManager.releaseSeat(seatIdToRelease)) {
                // SAVE SEAT CHANGES IMMEDIATELY
                if (seatManager.saveChanges()) {
                    seatReleased = true;
                    std::cout << "Released and saved seat " << seatIdToRelease << std::endl;
                } else {
                    std::cerr << "ERROR: Failed to save seat changes!" << std::endl;
                    return false;   // Thất bại nếu không thể lưu.
                }
            }
        }
    }

    flight->setAvailableSeats(flight->getAvailableSeats() + 1); // Cập nhật số ghế trống
    
    // Bước 6: Cập nhật trạng thái đặt chỗ
    booking->setStatus(BookingStatus::Cancelled);
    
    // Bước 7: LƯU NGAY CÁC THAY ĐỔI ĐẶT PHÒNG
    if (!saveDataToFiles(bookingsFilePath_)) {
        std::cerr << "ERROR: Failed to save booking status!" << std::endl;
        
        // HOÀN TRẢ: Đặt lại chỗ ngồi nếu chỗ đó đã được giải phóng.
        if (seatReleased && flight != nullptr) {
            seatManager.loadSeatMapFor(flight);
            seatManager.bookSeat(seatIdToRelease);
            seatManager.saveChanges();
            std::cerr << "Rolled back seat release" << std::endl;
        }
        
        booking->setStatus(BookingStatus::Issued);
        return false;
    }
    
    std::cout << "Successfully cancelled booking " << bookingId << std::endl;
    return true;
}

// --- CÁC HÀM TÌM KIẾM  ---

Booking* BookingManager::findBookingById(const std::string& bookingId) {
    Booking** bookingPtrPtr = bookingIdTable.find(bookingId);
    return (bookingPtrPtr != nullptr) ? *bookingPtrPtr : nullptr;
}

std::vector<Booking*> BookingManager::findBookingsByPassengerId(const std::string& passengerId) {
    std::vector<Booking*> result;
    for (int i = 0; i < allBookings.size(); ++i) {
        if (allBookings[i] != nullptr && allBookings[i]->getPassengerId() == passengerId) {
            result.push_back(allBookings[i]);
        }
    }
    return result;
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

// --- NGHIỆP VỤ 4: ĐỔI VÉ ---
bool BookingManager::changeBooking(FlightManager& flightManager,
                             SeatManager& seatManager,
                             const std::string& bookingId,
                             const std::string& newFlightId,
                             const std::string& newSeatNumber,
                             BookingClass newClass) {
    // Bước 1: Tìm và xác nhận đặt chỗ
    Booking* booking = findBookingById(bookingId);
    if (!booking || booking->getStatus() != BookingStatus::Issued) {
        std::cerr << "Cannot change booking: Booking not found or not in Issued status" << std::endl;
        return false;
    }
    
    // Bước 2: Xác thực chuyến bay mới tồn tại
    Flight* newFlight = flightManager.findFlightById(newFlightId);
    if (!newFlight) {
        std::cerr << "Cannot change booking: New flight not found" << std::endl;
        return false;
    }
    
    // Bước 3: Lưu thông tin chuyến bay cũ (để khôi phục nếu cần)
    std::string oldFlightId = booking->getFlightId();
    std::string oldSeatId = booking->getSeatID();
    Flight* oldFlight = flightManager.findFlightById(oldFlightId);

    int newBaseFare = (newClass == BookingClass::Economy) 
                     ? newFlight->getFareEconomy() 
                     : newFlight->getFareBusiness();
    
    // Bước 4: Đặt chỗ ngồi mới
    if (!seatManager.loadSeatMapFor(newFlight)) {
        std::cerr << "ERROR: Failed to load seat map for new flight" << std::endl;
        return false;
    }
    
    if (!seatManager.bookSeat(newSeatNumber)) {
        std::cerr << "ERROR: Failed to book new seat" << std::endl;
        return false;
    }
    
    if (!seatManager.saveChanges()) {
        // Hoàn tác: giải phóng chỗ ngồi mới mà chúng ta vừa cố gắng đặt.
        seatManager.releaseSeat(newSeatNumber);
        std::cerr << "ERROR: Failed to save new seat booking" << std::endl;
        return false;
    }
    
    std::cout << "Booked new seat " << newSeatNumber << std::endl;
    
    // Bước 5: CHỈ SAU KHI GHẾ MỚI ĐƯỢC CHỌN, MỚI ĐƯỢC GIẢI PHÓNG GHẾ CŨ
    if (oldFlight && !oldSeatId.empty()) {
        if (seatManager.loadSeatMapFor(oldFlight)) {
            if (seatManager.releaseSeat(oldSeatId)) {
                if (seatManager.saveChanges()) {
                    std::cout << "Released old seat " << oldSeatId << std::endl;
                } else {
                    std::cerr << "WARNING: Failed to save after releasing old seat (new seat already booked)" << std::endl;
                }
            }
        }
    }
    
    // Step 6: Update booking information
    booking->setFlightId(newFlightId);
    booking->setSeatId(newSeatNumber);
    booking->setStatus(BookingStatus::Changed);
    booking->setClass(newClass);
    booking->setBaseFare(newBaseFare);
    
    // Bước 7: Lưu các thay đổi đặt chỗ vào tệp
    if (!saveDataToFiles(bookingsFilePath_)) {
        std::cerr << "ERROR: Failed to save booking changes (seats already changed, manual intervention needed)" << std::endl;
        return false;
    }
    
    std::cout << "Successfully changed booking " << bookingId << std::endl;
    return true;
}

// --- LƯU BOOKING NGAY LẬP TỨC VÀO FILE ---
bool BookingManager::saveBookingToFile(Booking* booking) {
    if (!booking) {
        std::cerr << "Lỗi: Booking không hợp lệ" << std::endl;
        return false;
    }
    
    // Kiểm tra xem ID đặt chỗ đã tồn tại trong bộ nhớ chưa.
    if (bookingIdTable.find(booking->getBookingId()) != nullptr) {
        std::cerr << "Lỗi: Booking ID đã tồn tại: " << booking->getBookingId() << std::endl;
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

// --- CÁC PHƯƠNG THỨC HỖ TRỢ CHO GIAO DIỆN NGƯỜI DÙNG ---

// Kiểm tra xem có thể hủy đặt phòng được không (có tính đến giới hạn thời gian).
bool BookingManager::canCancelBooking(const std::string& bookingId, 
                                      FlightManager& flightManager) const {
    Booking* booking = const_cast<BookingManager*>(this)->findBookingById(bookingId);
    if (!booking || booking->getStatus() != BookingStatus::Issued) {
        return false;
    }
    
    if (!currentRule || !currentRule->isCancelAllowed()) {
        return false;
    }
    
    Flight* flight = flightManager.findFlightById(
        booking->getFlightId()
    );
    if (!flight) return false;
    
    auto departureTime = utils::DateTime::fromDmYHm(
        flight->getDepartureDate(), 
        flight->getDepartureTime()
    );
    auto now = utils::DateTime::nowUtc();
    auto duration = std::chrono::duration_cast<std::chrono::hours>(
        departureTime - now
    );
    
    return currentRule->isCancellable(duration.count());
}

// Tìm hiểu thời hạn hủy đặt phòng
std::string BookingManager::getCancellationDeadline( const std::string& bookingId, FlightManager& flightManager) const 
{ 
    Booking* booking = const_cast<BookingManager*>(this)->findBookingById(bookingId);
    if (!booking) return "N/A";
    
    Flight* flight = flightManager.findFlightById(
        booking->getFlightId()
    );
    if (!flight) return "N/A";
    
    if (!currentRule) return "N/A";
    
    //Nhận thời gian khởi hành
    auto departureTime = utils::DateTime::fromDmYHm(
        flight->getDepartureDate(), 
        flight->getDepartureTime()
    );
    
    // Trừ đi số giờ hủy tối thiểu
    auto deadline = departureTime - std::chrono::hours(currentRule->getCancelCutoffHours());
    
    return utils::DateTime::formatLocal(deadline, "%d/%m/%Y %H:%M");
}

bool BookingManager::canChangeBooking(const std::string& bookingId, FlightManager& flightManager) const {
    Booking* booking = const_cast<BookingManager*>(this)->findBookingById(bookingId);
    if (!booking || booking->getStatus() != BookingStatus::Issued) {
        return false;
    }
    
    if (!currentRule || !currentRule->isChangeAllowed()) {
        return false;
    }
    
    Flight* flight = flightManager.findFlightById(
        booking->getFlightId()
    );
    if (!flight) return false;
    
    auto departureTime = utils::DateTime::fromDmYHm(
        flight->getDepartureDate(), 
        flight->getDepartureTime()
    );
    auto now = utils::DateTime::nowUtc();
    auto duration = std::chrono::duration_cast<std::chrono::hours>(
        departureTime - now
    );
    
    return currentRule->isChangeable(duration.count());
}

std::string BookingManager::getChangeDeadline(
    const std::string& bookingId,
    FlightManager& flightManager) const 
{ 
    Booking* booking = const_cast<BookingManager*>(this)->findBookingById(bookingId);
    if (!booking) return "N/A";
    
    Flight* flight = flightManager.findFlightById(
        booking->getFlightId()
    );
    if (!flight) return "N/A";
    
    if (!currentRule) return "N/A";
    
    // Nhận thời gian khởi hành
    auto departureTime = utils::DateTime::fromDmYHm(
        flight->getDepartureDate(), 
        flight->getDepartureTime()
    );
    
    // Trừ đi số giờ thay đổi tối thiểu
    auto deadline = departureTime - std::chrono::hours(currentRule->getChangeCutoffHours());
    
    return utils::DateTime::formatLocal(deadline, "%d/%m/%Y %H:%M");
}

int BookingManager::applyPromotion(const std::string& promoCode, int baseFare) {
    if (promoCode.empty() || baseFare <= 0) return baseFare;
    
    std::ifstream file("C:/PBL2/data/promotions.txt");
    if (!file.is_open()) return baseFare;
    
    std::string line, code, type;
    double value;
    bool isActive;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Parse: CODE|Desc|TYPE|VALUE|Start|End|Active
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        size_t p5 = line.find('|', p4 + 1);
        size_t p6 = line.find('|', p5 + 1);
        
        if (p1 == std::string::npos || p6 == std::string::npos) continue;
        
        code = line.substr(0, p1);
        type = line.substr(p2 + 1, p3 - p2 - 1);
        value = std::stod(line.substr(p3 + 1, p4 - p3 - 1));
        isActive = (line.substr(p6 + 1, 1) == "1");
        
        // So sánh mã (không phân biệt hoa thường)
        std::string upperCode = code, upperInput = promoCode;
        std::transform(upperCode.begin(), upperCode.end(), upperCode.begin(), ::toupper);
        std::transform(upperInput.begin(), upperInput.end(), upperInput.begin(), ::toupper);
        
        if (upperCode == upperInput && isActive) {
            file.close();
            
            // Tính discount
            int discount = 0;
            if (type == "PERCENT") {
                discount = static_cast<int>(baseFare * (value / 100.0));
            } else if (type == "AMOUNT") {
                discount = static_cast<int>(value);
            }
            
            // Đảm bảo không âm
            int finalPrice = baseFare - discount;
            return (finalPrice > 0) ? finalPrice : 0;
        }
    }
    
    file.close();
    return baseFare; // Không tìm thấy hoặc hết hạn → trả về giá gốc
}