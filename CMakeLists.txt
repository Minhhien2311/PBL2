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

# ===== Tìm Qt với Charts =====
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

# Kiểm tra nếu tìm thấy Charts
if(Qt6Charts_FOUND)
    message(STATUS "Qt6 Charts found: ${Qt6Charts_DIR}")
else()
    message(FATAL_ERROR "Qt6 Charts NOT found! Please install Qt Charts module.")
endif()

# ===== Thu thập nguồn =====
file(GLOB_RECURSE UI_SOURCES
    "src/UI/*.cpp"
    "src/UI/*.h"
)

file(GLOB_RECURSE CORE_SOURCES 
    "src/core/*.cpp"
    "include/core/*.h"    # ← THÊM DÒNG NÀY
)
file(GLOB_RECURSE ENTITY_SOURCES  "src/entities/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES   "src/utils/*.cpp")

# ===== Executable chính =====
add_executable(pbl2 WIN32
    ${UI_SOURCES}
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    ${UTILS_SOURCES}
    ${ADDITIONAL_SOURCES}
)

# ===== Include paths =====
target_include_directories(pbl2 PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/core"
    "${CMAKE_SOURCE_DIR}/include/entities"
    "${CMAKE_SOURCE_DIR}/include/DSA"
    "${CMAKE_SOURCE_DIR}/include/utils"
)

# ===== Link Qt - THÊM CHARTS =====
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

# Copy data/ only if build/data doesn't exist yet
if(EXISTS "${CMAKE_SOURCE_DIR}/data" AND NOT EXISTS "${CMAKE_BINARY_DIR}/data")
  add_custom_command(TARGET pbl2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/data"
            "${CMAKE_BINARY_DIR}/data"
    COMMENT "Initial copy: data/ -> build/data/")
endif()

# ===== Bundle Qt DLL =====
if(WIN32)
  add_custom_command(TARGET pbl2 POST_BUILD
    COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe"
            --no-translations --compiler-runtime
            "$<TARGET_FILE:pbl2>"
    COMMENT "Running windeployqt (copy Qt DLLs cạnh PBL2.exe)")
endif()