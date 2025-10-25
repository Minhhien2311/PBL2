cmake_minimum_required(VERSION 3.20)
project(PBL2 LANGUAGES CXX)

# ====== Thiết lập chung cho dự án ======
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(DATA_DIR "${CMAKE_BINARY_DIR}/data")
set(ASSETS_DIR "${CMAKE_BINARY_DIR}/assets")

# ====== Tuỳ chọn UI backend ======
option(USE_SFML "Dùng SFML làm giao diện (nếu OFF thì dùng FTXUI)" ON)

# =========================================================
# =============   KHAI BÁO CÁC LIB CỦA APP    =========
# =========================================================

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
target_link_libraries(ui_states PUBLIC core ui_components entities) # Đã link các thư viện cần thiết trực tiếp

# =========================================================
# =====================     APP     =======================
# =========================================================
add_executable(PBL2)
target_sources(PBL2 PRIVATE
  app/main.cpp
)
target_compile_definitions(PBL2 PRIVATE DATA_DIR="${DATA_DIR}" ASSETS_DIR="${ASSETS_DIR}")
# Link tất cả các thư viện tĩnh cần thiết trực tiếp vào executable
# target_link_libraries(PBL2 PRIVATE ui_states) # Thay thế dòng này bằng dòng dưới

set_target_properties(PBL2 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  WIN32_EXECUTABLE TRUE
)

# =========================================================
# ============   LIÊN KẾT UI BACKEND (SFML)   =============
# =========================================================
if (USE_SFML)
  # --- Cấu hình build SFML từ source ---
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
  set(SFML_USE_STATIC_STD_LIBS OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_AUDIO ON  CACHE BOOL "" FORCE)
  set(SFML_USE_SYSTEM_DEPS ON CACHE BOOL "" FORCE)

  if (EXISTS "${CMAKE_SOURCE_DIR}/SFML/CMakeLists.txt")
      add_subdirectory(${CMAKE_SOURCE_DIR}/SFML)
  else()
      message(FATAL_ERROR "Thư mục mã nguồn SFML không tìm thấy tại ${CMAKE_SOURCE_DIR}/SFML")
  endif()

  # --- Định nghĩa INTERFACE library cho SFML ---
  add_library(sfml_libs INTERFACE)
  if(WIN32)
      target_link_libraries(sfml_libs INTERFACE sfml-graphics sfml-window sfml-system sfml-audio sfml-main)
  else()
      target_link_libraries(sfml_libs INTERFACE sfml-graphics sfml-window sfml-system sfml-audio)
  endif()

  # --- Link SFML vào các thư viện của bạn CẦN nó ---
  target_link_libraries(core PUBLIC sfml_libs)
  target_link_libraries(ui_components PUBLIC sfml_libs)
  target_link_libraries(ui_states PUBLIC sfml_libs)

  # --- Link tất cả thư viện cần thiết vào App chính MỘT CÁCH TƯỜNG MINH ---
  target_link_libraries(PBL2 PRIVATE ui_states core ui_components entities utils sfml_libs)

  target_compile_definitions(PBL2 PRIVATE UI_BACKEND_SFML=1)

  # --- Copy DLL SFML ---
  set(SFML_DLL_TARGETS sfml-graphics sfml-window sfml-system sfml-audio)
  # sfml-main là thư viện tĩnh, không cần copy DLL

  foreach(lib IN LISTS SFML_DLL_TARGETS)
    if (TARGET ${lib})
      get_target_property(target_type ${lib} TYPE)
      if(target_type STREQUAL "SHARED_LIBRARY")
        add_custom_command(TARGET PBL2 POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  $<TARGET_FILE:${lib}>  $<TARGET_FILE_DIR:PBL2>
          COMMENT "Copying ${lib} DLL"
          VERBATIM)
      endif()
    endif()
  endforeach()

  # --- Copy DLL phụ thuộc (OpenAL) ---
  find_file(OPENAL_DLL_PATH openal32.dll HINTS
      "${CMAKE_BINARY_DIR}/lib"
      "${CMAKE_BINARY_DIR}/bin"
      "${CMAKE_BINARY_DIR}/SFML/lib/openal-soft-build"
      "${CMAKE_BINARY_DIR}/SFML/bin"
      "${CMAKE_INSTALL_PREFIX}/bin"
      "${CMAKE_SOURCE_DIR}/SFML/extlibs/bin/x64"
      "${CMAKE_SOURCE_DIR}/SFML/extlibs/bin/x86"
      "$ENV{SystemRoot}/System32"
      NO_DEFAULT_PATH
      DOC "Path to OpenAL32.dll"
      )

  if (OPENAL_DLL_PATH)
      message(STATUS "Found OpenAL DLL: ${OPENAL_DLL_PATH}")
      add_custom_command(TARGET PBL2 POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  "${OPENAL_DLL_PATH}" "$<TARGET_FILE_DIR:PBL2>"
          COMMENT "Copying OpenAL DLL"
          VERBATIM)
  else()
      message(WARNING "openal32.dll not found automatically. Audio might not work. Please copy it manually to the executable directory or adjust HINTS in find_file(OPENAL_DLL_PATH...) in CMakeLists.txt.")
  endif()

else()
  # ... (phần FTXUI) ...
endif()

# =========================================================
# ================     HẬU BIÊN DỊCH     =================
# =========================================================
# Copy data/
add_custom_command(TARGET PBL2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/data"
          "${DATA_DIR}"
  COMMENT "Copying data directory"
)
# Copy assets/
add_custom_command(TARGET PBL2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/assets"
          "${ASSETS_DIR}"
  COMMENT "Copying assets directory"
)

target_include_directories(PBL2 PRIVATE ${CMAKE_SOURCE_DIR}/include)

