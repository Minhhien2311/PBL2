# Tên trình biên dịch
CXX = g++

# Thư mục build để chứa file object và file thực thi
BUILD_DIR = build
TARGET = $(BUILD_DIR)/main.exe

# Cờ biên dịch: -g để debug, -Wall để hiện tất cả cảnh báo
CXXFLAGS = -g -Wall

# Các thư mục chứa file header
INCLUDES = -IC:/PBL2/include -IC:/PBL2/lib/SFML-2.6.1/include

# Các thư mục chứa thư viện
LIBDIRS = -LC:/PBL2/lib/SFML-2.6.1/lib

# Các thư viện cần link (phiên bản debug)
LIBS = -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -lsfml-main-d

# Tự động tìm tất cả các file .cpp trong các thư mục con của src và app
SRCS := $(wildcard app/main.cpp) $(wildcard src/core/*.cpp) $(wildcard src/entities/*.cpp) $(wildcard src/UI/components/*.cpp) $(wildcard src/UI/states/*.cpp) $(wildcard src/utils/*.cpp)

# Tạo danh sách các file object (.o) tương ứng trong thư mục build
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Rule mặc định: build tất cả
all: $(TARGET)

# Rule để link các file object thành file thực thi
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJS) -o $@ $(LIBDIRS) $(LIBS)
	@echo "Build successful! Executable is at $(TARGET)"

# Rule để biên dịch file .cpp thành file .o
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule để dọn dẹp thư mục build
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

.PHONY: all clean