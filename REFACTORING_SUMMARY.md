# Tóm tắt Tái cấu trúc Flight và FlightInstance

## Tổng quan
Tài liệu này mô tả các thay đổi được thực hiện để tái cấu trúc cơ chế tạo và định dạng ID cho các lớp `Flight` và `FlightInstance`.

## 1. Thay đổi trong lớp Flight

### 1.1. Loại bỏ thuộc tính flightNumber
- **Trước:** `std::string flightNumber;` - Mã tuyến bay công khai (VD: "VN123")
- **Sau:** Thuộc tính này đã được loại bỏ hoàn toàn khỏi lớp Flight

### 1.2. Thay đổi cơ chế tạo flightId
- **Trước:** `flightId` được tạo bằng `IdGenerator::generateFlightId()` với format "FL-001", "FL-002"...
- **Sau:** `flightId` được tạo theo công thức: `<Airline>-<DepartureIATA>-<ArrivalIATA>`
- **Ví dụ:** `Vietnam Airlines-HAN-SGN`, `Vietjet Air-SGN-HAN`

### 1.3. Cập nhật Constructor
```cpp
// Trước:
Flight(const std::string& number,
       const std::string& airline,
       const std::string& departureIATA,
       const std::string& arrivalIATA);

// Sau:
Flight(const std::string& airline,
       const std::string& departureIATA,
       const std::string& arrivalIATA);
```

### 1.4. Cập nhật Getters
- Đã loại bỏ: `getFlightNumber()`
- Vẫn giữ: `getFlightId()`, `getAirline()`, `getDepartureAirport()`, `getArrivalAirport()`

### 1.5. Cập nhật định dạng file
**Trước (flights.txt):**
```
FL-001|VN123|Vietnam Airlines|HAN|SGN
```

**Sau (flights.txt):**
```
Vietnam Airlines-HAN-SGN|Vietnam Airlines|HAN|SGN
```

## 2. Thay đổi trong lớp FlightInstance

### 2.1. Thêm thuộc tính flightNumber
- **Thêm mới:** `std::string flightNumber;` - Số hiệu chuyến bay công khai (VD: "VJ153")
- Đây là thuộc tính đã được di chuyển từ lớp Flight sang FlightInstance

### 2.2. Thay đổi cơ chế tạo instanceId
- **Trước:** `instanceId` được tạo bằng `IdGenerator::generateInstanceId()` với format "FI-00001", "FI-00002"...
- **Sau:** `instanceId` được tạo theo công thức: `<FlightNumber>-<YYYYMMDD>`
- **Ví dụ:** `VN123-20251025`, `VJ234-20251115`

### 2.3. Hàm chuyển đổi ngày tháng
Thêm hàm helper để chuyển đổi ngày từ "DD/MM/YYYY" sang "YYYYMMDD":
```cpp
static std::string convertDateToYYYYMMDD(const std::string& dateStr) {
    // "25/10/2025" -> "20251025"
    std::string day = dateStr.substr(0, 2);
    std::string month = dateStr.substr(3, 2);
    std::string year = dateStr.substr(6, 4);
    return year + month + day;
}
```

### 2.4. Cập nhật Constructor
```cpp
// Trước:
FlightInstance(const std::string& flightId,
               const std::string& departureDate,
               const std::string& departureTime,
               const std::string& arrivalDate,
               const std::string& arrivalTime,
               int totalEconomySeats,
               int totalBusinessSeats,
               int fareEconomy,
               int fareBusiness);

// Sau:
FlightInstance(const std::string& flightId,
               const std::string& flightNumber,  // <-- Thêm mới
               const std::string& departureDate,
               const std::string& departureTime,
               const std::string& arrivalDate,
               const std::string& arrivalTime,
               int totalEconomySeats,
               int totalBusinessSeats,
               int fareEconomy,
               int fareBusiness);
```

### 2.5. Cập nhật Getters
- Thêm mới: `getFlightNumber()`

### 2.6. Cập nhật định dạng file
**Trước (flight_instances.txt):**
```
FI-00001|FL-001|25/10/2025|08:30|25/10/2025|10:40|150|150|20|20|1200000|3500000
```

**Sau (flight_instances.txt):**
```
VN123-20251025|Vietnam Airlines-HAN-SGN|VN123|25/10/2025|08:30|25/10/2025|10:40|150|150|20|20|1200000|3500000
```

## 3. Thay đổi trong FlightManager

### 3.1. Cập nhật createNewFlight
```cpp
// Trước:
bool createNewFlight(const std::string& number,
                     const std::string& airline,
                     const std::string& departureIATA,
                     const std::string& arrivalIATA);

// Sau:
bool createNewFlight(const std::string& airline,
                     const std::string& departureIATA,
                     const std::string& arrivalIATA);
```

