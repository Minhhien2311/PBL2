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

# ===== Executable chính (WIN32 để không mở console) =====
add_executable(pbl2 WIN32
    ${UI_SOURCES}
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    ${UTILS_SOURCES}
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
target_link_libraries(pbl2 PRIVATE 
Qt6::Widgets
mingw32
)

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