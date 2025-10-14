cmake_minimum_required(VERSION 3.20)
project(PBL2 LANGUAGES CXX)

# ====== Thiết lập chung cho dự án ======
set(CMAKE_CXX_STANDARD 20)                 # Chuẩn C++ sử dụng
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)      # Xuất compile_commands.json (phân tích tĩnh/IDE)

# Thiết lập cảnh báo hữu ích
if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Biến macro trỏ tới thư mục data trong thư mục build (để code mở file dữ liệu runtime)
set(DATA_DIR "${CMAKE_BINARY_DIR}/data")


# ===============   KHAI BÁO CÁC MODULE DƯỚI DẠNG LIB   ==============


# -------------------- core (STATIC library) ---------------------------
add_library(core STATIC)

# Liệt kê TƯỜNG MINH các file .cpp thuộc module core (điền đúng tên file của bạn)
target_sources(core PRIVATE
  src/core/MenuManager.cpp
  src/core/SystemManager.cpp
)

# Khai báo đường dẫn header CHO TARGET (ai link core sẽ kế thừa include/)
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Định nghĩa macro dùng trong code (cho phép sử dụng std::string(DATA_DIR))
target_compile_definitions(core PUBLIC DATA_DIR="${DATA_DIR}")

# -------------------- utils (STATIC library) --------------------------
add_library(utils STATIC)

# Liệt kê TƯỜNG MINH các file .cpp thuộc module utils
target_sources(utils PRIVATE
  src/utils/AuthenticationManager.cpp
  src/utils/Helpers.cpp
  src/utils/Time.cpp
)

# Đường dẫn header + macro cho utils
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(utils PUBLIC DATA_DIR="${DATA_DIR}")

# -------------------- entities (STATIC library) -----------------------
add_library(entities STATIC)

# Liệt kê TƯỜNG MINH các file .cpp thuộc module entities
target_sources(entities PRIVATE
  src/entities/Account.cpp
  src/entities/AccountAdmin.cpp
  src/entities/AccountAgent.cpp
  src/entities/Booking.cpp
  src/entities/Flight.cpp
  src/entities/FlightInstance.cpp
  src/entities/FlightRule.cpp
  src/entities/Passenger.cpp
  src/entities/Promotion.cpp
  src/entities/Ticket.cpp
)

# entities phụ thuộc core, utils (PUBLIC để app/link khác kế thừa)
target_include_directories(entities PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(entities PUBLIC core utils)


# ===================   KHAI BÁO EXECUTABLE (APP)   ===================


# Tạo target exe tên PBL2 (file chạy sẽ là PBL2 hoặc PBL2.exe)
add_executable(PBL2)

# Liệt kê TƯỜNG MINH các file .cpp thuộc app (thường chỉ main.cpp)
target_sources(PBL2 PRIVATE
  app/main.cpp
)

# App link các module
target_include_directories(PBL2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(PBL2 PRIVATE core entities utils)

# Định nghĩa macro DATA_DIR cho app
target_compile_definitions(PBL2 PRIVATE DATA_DIR="${DATA_DIR}")


# ==================   HẬU BIÊN DỊCH (SAU BUILD)   ====================


# Sau khi build, copy toàn bộ thư mục data/ ở source sang thư mục build (đặt cạnh file chạy)
add_custom_command(TARGET PBL2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/data"
          "${CMAKE_BINARY_DIR}/data"
)