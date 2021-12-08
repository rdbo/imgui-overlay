CXX=g++
BIN_NAME=overlay
OUT_DIR=./build

overlay: setup
	$(CXX) -o $(OUT_DIR)/$(BIN_NAME) -g overlay/*.cpp imgui/*.cpp -I imgui -Wall -Wextra -Wpedantic -lX11 -lGL -lglfw

setup:
	if [ ! -d $(OUT_DIR) ]; then mkdir $(OUT_DIR); fi

clean:
	if [ -d $(OUT_DIR) ]; then rm -rf $(OUT_DIR); fi
