cmake_minimum_required(VERSION 3.20)
project(PBL2 LANGUAGES CXX)

# ====== Thiết lập chung cho dự án ======
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Xác định Build Type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-g) # Thêm -g cho Debug
  else()
    add_compile_options(-O3) # Thêm tối ưu hóa -O3 cho Release
  endif()
endif()

set(DATA_DIR "${CMAKE_BINARY_DIR}/data")
set(ASSETS_DIR "${CMAKE_BINARY_DIR}/assets")

# ====== Tuỳ chọn UI backend ======
option(USE_SFML "Dùng SFML làm giao diện (nếu OFF thì dùng FTXUI)" ON)

# =========================================================
# =============   KHAI BÁO CÁC LIB CỦA APP    =========
# =========================================================
# (Các thư viện utils, entities, core, ui_components, ui_states giữ nguyên)
# --- LIB: utils ---
add_library(utils STATIC)
target_sources(utils PRIVATE
  src/utils/Helpers.cpp
  src/utils/Time.cpp
  src/utils/GenID.cpp
)
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(utils PUBLIC DATA_DIR="${DATA_DIR}")

# --- LIB: entities ---
add_library(entities STATIC)
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
)
target_include_directories(entities PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(entities PUBLIC utils)

# --- LIB: core ---
add_library(core STATIC)
target_sources(core PRIVATE
  src/core/AccountManager.cpp
  src/core/BookingManager.cpp
  src/core/FlightManager.cpp
  src/core/App.cpp
)
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(core PUBLIC DATA_DIR="${DATA_DIR}" ASSETS_DIR="${ASSETS_DIR}")
target_link_libraries(core PUBLIC entities) # entities đã link utils

# --- LIB: ui_components ---
add_library(ui_components STATIC)
target_sources(ui_components PRIVATE
  src/UI/components/Button.cpp
  src/UI/components/TextBox.cpp
)
target_include_directories(ui_components PUBLIC ${CMAKE_SOURCE_DIR}/include)

# --- LIB: ui_states ---
add_library(ui_states STATIC)
target_sources(ui_states PRIVATE
  src/UI/states/LoginState.cpp
)
target_include_directories(ui_states PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ui_states PUBLIC core ui_components entities) 

# =========================================================
# =====================     APP     =======================
# =========================================================
add_executable(PBL2)
target_sources(PBL2 PRIVATE
  app/main.cpp
)
target_compile_definitions(PBL2 PRIVATE DATA_DIR="${DATA_DIR}" ASSETS_DIR="${ASSETS_DIR}")

set_target_properties(PBL2 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  WIN32_EXECUTABLE TRUE
)

# =========================================================
# ============   LIÊN KẾT UI BACKEND (SFML)   =============
# =========================================================
if (USE_SFML)
  
  # --- TÌM SFML ĐÃ TẢI SẴN (PRE-COMPILED) ---
  set(SFML_DIR "${CMAKE_SOURCE_DIR}/lib/SFML-2.6.1") 

  if(NOT EXISTS "${SFML_DIR}/include/SFML/Graphics.hpp")
    message(FATAL_ERROR "Không tìm thấy SFML headers tại ${SFML_DIR}/include. Bạn đã tải bản pre-compiled và chép vào lib/SFML-2.6.1 chưa?")
  endif()
  
  message(STATUS "Using pre-compiled SFML from: ${SFML_DIR}")

  # Thêm đường dẫn Include của SFML vào TẤT CẢ các thư viện con CẦN nó
  target_include_directories(core PUBLIC "${SFML_DIR}/include")
  target_include_directories(ui_components PUBLIC "${SFML_DIR}/include")
  target_include_directories(ui_states PUBLIC "${SFML_DIR}/include")
  target_include_directories(PBL2 PRIVATE "${SFML_DIR}/include") 

  # Thêm đường dẫn Lib của SFML cho target chính
  target_link_directories(PBL2 PRIVATE "${SFML_DIR}/lib")
  
  # --- Định nghĩa thư viện SFML cần link dựa trên Build Type ---
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Linking SFML Debug libraries (-d)")
    set(SFML_LIBRARIES
        sfml-graphics-d
        sfml-window-d
        sfml-audio-d
        sfml-system-d
    )
    if(WIN32)
      list(APPEND SFML_LIBRARIES sfml-main-d)
    endif()
  else()
    message(STATUS "Linking SFML Release libraries")
    set(SFML_LIBRARIES
        sfml-graphics
        sfml-window
        sfml-audio
        sfml-system
    )
    if(WIN32)
      list(APPEND SFML_LIBRARIES sfml-main)
    endif()
  endif()
  
  # --- Link mọi thứ vào App chính ---
  target_link_libraries(PBL2 PRIVATE
      # --- SỬA LỖI PHỤ THUỘC VÒNG ---
      -Wl,--start-group
      ui_states       # Thư viện của bạn
      core
      ui_components
      entities
      utils
      -Wl,--end-group
      # --- HẾT SỬA LỖI ---
      
      ${SFML_LIBRARIES} # Link các thư viện SFML (đặt sau cùng)
  )

  target_compile_definitions(PBL2 PRIVATE UI_BACKEND_SFML=1)

  # --- Copy DLL SFML (từ thư mục bin của bản pre-compiled) ---
  # Khối này sẽ copy CẢ sfml-*.dll VÀ libstdc++*.dll (phiên bản 13.1.0)
  set(SFML_BIN_DIR "${SFML_DIR}/bin")
  if(EXISTS "${SFML_BIN_DIR}")
    # Lấy danh sách TẤT CẢ các file .dll trong thư mục bin
    file(GLOB SFML_DLLS "${SFML_BIN_DIR}/*.dll")
    message(STATUS "Found SFML DLLs: ${SFML_DLLS}")
    
    foreach(dll_file IN LISTS SFML_DLLS)
      add_custom_command(TARGET PBL2 POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  "${dll_file}" "$<TARGET_FILE_DIR:PBL2>"
          COMMENT "Copying ${dll_file}"
          VERBATIM)
    endforeach()
  else()
    message(WARNING "SFML bin directory not found at ${SFML_BIN_DIR}. DLLs must be copied manually.")
  endif()
  
else()
  # ... (phần FTXUI) ...
endif()

# =========================================================
# ================     HẬU BIÊN DỊCH     =================
# =========================================================
# Copy data/ và assets/
add_custom_command(TARGET PBL2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data" "${DATA_DIR}"
  COMMENT "Copying data directory"
)
add_custom_command(TARGET PBL2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${ASSETS_DIR}"
  COMMENT "Copying assets directory"
)

# Thêm include gốc vào PBL2
target_include_directories(PBL2 PRIVATE ${CMAKE_SOURCE_DIR}/include)

