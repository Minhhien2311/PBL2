# ================== WINDOWS (PowerShell/CMD) ==================
SHELL := cmd.exe
.SHELLFLAGS := /Q /C

# --- Helper: đổi path / -> \ khi gọi lệnh Windows
winpath = $(subst /,\,$(1))

# Compiler
CXX := C:/mingw64/bin/g++.exe
STD := -std=c++17

# Output
BUILD_DIR := build
OBJ_DIR   := obj
TARGET    := $(BUILD_DIR)/PBL2.exe

# Debug/Release
DBG ?= 1
ifeq ($(DBG),1)
  CXXFLAGS := $(STD) -Wall -Wextra -O0 -g -D_DEBUG
else
  CXXFLAGS := $(STD) -Wall -Wextra -O2
  LDFLAGS  := -mwindows
endif

# Vị trí MinGW bin để lấy runtime DLL
MINGW_BIN := C:/mingw64/bin

ASSETS_DIR := assets

# SFML
SFML_ROOT := C:/PBL2/lib/SFML-2.6.1
INCLUDES  := -IC:/PBL2/include -I"$(SFML_ROOT)/include"
LIBDIRS   := -L"$(SFML_ROOT)/lib"

# 0 = lib không có hậu tố -d; 1 = lib có -d
USE_SFML_DEBUG_LIBS ?= 0
ifeq ($(USE_SFML_DEBUG_LIBS),1)
  LIBS := -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -lsfml-main-d
else
  LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main
endif

# ================== QUÉT NGUỒN ==================
SRCS := \
  app/main.cpp \
  $(wildcard src/core/*.cpp) \
  $(wildcard src/entities/*.cpp) \
  $(wildcard src/UI/components/*.cpp) \
  $(wildcard src/UI/states/*.cpp) \
  $(wildcard src/UI/pages/*.cpp) \
  $(wildcard src/utils/*.cpp)

OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# ================== RULES ==================
.PHONY: all run clean print dlls assets    # <— bổ sung 'assets' ở đây

all: $(TARGET)

# Link: đảm bảo build/ tồn tại
$(TARGET): $(OBJS)
	@if not exist "$(call winpath,$(BUILD_DIR))" mkdir "$(call winpath,$(BUILD_DIR))"
	@echo [LD] $@
	@"$(CXX)" $(OBJS) -o "$(call winpath,$@)" $(LIBDIRS) $(LIBS) $(LDFLAGS)
	@$(MAKE) dlls --no-print-directory
	@$(MAKE) assets --no-print-directory
	@echo Build successful! Executable at $(TARGET)

# Compile: tạo cây obj/... nếu thiếu
$(OBJ_DIR)/%.o: %.cpp
	@for %%d in ("$(call winpath,$(dir $@))") do if not exist "%%~d" mkdir "%%~d"
	@echo [CXX] $<
	@"$(CXX)" $(CXXFLAGS) $(INCLUDES) -c "$(call winpath,$<)" -o "$(call winpath,$@)"

# Copy DLL SFML vào build/
# --- thay toàn bộ rule dlls bằng đoạn này ---
dlls:
	@echo [DLL] Copy SFML ^& MinGW runtime DLLs to $(BUILD_DIR)
	@REM ===== SFML (release + debug) =====
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-graphics-2.dll)"    copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-graphics-2.dll)"    "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-graphics-d-2.dll)"  copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-graphics-d-2.dll)"  "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-window-2.dll)"      copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-window-2.dll)"      "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-window-d-2.dll)"    copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-window-d-2.dll)"    "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-system-2.dll)"      copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-system-2.dll)"      "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-system-d-2.dll)"    copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-system-d-2.dll)"    "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-audio-2.dll)"       copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-audio-2.dll)"       "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-audio-d-2.dll)"     copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-audio-d-2.dll)"     "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-network-2.dll)"     copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-network-2.dll)"     "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/sfml-network-d-2.dll)"   copy /Y "$(call winpath,$(SFML_ROOT)/bin/sfml-network-d-2.dll)"   "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(SFML_ROOT)/bin/openal32.dll)"           copy /Y "$(call winpath,$(SFML_ROOT)/bin/openal32.dll)"           "$(call winpath,$(BUILD_DIR))\" >NUL
	@REM ===== MinGW runtime (nếu link động) =====
	@if exist "$(call winpath,$(MINGW_BIN)/libstdc++-6.dll)"            copy /Y "$(call winpath,$(MINGW_BIN)/libstdc++-6.dll)"            "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(MINGW_BIN)/libgcc_s_seh-1.dll)"         copy /Y "$(call winpath,$(MINGW_BIN)/libgcc_s_seh-1.dll)"         "$(call winpath,$(BUILD_DIR))\" >NUL
	@if exist "$(call winpath,$(MINGW_BIN)/libwinpthread-1.dll)"        copy /Y "$(call winpath,$(MINGW_BIN)/libwinpthread-1.dll)"        "$(call winpath,$(BUILD_DIR))\" >NUL

assets:
	@echo [ASSETS] Sync assets to $(BUILD_DIR)\assets
	@if exist "$(call winpath,$(ASSETS_DIR))" xcopy "$(call winpath,$(ASSETS_DIR))" "$(call winpath,$(BUILD_DIR))/assets/" /E /I /Y >NUL

run: all
	@cd "$(call winpath,$(BUILD_DIR))" && ".\PBL2.exe"

clean:
	@echo [CLEAN] removing build and obj
	@if exist "$(call winpath,$(BUILD_DIR))" rmdir /S /Q "$(call winpath,$(BUILD_DIR))"
	@if exist "$(call winpath,$(OBJ_DIR))"   rmdir /S /Q "$(call winpath,$(OBJ_DIR))"

print:
	@echo === SRCS ===
	@for %%f in ($(SRCS)) do @echo  %%f
	@echo === OBJS ===
	@for %%f in ($(OBJS)) do @echo  %%f

# ================== COPY DATA FOLDER ==================
# Thư mục dữ liệu nguồn
DATA_DIR := data

# Bổ sung phụ thuộc: gọi 'data' trước khi chạy recipe 'dlls'
dlls: data

.PHONY: data
data:
	@echo [DATA] Sync data to $(BUILD_DIR)\data
	@if exist "$(call winpath,$(DATA_DIR))" xcopy "$(call winpath,$(DATA_DIR))" "$(call winpath,$(BUILD_DIR))/data/" /E /I /Y >NUL
