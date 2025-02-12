CC = gcc
CF = -g -Iinclude -m64 $(VARS)
LF = -lkernel32 -luser32 -lonecore

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

VARS=-DNTDDI_VERSION=NTDDI_WIN10_RS5 -D_WIN32_WINNT=_WIN32_WINNT_WIN10

TARGET = $(BIN_DIR)\pterm.exe

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJECT_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Allowing parallel compiling
MAKEFLAGS+=-j8

all: $(BIN_DIR) $(OBJ_DIR) $(TARGET)

run: all
	$(TARGET)

$(BIN_DIR):
	mkdir $@

$(OBJ_DIR):
	mkdir $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo [*] Compiling  $<
	@$(CC) $(CF) -c -o $@ $<

$(TARGET): $(OBJECT_FILES) | $(BIN_DIR)
	@echo [*] Linking all togither
	@$(CC) -o $@ $^ $(LF)

clean:
	del /q /f /s $(OBJ_DIR)\* $(TARGET)

.PHONY: all clean