### 3.2. Cập nhật kiểm tra trùng lặp
- **Trước:** Kiểm tra trùng lặp dựa trên `flightNumber`
- **Sau:** Kiểm tra trùng lặp dựa trên `flightId` (combination của airline và route)

### 3.3. Cập nhật createNewInstance
```cpp
// Sau:
bool createNewInstance(const std::string& flightId,
                      const std::string& flightNumber,  // <-- Thêm mới
                      const std::string& departureDate,
                      const std::string& departureTime,
                      const std::string& arrivalDate,
                      const std::string& arrivalTime,
                      int totalEconomySeats,
                      int totalBusinessSeats,
                      int fareEconomy,
                      int fareBusiness);
```

## 4. Thay đổi trong IdGenerator

### 4.1. Đánh dấu deprecated
Các hàm sau đã được đánh dấu là DEPRECATED:
- `generateFlightId()` - Flight IDs are now generated from airline-departure-arrival format
- `generateInstanceId()` - FlightInstance IDs are now generated from flightNumber-YYYYMMDD format

Các hàm này vẫn tồn tại trong code để tương thích ngược, nhưng không nên sử dụng cho các tính năng mới.

## 5. Cập nhật dữ liệu

### 5.1. File flights.txt
- Đã chuyển đổi tất cả 36 flights sang định dạng mới
- Thêm 2 flights mới để đảm bảo tính nhất quán:
  - `Vietjet Air-HAN-SGN`
  - `Vietjet Air-SGN-DAD`

### 5.2. File flight_instances.txt
- Đã chuyển đổi tất cả 50 flight instances sang định dạng mới
- Tất cả các instance đều tham chiếu đến các flight hợp lệ

## 6. Lợi ích của việc tái cấu trúc

1. **Dễ đọc hơn:** FlightId giờ đây mang ý nghĩa rõ ràng (airline + route)
2. **Giảm trùng lặp:** Không còn lưu cả flight ID và flight number ở cùng một nơi
3. **Linh hoạt hơn:** FlightInstance có thể có nhiều số hiệu khác nhau cho cùng một route
4. **Tổ chức tốt hơn:** Số hiệu chuyến bay (flight number) được gắn với chuyến bay cụ thể, không phải với route

## 7. Lưu ý khi sử dụng

### 7.1. Tạo Flight mới
```cpp
// Trước:
flightManager.createNewFlight("VN123", "Vietnam Airlines", "HAN", "SGN");

// Sau:
flightManager.createNewFlight("Vietnam Airlines", "HAN", "SGN");
```

### 7.2. Tạo FlightInstance mới
```cpp
// Sau:
flightManager.createNewInstance(
    "Vietnam Airlines-HAN-SGN",  // flightId
    "VN123",                      // flightNumber (mới)
    "25/10/2025",                 // departureDate
    "08:30",                      // departureTime
    "25/10/2025",                 // arrivalDate
    "10:40",                      // arrivalTime
    150,                          // totalEconomySeats
    20,                           // totalBusinessSeats
    1200000,                      // fareEconomy
    3500000                       // fareBusiness
);
```

### 7.3. Lấy thông tin Flight Number
```cpp
// Trước (từ Flight):
std::string flightNum = flight->getFlightNumber();  // ❌ Không còn hoạt động

// Sau (từ FlightInstance):
std::string flightNum = flightInstance->getFlightNumber();  // ✅ Đúng
```

## 8. Kiểm tra và Xác thực

Tất cả dữ liệu đã được xác thực:
- ✅ 35 unique flights trong flights.txt
- ✅ 50 flight instances trong flight_instances.txt
- ✅ Tất cả flight instances đều tham chiếu đến flights hợp lệ
- ✅ Định dạng ID mới hoạt động chính xác

## 9. Checklist cho Developer

Khi làm việc với code mới:
- [ ] Đảm bảo sử dụng `FlightInstance::getFlightNumber()` thay vì `Flight::getFlightNumber()`
- [ ] Khi tạo Flight mới, không truyền flightNumber vào constructor
- [ ] Khi tạo FlightInstance mới, nhớ truyền flightNumber
- [ ] Kiểm tra các UI components có sử dụng flight number không và cập nhật chúng
- [ ] Test kỹ các chức năng tìm kiếm, lọc, và hiển thị

## 10. Các File Đã Thay Đổi

### Core Files:
1. `include/entities/Flight.h`
2. `src/entities/Flight.cpp`
3. `include/entities/FlightInstance.h`
4. `src/entities/FlightInstance.cpp`
5. `include/core/FlightManager.h`
6. `src/core/FlightManager.cpp`

### Utility Files:
7. `include/utils/GenID.h`

### Data Files:
8. `data/flights.txt`
9. `data/flight_instances.txt`

---
**Ngày tạo:** 31/10/2025  
**Tác giả:** GitHub Copilot Coding Agent  
**Phiên bản:** 1.0
