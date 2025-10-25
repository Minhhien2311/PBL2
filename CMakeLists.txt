cmake_minimum_required(VERSION 3.20)
project(PBL2 LANGUAGES CXX)

# ====== Thiết lập chung cho dự án ======
set(CMAKE_CXX_STANDARD 20)                    # Dùng C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)         # Xuất compile_commands.json (cho IDE / phân tích tĩnh)

# Cảnh báo biên dịch
if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Biến macro trỏ tới thư mục data trong thư mục build (để code mở file runtime)
set(DATA_DIR "${CMAKE_BINARY_DIR}/data")

# ====== Tuỳ chọn UI backend ======
# BẬT SFML (ON) để build giao diện bằng SFML; TẮT (OFF) để chuyển sang FTXUI.
option(USE_SFML "Dùng SFML làm giao diện (nếu OFF thì dùng FTXUI)" ON)

# =========================================================
# =============     KHAI BÁO CÁC LIB CỦA APP     =========
# =========================================================
add_library(core STATIC)
target_sources(core PRIVATE
  src/core/MenuManager.cpp
  src/core/AccountManager.cpp
  src/core/BookingManager.cpp
  src/core/FlightManager.cpp
)
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(core PUBLIC DATA_DIR="${DATA_DIR}")

add_library(utils STATIC)
target_sources(utils PRIVATE
  src/utils/Helpers.cpp
  src/utils/Time.cpp
  src/utils/GenID.cpp
)
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(utils PUBLIC DATA_DIR="${DATA_DIR}")

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
target_link_libraries(entities PUBLIC core utils)

# =========================================================
# =====================     APP     =======================
# =========================================================
add_executable(PBL2)
target_sources(PBL2 PRIVATE
  app/main.cpp
  src/interface/Admin_Interface.cpp
  src/interface/Agent_Interface.cpp
  src/interface/Login_Interface.cpp
)
target_compile_definitions(PBL2 PRIVATE DATA_DIR="${DATA_DIR}")
target_link_libraries(PBL2 PRIVATE entities)

# Đặt nơi sinh file exe = build/ (không vào build/bin)
set_target_properties(PBL2 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# =========================================================
# ============   LIÊN KẾT UI BACKEND (SFML)   =============
# =========================================================
if (USE_SFML)
  # ---- Nhúng mã nguồn SFML 2.6.1 vào build/ hiện tại ----
  # Yêu cầu trong cây nguồn có thư mục ${CMAKE_SOURCE_DIR}/SFML (đã clone).
  #
  # Dùng DLL, tắt example/test; DÙNG deps hệ thống (UCRT) và BẬT Audio.
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
  set(SFML_USE_STATIC_STD_LIBS OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_AUDIO ON  CACHE BOOL "" FORCE)
  set(SFML_USE_SYSTEM_DEPS ON CACHE BOOL "" FORCE)

  add_subdirectory(${CMAKE_SOURCE_DIR}/SFML)

  # Link các module cần thiết (có cả audio)
  target_link_libraries(PBL2 PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)
  target_compile_definitions(PBL2 PRIVATE UI_BACKEND_SFML=1)

  # --- Copy ĐÚNG DLL của các target SFML về cạnh PBL2.exe (tự khớp Debug/Release & đường dẫn) ---
  foreach(lib IN ITEMS sfml-graphics sfml-window sfml-system sfml-audio)
    if (TARGET ${lib})
      add_custom_command(TARGET PBL2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${lib}>  $<TARGET_FILE_DIR:PBL2>
        VERBATIM)
    endif()
  endforeach()

  # --- (Tuỳ chọn) Nếu KHÔNG thêm C:\msys64\ucrt64\bin vào PATH, copy thêm DLL UCRT phụ thuộc ---
  # set(UCRT_BIN "C:/msys64/ucrt64/bin")
  # if (EXISTS "${UCRT_BIN}")
  #   foreach(dll IN ITEMS
  #     libopenal-1.dll
  #     libFLAC-*.dll
  #     libvorbis-*.dll libvorbisfile-*.dll
  #     libogg-0.dll
  #     libfreetype-6.dll)
  #     add_custom_command(TARGET PBL2 POST_BUILD
  #       COMMAND ${CMAKE_COMMAND} -E copy_if_different
  #               "${UCRT_BIN}/${dll}" "$<TARGET_FILE_DIR:PBL2>"
  #       VERBATIM)
  #   endforeach()
  # endif()
else()
  # add_subdirectory(ftxui)  # nếu sau này dùng FTXUI
  # target_link_libraries(PBL2 PRIVATE ftxui::screen ftxui::dom ftxui::component)
  target_compile_definitions(PBL2 PRIVATE UI_BACKEND_FTXUI=1)
endif()

# =========================================================
# ================     HẬU BIÊN DỊCH     =================
# =========================================================
# Copy toàn bộ thư mục data/ sang build/ để chạy runtime
add_custom_command(TARGET PBL2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/data"
          "${CMAKE_BINARY_DIR}/data"
)
