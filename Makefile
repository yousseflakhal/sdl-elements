# Compiler & flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -MMD -MP
INCLUDE := include
LIB     := lib
SRC     := src
BUILD   := build
OUTPUT  := output

# Executable
TARGET  := main
ifeq ($(OS),Windows_NT)
  TARGET := $(OUTPUT)/$(TARGET).exe
  MKDIR  := mkdir
  RM     := del /q /f
  RMDIR  := rmdir /s /q
else
  TARGET := $(OUTPUT)/$(TARGET)
  MKDIR  := mkdir -p
  RM     := rm -f
  RMDIR  := rm -rf
endif

# SDL include/lib + link
CXXFLAGS += -I$(INCLUDE)
LDFLAGS  += -L$(LIB) -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Sources/objects/deps
SRCS := $(wildcard $(SRC)/*.cpp)
OBJS := $(patsubst $(SRC)/%.cpp,$(BUILD)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Default
all: $(OUTPUT) $(BUILD) $(TARGET)
	@echo Build complete

$(OUTPUT) $(BUILD):
	@$(MKDIR) $@ 2>NUL || true

# Link (only runs if any object changed)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Compile (only recompiles changed .cpp)
$(BUILD)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Auto dependencies
-include $(DEPS)

.PHONY: clean run

clean:
	-$(RMDIR) $(BUILD) 2>NUL || true
	-$(RMDIR) $(OUTPUT) 2>NUL || true
	@echo Cleanup complete

run: all
	"./$(TARGET)"
