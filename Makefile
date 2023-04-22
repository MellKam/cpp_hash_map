BUILD_DIR = build
CC = g++

build_dir:
	if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi

run: main.cpp build_dir
	$(CC) --std=c++17 -o ./$(BUILD_DIR)/target ./main.cpp && ./$(BUILD_DIR)/target

build-dev: main.cpp build_dir
	$(CC) --std=c++17 -g -Wall -Werror -o ./$(BUILD_DIR)/target ./main.cpp