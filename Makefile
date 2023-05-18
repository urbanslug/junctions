BUILD_DIR=bin

all:
	@mkdir -p $(BUILD_DIR)
	g++ -c src/core.cpp -o bin/core.o
	g++ -c src/utils.cpp -o bin/utils.o
	g++ -Wall -pipe -O2 -std=c++11 bin/core.o bin/utils.o src/main.cpp -o $(BUILD_DIR)/junctions
	#g++ -g -Wall -pipe bin/core.o bin/utils.o src/main.cpp -o $(BUILD_DIR)/junctions

perf:
	@mkdir -p $(BUILD_DIR)
	g++ -c src/core.cpp -o bin/core.o
	g++ -c src/utils.cpp -o bin/utils.o
	g++ -g -Wall -pipe -std=c++11 bin/core.o bin/utils.o src/main.cpp -o $(BUILD_DIR)/junctions

test:
	@mkdir -p $(BUILD_DIR)
	g++ -c src/core.cpp -o bin/core.o
	g++ -c src/utils.cpp -o bin/utils.o
	g++ -g -Wall -pipe bin/core.o bin/utils.o src/tests.cpp -o $(BUILD_DIR)/test

debug:
	@mkdir -p $(BUILD_DIR)
	g++ -c src/core.cpp -o bin/core.o
	g++ -c src/utils.cpp -o bin/utils.o
	g++ -g -Wall -pipe -std=c++11 bin/core.o bin/utils.o src/tests.cpp -lm -o $(BUILD_DIR)/test_debug

clean:
	rm -rf $(BUILD_DIR)
