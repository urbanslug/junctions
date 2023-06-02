BUILD_DIR=bin

all:
	@mkdir -p $(BUILD_DIR)
	g++ -c src/core.cpp -o bin/core.o
	g++ -c src/utils.cpp -o bin/utils.o
	g++ -Wall -pipe -O3 -std=c++11 bin/core.o bin/utils.o src/main.cpp -o $(BUILD_DIR)/junctions

# run tests
test:
	@mkdir -p $(BUILD_DIR)
	g++ -g -c src/core.cpp -o bin/core.o
	g++ -g -c src/utils.cpp -o bin/utils.o
	g++ -g -Wall -pipe -std=c++11 bin/core.o bin/utils.o src/tests.cpp -o $(BUILD_DIR)/test

# compile with debugging info
debug:
	@mkdir -p $(BUILD_DIR)
	g++ -g -c src/core.cpp -o bin/core.o
	g++ -g -c src/utils.cpp -o bin/utils.o
	g++ -g -Wall -Wextra -DDEBUG -pipe -std=c++11 bin/core.o bin/utils.o src/main.cpp -lm -o $(BUILD_DIR)/junctions

clean:
	rm -rf $(BUILD_DIR)
