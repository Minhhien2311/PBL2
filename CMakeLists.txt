cmake_minimum_required(VERSION 3.21)
project(PBL2 LANGUAGES CXX)

# ===== Chuẩn C++ =====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Qt PATH (sửa nếu khác) =====
# ví dụ: C:/Qt/6.9.3/mingw_64
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/mingw_64")

# ===== Bật MOC/RCC (không dùng UIC vì KHÔNG có .ui) =====
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# (Giữ AUTOUIC OFF cho sạch)
set(CMAKE_AUTOUIC OFF)

# ===== Tìm Qt =====
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ===== Thu thập nguồn =====
# UI: chứa cả .h và .cpp trong src/UI/
file(GLOB_RECURSE UI_SOURCES
    "src/UI/*.cpp"
    "src/UI/*.h"
)

# Core / Entities / Utils trong src/
file(GLOB_RECURSE CORE_SOURCES    "src/core/*.cpp")
file(GLOB_RECURSE ENTITY_SOURCES  "src/entities/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES   "src/utils/*.cpp")

# Ensure new files are included
set(ADDITIONAL_SOURCES
    "src/entities/Seat.cpp"
    "src/core/SeatManager.cpp"
)

# ===== Executable chính (WIN32 để không mở console) =====
add_executable(pbl2 WIN32
    ${UI_SOURCES}
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    ${UTILS_SOURCES}
    ${ADDITIONAL_SOURCES}
)

# ===== Include paths (headers ở include/...) =====
target_include_directories(pbl2 PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/core"
    "${CMAKE_SOURCE_DIR}/include/entities"
    "${CMAKE_SOURCE_DIR}/include/DSA"
    "${CMAKE_SOURCE_DIR}/include/utils"
)

# ===== Link Qt =====
target_link_libraries(pbl2 PRIVATE Qt6::Widgets)

# ===== Đầu ra exe: build/PBL2.exe =====
set_target_properties(pbl2 PROPERTIES
    OUTPUT_NAME "PBL2"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# ===== Copy data/ nếu có =====
if(EXISTS "${CMAKE_SOURCE_DIR}/data")
  add_custom_command(TARGET pbl2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/data"
            "${CMAKE_BINARY_DIR}/data"
    COMMENT "Copy data/ -> build/data")
endif()

# ===== Bundle Qt DLL cạnh PBL2.exe (Windows) =====
if(WIN32)
  add_custom_command(TARGET pbl2 POST_BUILD
    COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe"
            --no-translations --compiler-runtime
            "$<TARGET_FILE:pbl2>"
    COMMENT "Running windeployqt (copy Qt DLLs cạnh PBL2.exe)")
endif()

# <<<<<<< HEAD
# # Biến macro trỏ tới thư mục data trong thư mục build (để code mở file dữ liệu runtime)
# set(DATA_DIR "${CMAKE_BINARY_DIR}/data")

# # Thêm thư mục con ftxui vào quá trình build
# add_subdirectory(ftxui)

# # ===============   KHAI BÁO CÁC MODULE DƯỚI DẠNG LIB   ==============


# # -------------------- core (STATIC library) ---------------------------
# add_library(core STATIC)

# # Liệt kê TƯỜNG MINH các file .cpp thuộc module core (điền đúng tên file của bạn)
# target_sources(core PRIVATE
#   src/core/MenuManager.cpp
#   src/core/AccountManager.cpp
#   src/core/BookingManager.cpp
#   src/core/FlightManager.cpp
#   src/core/ReportManager.cpp
# )

# # Khai báo đường dẫn header CHO TARGET (ai link core sẽ kế thừa include/)
# target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)

# # Định nghĩa macro dùng trong code (cho phép sử dụng std::string(DATA_DIR))
# target_compile_definitions(core PUBLIC DATA_DIR="${DATA_DIR}")

# # -------------------- utils (STATIC library) --------------------------
# add_library(utils STATIC)

# # Liệt kê TƯỜNG MINH các file .cpp thuộc module utils
# target_sources(utils PRIVATE
#   src/utils/Helpers.cpp
#   src/utils/Time.cpp
#   src/utils/GenID.cpp
# )

# # Đường dẫn header + macro cho utils
# target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
# target_compile_definitions(utils PUBLIC DATA_DIR="${DATA_DIR}")

# # -------------------- entities (STATIC library) -----------------------
# add_library(entities STATIC)

# # Liệt kê TƯỜNG MINH các file .cpp thuộc module entities
# target_sources(entities PRIVATE
#   src/entities/Account.cpp
#   src/entities/AccountAdmin.cpp
#   src/entities/AccountAgent.cpp
#   src/entities/Booking.cpp
#   src/entities/Flight.cpp
#   src/entities/FlightInstance.cpp
#   src/entities/FlightRule.cpp
#   src/entities/Passenger.cpp
#   src/entities/Promotion.cpp
# )

# # entities phụ thuộc core, utils (PUBLIC để app/link khác kế thừa)
# target_include_directories(entities PUBLIC ${CMAKE_SOURCE_DIR}/include)
# target_link_libraries(entities PUBLIC core utils)


# # ===================   KHAI BÁO EXECUTABLE (APP)   ===================


# # Tạo target exe tên PBL2 (file chạy sẽ là PBL2 hoặc PBL2.exe)
# add_executable(PBL2)

# # Liệt kê TƯỜNG MINH các file .cpp thuộc app (thường chỉ main.cpp)
# target_sources(PBL2 PRIVATE
#   app/main.cpp

#     # THÊM CÁC FILE GIAO DIỆN VÀO ĐÂY
#   src/interface/Admin_Interface.cpp
#   src/interface/Agent_Interface.cpp
#   src/interface/Login_Interface.cpp
# )

# # App link các module
# # App link các module và thư viện FTXUI
# target_include_directories(PBL2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
# target_link_libraries(PBL2 PRIVATE 
#     core 
#     entities 
#     utils
#     ftxui::component
#     ftxui::screen
# )

# # Định nghĩa macro DATA_DIR cho app
# target_compile_definitions(PBL2 PRIVATE DATA_DIR="${DATA_DIR}")


# # ==================   HẬU BIÊN DỊCH (SAU BUILD)   ====================

# # ...

# # Sau khi build, copy toàn bộ thư mục data/ ở source sang thư mục build (đặt cạnh file chạy)
# add_custom_command(TARGET PBL2 POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory
#           "${CMAKE_SOURCE_DIR}/data"
#           "${CMAKE_BINARY_DIR}/data"
# )

# =======
# # ===== Bundle Qt DLL cạnh PBL2.exe (Windows) =====
# if(WIN32)
#   add_custom_command(TARGET pbl2 POST_BUILD
#     COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe"
#             --no-translations --compiler-runtime
#             "$<TARGET_FILE:pbl2>"
#     COMMENT "Running windeployqt (copy Qt DLLs cạnh PBL2.exe)")
# endif()
# >>>>>>> 35cb79dcedacc14b6b9c456b2b871c6a81cd4b30
