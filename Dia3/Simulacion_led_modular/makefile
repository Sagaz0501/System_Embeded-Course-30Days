# ===== Config =====
CC        = gcc
CFLAGS    = -Wall -Wextra -O2 -std=c11 -D_GNU_SOURCE -Iinclude
SRC_DIR   = src
BUILD_DIR = build
BIN_DIR   = bin

# ===== Fuentes =====
COMMON_SRCS  = $(SRC_DIR)/gpio_sim.c $(SRC_DIR)/debounce.c $(SRC_DIR)/timeutil.c $(SRC_DIR)/tty.c
SWITCH_SRC   = $(SRC_DIR)/main_switch.c
TOGGLE_SRC   = $(SRC_DIR)/main_toggle.c

COMMON_OBJS  = $(COMMON_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
SWITCH_OBJ   = $(BUILD_DIR)/main_switch.o
TOGGLE_OBJ   = $(BUILD_DIR)/main_toggle.o

BIN_SWITCH   = $(BIN_DIR)/boton_switch
BIN_TOGGLE   = $(BIN_DIR)/boton_toggle

# ===== Targets por defecto =====
all: dirs $(BIN_SWITCH) $(BIN_TOGGLE)

dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# ===== Link =====
$(BIN_SWITCH): $(COMMON_OBJS) $(SWITCH_OBJ) | dirs
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_TOGGLE): $(COMMON_OBJS) $(TOGGLE_OBJ) | dirs
	$(CC) $(CFLAGS) $^ -o $@

# ===== Compilar .o =====
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# ===== Atajos para correr =====
run-switch: $(BIN_SWITCH)
	./$(BIN_SWITCH)

run-toggle: $(BIN_TOGGLE)
	./$(BIN_TOGGLE)

# ===== Limpiar =====
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean dirs run-switch run-toggle
