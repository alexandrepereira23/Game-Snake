CC ?= gcc
CPPFLAGS := -Isrc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
LDLIBS := -lglut -lGL

BUILD_DIR := build
GAME := $(BUILD_DIR)/snake
TEST := $(BUILD_DIR)/test_game

.PHONY: all run test clean

all: $(GAME)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(GAME): src/main.c src/game.c src/game.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) src/main.c src/game.c -o $@ $(LDLIBS)

$(TEST): tests/test_game.c src/game.c src/game.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_game.c src/game.c -o $@

run: $(GAME)
	./$(GAME)

test: $(TEST)
	./$(TEST)

clean:
	rm -rf $(BUILD_DIR)
