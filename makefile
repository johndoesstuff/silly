CC = gcc
CFLAGS = -Wall -g
TARGET = miniAsm
SRC_DIR = src
BUILD_DIR = build
SRC = $(SRC_DIR)/miniAsm.c
OBJ = $(BUILD_DIR)/miniAsm.o

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(OBJ)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
