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

// <<< THÊM MỚI: Xây dựng bảng băm cho Passenger ID >>>
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
// <<< THAY ĐỔI: Cập nhật HashTable >>>
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
    // (Bỏ qua logic cũ `instance->bookSeats(...)` vì SeatManager đã xử lý)
    std::string currentDate = utils::DateTime::formatLocal(utils::DateTime::nowUtc(), "%Y-%m-%d %H:%M:%S");
    
    // Sử dụng seatId đã lấu được ở dòng 86 (KHÔNG gọi getSelectedSeat() vì đã bị reset)
    Booking* newBooking = new Booking(flightId, agentId, passengerId, seatId, currentDate, bookingClass, baseFare, BookingStatus::Issued); // <-- Đã đổi

    // Thêm vào std::vector và HashTable
    this->allBookings.push_back(newBooking);
    this->bookingIdTable.insert(newBooking->getBookingId(), newBooking);
    
    return newBooking;
}

// --- NGHIỆP VỤ 2: HỦY BOOKING (ĐÃ CẬP NHẬT LOGIC THỜI GIAN) ---
// CẬP NHẬT: Thêm tham số SeatManager
bool BookingManager::cancelBooking(FlightManager& flightManager, SeatManager& seatManager, const std::string& bookingId) {
    
    // Step 1: Find and validate booking
    Booking* booking = findBookingById(bookingId);
    if (booking == nullptr || booking->getStatus() != BookingStatus::Issued) {
        return false; 
    }
    
    // Step 2: Check cancellation rules
    if (currentRule == nullptr || !currentRule->isCancelAllowed()) {
        return false; 
    }
    
    // Step 3: Get flight (trước là flight instance)
    // <-- ĐÃ ĐỔI TÊN HÀNG LOẠT Ở ĐÂY
    Flight* flight = flightManager.findFlightById(booking->getFlightId());
    
    // Step 4: CHECK TIME CONSTRAINT FIRST (before releasing seat)
    if (flight != nullptr) { // <-- Đã đổi
        auto departureTime = utils::DateTime::fromDmYHm(
            flight->getDepartureDate(), // <-- Đã đổi
            flight->getDepartureTime()  // <-- Đã đổi
        );
        auto now = utils::DateTime::nowUtc();
        auto duration = std::chrono::duration_cast<std::chrono::hours>(
            departureTime - now
        );
        
        if (!currentRule->isCancellable(duration.count())) {
            std::cerr << "Cannot cancel: Too close to departure time" << std::endl;
            return false;   // Fail early - no seat changes made yet
        }
    }
    
    // Step 5: NOW release seat (time check passed)
    std::string seatIdToRelease = booking->getSeatID();
    bool seatReleased = false;
    
    if (!seatIdToRelease.empty() && flight != nullptr) { // <-- Đã đổi
        if (seatManager.loadSeatMapFor(flight)) { // <-- Đã đổi
            if (seatManager.releaseSeat(seatIdToRelease)) {
                // SAVE SEAT CHANGES IMMEDIATELY
                if (seatManager.saveChanges()) {
                    seatReleased = true;
                    std::cout << "Released and saved seat " << seatIdToRelease << std::endl;
                } else {
                    std::cerr << "ERROR: Failed to save seat changes!" << std::endl;
                    // No rollback needed here - seat was released in memory only
                    // and save failed, so nothing was persisted to disk yet.
                    // Booking status hasn't been changed yet, so state is consistent.
                    return false;   // Fail if can't save
                }
            }
        }
    }

    flight->setAvailableSeats(flight->getAvailableSeats() + 1); // Cập nhật số ghế trống
    
    // Step 6: Update booking status
    booking->setStatus(BookingStatus::Cancelled);
    
    // Step 7: SAVE BOOKING CHANGES IMMEDIATELY
    if (!saveDataToFiles(bookingsFilePath_)) {
        std::cerr << "ERROR: Failed to save booking status!" << std::endl;
        
        // ROLLBACK: Re-book the seat if it was released
        if (seatReleased && flight != nullptr) { // <-- Đã đổi
            seatManager.loadSeatMapFor(flight); // <-- Đã đổi
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

// <<< THAY ĐỔI: Dùng HashTable >>>
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
                             const std::string& newFlightId, // <-- Đã đổi
                             const std::string& newSeatNumber,
                             BookingClass newClass) {
    // Step 1: Find and validate booking
    Booking* booking = findBookingById(bookingId);
    if (!booking || booking->getStatus() != BookingStatus::Issued) {
        std::cerr << "Cannot change booking: Booking not found or not in Issued status" << std::endl;
        return false;
    }
    
    // Step 2: Validate new flight exists (trước là flight instance)
    // <-- ĐÃ ĐỔI TÊN HÀNG LOẠT Ở ĐÂY
    Flight* newFlight = flightManager.findFlightById(newFlightId);
    if (!newFlight) {
        std::cerr << "Cannot change booking: New flight not found" << std::endl; // <-- Đã đổi
        return false;
    }
    
    // Step 3: Save old flight info (for rollback if needed)
    std::string oldFlightId = booking->getFlightId(); // <-- Đã đổi
    std::string oldSeatId = booking->getSeatID();
    Flight* oldFlight = flightManager.findFlightById(oldFlightId); // <-- Đã đổi

    // ✅ THÊM: Calculate new fare based on class
    int newBaseFare = (newClass == BookingClass::Economy) 
                     ? newFlight->getFareEconomy() 
                     : newFlight->getFareBusiness();
    
    // BOOK NEW SEAT FIRST (Step 4)
    if (!seatManager.loadSeatMapFor(newFlight)) { // <-- Đã đổi
        std::cerr << "ERROR: Failed to load seat map for new flight" << std::endl; // <-- Đã đổi
        return false;
    }
    
    if (!seatManager.bookSeat(newSeatNumber)) {
        std::cerr << "ERROR: Failed to book new seat" << std::endl;
        return false;
    }
    
    if (!seatManager.saveChanges()) {
        // Rollback: release the new seat we just tried to book
        seatManager.releaseSeat(newSeatNumber);
        std::cerr << "ERROR: Failed to save new seat booking" << std::endl;
        return false;
    }
    
    std::cout << "Booked new seat " << newSeatNumber << std::endl;
    
    // ONLY AFTER NEW SEAT IS COMMITTED, RELEASE OLD SEAT (Step 5)
    if (oldFlight && !oldSeatId.empty()) { // <-- Đã đổi
        if (seatManager.loadSeatMapFor(oldFlight)) { // <-- Đã đổi
            if (seatManager.releaseSeat(oldSeatId)) {
                if (seatManager.saveChanges()) {
                    std::cout << "Released old seat " << oldSeatId << std::endl;
                } else {
                    std::cerr << "WARNING: Failed to save after releasing old seat (new seat already booked)" << std::endl;
                    // Not critical - new seat is already committed
                }
            }
        }
    }
    
    // Step 6: Update booking information
    booking->setFlightId(newFlightId); // <-- Đã đổi
    booking->setSeatId(newSeatNumber);
    booking->setStatus(BookingStatus::Changed);
    // ✅ THÊM: Update class and fare
    booking->setClass(newClass);
    booking->setBaseFare(newBaseFare);
    
    // Step 7: Save booking changes to file
    if (!saveDataToFiles(bookingsFilePath_)) {
        std::cerr << "ERROR: Failed to save booking changes (seats already changed, manual intervention needed)" << std::endl;
        // Critical error but seats are already changed
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
    
    // Check if booking ID already exists in memory
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

// --- HELPER METHODS FOR UI ---

// Check if booking can be cancelled (considering time constraints)
bool BookingManager::canCancelBooking(const std::string& bookingId, 
                                      FlightManager& flightManager) const {
    Booking* booking = const_cast<BookingManager*>(this)->findBookingById(bookingId);
    if (!booking || booking->getStatus() != BookingStatus::Issued) {
        return false;
    }
    
    if (!currentRule || !currentRule->isCancelAllowed()) {
        return false;
    }
    
    // <-- ĐÃ ĐỔI TÊN HÀNG LOẠT Ở ĐÂY
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

// Get cancellation deadline for a booking
std::string BookingManager::getCancellationDeadline(
    const std::string& bookingId,
    FlightManager& flightManager) const 
{ 
    Booking* booking = const_cast<BookingManager*>(this)->findBookingById(bookingId);
    if (!booking) return "N/A";
    
    // <-- ĐÃ ĐỔI TÊN HÀNG LOẠT Ở ĐÂY
    Flight* flight = flightManager.findFlightById(
        booking->getFlightId()
    );
    if (!flight) return "N/A";
    
    if (!currentRule) return "N/A";
    
    // Get departure time
    auto departureTime = utils::DateTime::fromDmYHm(
        flight->getDepartureDate(), 
        flight->getDepartureTime()
    );
    
    // Subtract minimum cancellation hours
    auto deadline = departureTime - std::chrono::hours(currentRule->getCancelCutoffHours());
    
    return utils::DateTime::formatLocal(deadline, "%d/%m/%Y %H:%M");
}