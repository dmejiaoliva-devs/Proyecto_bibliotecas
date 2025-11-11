CC=gcc
CFLAGS=-Wall -Wextra -O2 -fPIC
DLFLAGS=-ldl

BUILD_DIR=build
SRC_DIR=src

LIB1=$(BUILD_DIR)/libcalc.so
LIB2=$(BUILD_DIR)/libcalc2.so
BIN=$(BUILD_DIR)/demo

all: $(LIB1) $(LIB2) $(BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Biblioteca 1 (normal)
$(LIB1): $(SRC_DIR)/calc.c $(SRC_DIR)/calc.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $(SRC_DIR)/calc.c

# Biblioteca 2 (modificada para la demo)
$(LIB2): $(SRC_DIR)/calc2.c $(SRC_DIR)/calc.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $(SRC_DIR)/calc2.c

# Ejecutable que usa dlopen/dlsym/dlclose
$(BIN): $(SRC_DIR)/demo.c | $(BUILD_DIR)
	$(CC) -Wall -Wextra -O2 -o $@ $(SRC_DIR)/demo.c $(DLFLAGS) -Wl,-rpath,'$$ORIGIN'

run: all
	@echo "---- Ejecutando con libcalc.so ----"
	@$(BIN) ./build/libcalc.so

run2: all
	@echo "---- Ejecutando con libcalc2.so ----"
	@$(BIN) ./build/libcalc2.so

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run run2 clean
