#include "core/SeatManager.h"
#include "entities/flightInstance.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <map>

SeatManager::SeatManager() : flightInstanceId(""), selectedSeat(nullptr) {}

// THAY ĐỔI LỚN: Logic khởi tạo phù hợp với DynamicArray của bạn
void SeatManager::initializeSeatMap(int rows, int cols) {
    // 1. Gọi clear() để reset seatMap về size = 0
    seatMap.clear();

    // 2. Vì không có resize, chúng ta phải dùng vòng lặp và push_back
    for (int r = 0; r < rows; ++r) {
        // Tạo một mảng động cho hàng hiện tại, với dung lượng ban đầu là `cols`
        DynamicArray<Seat> rowArray(cols); 
        
        // Lấp đầy hàng này bằng các đối tượng Seat
        for (int c = 0; c < cols; ++c) {
            std::string id = Seat::coordinatesToId({r, c});
            rowArray.push_back(Seat(id)); // Kích thước của rowArray tăng dần
        }
        
        // Thêm hàng đã hoàn chỉnh vào seatMap
        seatMap.push_back(rowArray); // Kích thước của seatMap tăng dần
    }
}

// Hàm loadForFlight không cần thay đổi logic tính toán, chỉ cần initializeSeatMap hoạt động đúng
bool SeatManager::loadForFlight(const FlightInstance& flight) {
    release();

    this->flightInstanceId = flight.getInstanceId();
    if (this->flightInstanceId.empty()) { /* lỗi */ return false; }

    int totalCapacity = flight.getTotalCapacity();
    if (totalCapacity <= 0) { /* lỗi */ return false; }

    int rows = DEFAULT_ROWS;
    int cols = static_cast<int>(std::ceil(static_cast<double>(totalCapacity) / rows));

    // Hàm này giờ sẽ hoạt động đúng với DynamicArray của bạn
    initializeSeatMap(rows, cols); 
    std::cout << "Đã khởi tạo seatMap " << rows << "x" << cols 
              << " cho chuyến bay " << this->flightInstanceId << std::endl;

    return loadBookedSeatsFromFile();
}

void SeatManager::release() {
    this->flightInstanceId = "";
    this->seatMap.clear();
    this->selectedSeat = nullptr; // Rất quan trọng: reset lựa chọn khi đổi chuyến bay
}

// === CÁC HÀM MỚI CHO QUY TRÌNH CHỌN GHẾ ===
bool SeatManager::selectSeat(const std::string& seatId) {
    // Không thể chọn nếu chưa load chuyến bay
    if (flightInstanceId.empty()) {
        std::cerr << "Lỗi: Chưa có chuyến bay nào được tải." << std::endl;
        return false;
    }

    // Tìm tọa độ của ghế
    auto coords = Seat(seatId).getCoordinates();
    if (coords.first == -1) {
        std::cerr << "Lỗi: ID ghế '" << seatId << "' không hợp lệ." << std::endl;
        return false;
    }

    auto [row, col] = coords;

    // Kiểm tra xem tọa độ có hợp lệ không
    if (row >= seatMap.size() || col >= seatMap[row].size()) {
        std::cerr << "Lỗi: Ghế '" << seatId << "' không tồn tại trên chuyến bay này." << std::endl;
        return false;
    }

    // Kiểm tra xem ghế có còn trống không
    if (seatMap[row][col].isBooked()) {
        std::cerr << "Lỗi: Ghế '" << seatId << "' đã có người đặt." << std::endl;
        return false;
    }

    // Tất cả đều ổn, lưu con trỏ tới ghế này
    selectedSeat = &seatMap[row][col];
    std::cout << "Đã chọn thành công ghế: " << selectedSeat->getId() << std::endl;
    return true;
}

void SeatManager::cancelSelection() {
    if (selectedSeat) {
        std::cout << "Đã hủy lựa chọn ghế: " << selectedSeat->getId() << std::endl;
        selectedSeat = nullptr;
    }
}

const Seat* SeatManager::getSelectedSeat() const {
    return selectedSeat;
}

