cmake_minimum_required(VERSION 3.21)
project(PBL2 LANGUAGES CXX)

# ===== Chuẩn C++ =====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Qt PATH =====
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/mingw_64")

# ===== Bật MOC/RCC =====
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

# Chặn MOC scan file Qt internal (fix lỗi qmetatype.h)
set(CMAKE_AUTOMOC_RELAXED_MODE OFF)

# ===== Tìm Qt với Charts =====
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

if(Qt6Charts_FOUND)
    message(STATUS "Qt6 Charts found: ${Qt6Charts_DIR}")
else()
    message(FATAL_ERROR "Qt6 Charts NOT found!")
endif()

# ===== Thu thập nguồn - LIỆT KÊ RÕ RÀNG =====

# 1. UI Sources (đã chuyển ra ngoài src/)
file(GLOB_RECURSE UI_SOURCES
    "${CMAKE_SOURCE_DIR}/UI/*.cpp"
    "${CMAKE_SOURCE_DIR}/UI/*.h"
)

# 2. Core Sources - LIỆT KÊ THỦ CÔNG ĐỂ CHẮC CHẮN
set(CORE_SOURCES
    "${CMAKE_SOURCE_DIR}/src/core/AccountManager.cpp"
    "${CMAKE_SOURCE_DIR}/include/core/AccountManager.h"
    "${CMAKE_SOURCE_DIR}/src/core/AirportManager.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/BookingManager.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/FlightManager.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/PassengerManager.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/ReportManager.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/SeatManager.cpp"
)

# 3. Entity Sources
file(GLOB_RECURSE ENTITY_SOURCES  
    "${CMAKE_SOURCE_DIR}/src/entities/*.cpp"
)

# 4. Utils Sources
file(GLOB_RECURSE UTILS_SOURCES   
    "${CMAKE_SOURCE_DIR}/src/utils/*.cpp"
)

# ===== Debug: In ra danh sách file =====
message(STATUS "=== UI_SOURCES (${CMAKE_SOURCE_DIR}/UI/) ===")
foreach(src ${UI_SOURCES})
    message(STATUS "  ${src}")
endforeach()

message(STATUS "=== CORE_SOURCES ===")
foreach(src ${CORE_SOURCES})
    message(STATUS "  ${src}")
endforeach()

# ===== Executable chính =====
add_executable(pbl2
    ${UI_SOURCES}
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    ${UTILS_SOURCES}
)

# ===== Include paths =====
target_include_directories(pbl2 PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/core"
    "${CMAKE_SOURCE_DIR}/include/entities"
    "${CMAKE_SOURCE_DIR}/include/DSA"
    "${CMAKE_SOURCE_DIR}/include/utils"
    "${CMAKE_SOURCE_DIR}/UI"  # THÊM ĐỂ TÌM HEADER UI
)

# ===== Link Qt =====
target_link_libraries(pbl2 PRIVATE 
    Qt6::Core
    Qt6::Widgets 
    Qt6::Charts
)

# ===== Đầu ra exe =====
set_target_properties(pbl2 PROPERTIES
    OUTPUT_NAME "PBL2"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# ===== Bundle Qt DLL =====
if(WIN32)
    add_custom_command(TARGET pbl2 POST_BUILD
        COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe"
                --no-translations --compiler-runtime
                "$<TARGET_FILE:pbl2>"
        COMMENT "Running windeployqt")
endif()