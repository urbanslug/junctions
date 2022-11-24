BUILD_DIR=build

all:
	@mkdir -p $(BUILD_DIR)
	g++ src/main.cpp -o build/junctions

tests:
	@mkdir -p $(BUILD_DIR)
	g++ src/tests.cpp -o build/test

debug:
	@mkdir -p $(BUILD_DIR)
	g++ -g -Wall -pipe -O2 -std=c++11 src/tests.cpp -lm -o build/test_debug

clean:
	rm -rf $(BUILD_DIR)