bool SeatManager::confirmSelection() {
    if (!selectedSeat) {
        std::cerr << "Lỗi: Chưa có ghế nào được chọn để xác nhận." << std::endl;
        return false;
    }

    // 1. Đổi trạng thái của ghế mà con trỏ đang trỏ tới
    selectedSeat->bookSeat();
    std::cout << "Đang xác nhận và lưu ghế: " << selectedSeat->getId() << std::endl;

    // 2. Lưu tất cả thay đổi vào file
    bool success = saveAllFlightsStatusToFile();

    // 3. Reset con trỏ sau khi hoàn tất
    if (success) {
        selectedSeat = nullptr;
    } else {
        // Nếu lưu file thất bại, trả lại trạng thái cũ để người dùng thử lại
        selectedSeat->releaseSeat(); 
        std::cerr << "Lỗi: Không thể lưu thay đổi vào file. Vui lòng thử lại." << std::endl;
    }
    
    return success;
}

// CẬP NHẬT TRỰC TIẾP GHẾ VÀO FILE
bool SeatManager::updateAndSaveChanges(const std::string& seatId, SeatStatus newStatus) {
    if (flightInstanceId.empty()) { /* lỗi */ return false; }

    auto coords = Seat(seatId).getCoordinates();
    if (coords.first == -1) { /* lỗi */ return false; }

    auto [row, col] = coords;

    // THAY ĐỔI: Sử dụng hàm size()
    if (row >= seatMap.size() || col >= seatMap[row].size()) {
        std::cerr << "Lỗi: Tọa độ ghế nằm ngoài sơ đồ." << std::endl;
        return false;
    }
    
    seatMap[row][col].setStatus(newStatus);
    std::cout << "Đã cập nhật ghế " << seatId << " trong bộ nhớ." << std::endl;

    return saveAllFlightsStatusToFile();
}

const DynamicArray<DynamicArray<Seat>>& SeatManager::getSeatMap() const {
    return seatMap;
}

const std::string& SeatManager::getCurrentFlightId() const {
    return flightInstanceId;
}

bool SeatManager::loadBookedSeatsFromFile() {
    std::ifstream file(seatStatusFilePath);
    if (!file.is_open()) { /* ... thông báo ... */ return true; }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string currentFid;
        std::getline(ss, currentFid, '|');

        if (currentFid == this->flightInstanceId) {
            std::string bookedSeatsStr;
            std::getline(ss, bookedSeatsStr);

            std::stringstream seatsStream(bookedSeatsStr);
            std::string seatId;
            while(std::getline(seatsStream, seatId, ',')) {
                auto coords = Seat(seatId).getCoordinates();
                if (coords.first != -1) {
                    // THAY ĐỔI: Sử dụng getSize()
                    if (coords.first < seatMap.size() && coords.second < seatMap[coords.first].size()) {
                         seatMap[coords.first][coords.second].bookSeat();
                    }
                }
            }
            file.close();
            std::cout << "Đã tải thành công trạng thái ghế..." << std::endl;
            return true;
        }
    }
    file.close();
    std::cout << "Thông báo: Không tìm thấy dữ liệu..." << std::endl;
    return true;
}

bool SeatManager::saveAllFlightsStatusToFile() {
    std::map<std::string, std::string> allFlightsData;
    std::ifstream inFile(seatStatusFilePath);
    if (inFile.is_open()) { /* ... đọc file vào map ... */ }
    inFile.close();

    std::stringstream currentFlightLine;
    currentFlightLine << this->flightInstanceId << "|";
    std::string bookedSeatsStr;

    // THAY ĐỔI: Sử dụng vòng lặp for với chỉ số thay cho for-each
    for (int r = 0; r < seatMap.size(); ++r) {
        for (int c = 0; c < seatMap[r].size(); ++c) {
            const Seat& seat = seatMap[r][c]; // Lấy tham chiếu đến ghế để đọc
            if (seat.isBooked()) {
                if (!bookedSeatsStr.empty()) {
                    bookedSeatsStr += ",";
                }
                bookedSeatsStr += seat.getId();
            }
        }
    }

    currentFlightLine << bookedSeatsStr;
    allFlightsData[this->flightInstanceId] = currentFlightLine.str();

    std::ofstream outFile(seatStatusFilePath);
    if (!outFile.is_open()) { /* ... lỗi ... */ return false; }
    
    for (const auto& pair : allFlightsData) {
        outFile << pair.second << "\n";
    }
    
    outFile.close();
    std::cout << "Đã lưu thành công trạng thái mới..." << std::endl;
    return true;
}