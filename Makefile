CC = gcc
CF = -g -Iinclude -m64 -O3 $(VARS)
LF = -mwindows -ldwmapi -lgdi32 -lkernel32 -luser32 -lonecore

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

TEST_DIR = test

VARS=-DNTDDI_VERSION=NTDDI_WIN10_RS5 -D_WIN32_WINNT=_WIN32_WINNT_WIN10

TARGET = $(BIN_DIR)\pterm.exe
TEST_TARGET = $(BIN_DIR)\test.exe

C_SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
CXX_SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

C_OBJECT_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
CXX_OBJECT_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
OBJECT_FILES = $(C_SRC_FILES) $(CXX_SRC_FILES)


# test
TEST_SRC_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECT_FILES := $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_SRC_FILES))
TEST_OBJECT_FILES += $(filter-out $(OBJ_DIR)/main.o, $(OBJECT_FILES))

# Allowing parallel compiling
MAKEFLAGS+=-j8

all: $(BIN_DIR) $(OBJ_DIR) $(TARGET)
test: $(TEST_TARGET)

run: all
	$(TARGET)

run_test: test
	$(TEST_TARGET)

$(BIN_DIR):
	mkdir $@

$(OBJ_DIR):
	mkdir $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo [*] Compiling  $<
	@$(CC) $(CF) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo [*] Compiling  $<
	@$(CC) $(CF) -c -o $@ $<

$(TARGET): $(OBJECT_FILES) | $(BIN_DIR)
	@echo [*] Linking all togither
	@$(CC) $(CF) -o $@ $^ $(LF)


$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	@echo [*] Compiling test  $<
	@$(CC) $(CF) -c -o $@ $<

$(TEST_TARGET): $(TEST_OBJECT_FILES) | $(BIN_DIR)
	@echo [*] Linking all togither
	@$(CC) -o $@ $^ $(LF)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET) .zig-cache zig-out

.PHONY: all test clean
