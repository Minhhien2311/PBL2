# --- Compiler and Flags ---
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
#CXXFLAGS = -std=c++17 -Wall -Wextra -O2 # Example for release build

# --- Directories (Based on your structure image_2eb960.png) ---
INCLUDE_ROOT = include
EXTEND_DIR = Extend
SFML_INCLUDE_DIR = lib/SFML-2.6.1/include

# Explicitly list ALL header directories
IDIRS = $(INCLUDE_ROOT) \
        $(INCLUDE_ROOT)/core \
        $(INCLUDE_ROOT)/DSA \
        $(INCLUDE_ROOT)/entities \
        $(INCLUDE_ROOT)/UI \
        $(INCLUDE_ROOT)/UI/components \
        $(INCLUDE_ROOT)/UI/states \
        $(INCLUDE_ROOT)/utils \
        $(EXTEND_DIR) \
        $(SFML_INCLUDE_DIR)

APP_DIR = app
SRC_DIR = src
# Explicitly list ALL source directories where .cpp files reside
SRCDIRS = $(APP_DIR) \
          $(SRC_DIR)/core \
          $(SRC_DIR)/entities \
          $(SRC_DIR)/UI/components \
          $(SRC_DIR)/UI/states \
          $(SRC_DIR)/utils \
          $(EXTEND_DIR)

# VPATH tells make where to find source files based on SRCDIRS listing
VPATH = $(SRCDIRS)

# Build directories
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
# Output executable name (As requested: PBL2)
TARGET_NAME = PBL2
TARGET = $(BUILDDIR)/$(TARGET_NAME)
# Add .exe for Windows automatically
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    MKDIR_P = mkdir -p
    CP = cp -r
    RM = rm -rf
else
    MKDIR_P = mkdir -p
	CP = cp -r
	RM = rm -rf
endif

# --- SFML Configuration ---
SFML_LIB_PATH = lib/SFML-2.6.1/lib
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
SFML_DLL_PATH = lib/SFML-2.6.1/bin

# --- Compiler/Linker Flags ---
INCLUDES = $(addprefix -I,$(IDIRS))
LDFLAGS = -L$(SFML_LIB_PATH)
LDLIBS = $(SFML_LIBS)

# --- Find Source Files and Define Object Files ---
# Find all .cpp files ONLY in the directories listed in SRCDIRS
SRC = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
# Make SRC unique
SRC := $(sort $(SRC))
# Create object file names in OBJDIR (build/obj/main.o, build/obj/App.o, ...)
OBJ = $(addprefix $(OBJDIR)/, $(notdir $(SRC:.cpp=.o)))

# --- Debug Print ---
# These lines will print when 'make' starts processing the Makefile
$(info ==========================================================)
$(info Source Dirs (SRCDIRS): $(SRCDIRS))
$(info Source Files (SRC)  : $(SRC))
$(info Object Files (OBJ) : $(OBJ))
$(info ==========================================================)

# --- Rules ---

# Default target: build everything
all: $(TARGET) copy_resources copy_dlls

# Linking rule: create the executable from object files
$(TARGET): $(OBJ) | $(BUILDDIR)
	@echo "Linking..."
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)
	@echo "Build successful: $@"

# Single Compilation Rule using VPATH
# make will search for %.cpp in directories listed in VPATH
# $< is the first prerequisite (the .cpp file found via VPATH)
# $@ is the target name (the .o file)
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@echo "Compiling $< (found via VPATH for $@) ..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule to create build directories if they don't exist
$(BUILDDIR):
	@echo "Creating directory $(BUILDDIR)..."
	$(MKDIR_P) $(BUILDDIR)

$(OBJDIR): | $(BUILDDIR) # Ensure BUILDDIR exists before OBJDIR
	@echo "Creating directory $(OBJDIR)..."
	$(MKDIR_P) $(OBJDIR)

# Rule to copy resource files (assets, data) to the build directory
copy_resources: | $(BUILDDIR)
	@echo "Copying resources to $(BUILDDIR)..."
	$(CP) assets $(BUILDDIR)/assets || echo "Warning: Could not copy assets."
	$(CP) data $(BUILDDIR)/data || echo "Warning: Could not copy data."

# Rule to copy SFML DLLs to the build directory (for Windows dynamic linking)
DLLS = $(wildcard $(SFML_DLL_PATH)/*.dll)
copy_dlls: | $(BUILDDIR)
ifeq ($(OS),Windows_NT)
	$(if $(DLLS), \
		@echo "Copying SFML DLLs to $(BUILDDIR)..."; \
		$(CP) $(DLLS) $(BUILDDIR)/, \
		@echo "SFML DLLs not found in $(SFML_DLL_PATH)." \
	)
else
	@echo "Skipping DLL copy on non-Windows system."
endif

# Clean rule: remove build directory
clean:
	@echo "Cleaning build files..."
	$(RM) $(BUILDDIR)

# Phony targets: targets that are not files
.PHONY: all clean copy_resources copy_dlls

# Suppress implicit rules to avoid unexpected behavior
.SUFFIXES: