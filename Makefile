BUILD_DIR=bin

RELEASE_OPTIMISATION_LEVEL="-O3"

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

nu:
	@mkdir -p $(BUILD_DIR)
	g++ -g -c src/core/core.cpp -o bin/core.o
	g++ -g -c src/core/suffix_tree.cpp -o bin/suffix_tree.o
	g++ -g -c src/eds/eds.cpp -o bin/eds.o
	g++ -g  -c src/graph/graph.cpp -o bin/graph.o
	g++ -g -c src/intersect/improved.cpp -o bin/improved.o
	g++ -g -c src/intersect/naive.cpp -o bin/naive.o
	g++ -g  -DDEBUG -pipe -std=c++11 bin/*.o src/new_main.cpp -lm -o $(BUILD_DIR)/junctions

nures:
	@mkdir -p $(BUILD_DIR)
	g++ -O3 -c src/core/core.cpp -o bin/core.o
	g++ -O3 -c src/core/suffix_tree.cpp -o bin/suffix_tree.o
	g++ -O3 -c src/eds/eds.cpp -o bin/eds.o
	g++ -O3 -c src/graph/graph.cpp -o bin/graph.o
	g++ -O3 -c src/intersect/improved.cpp -o bin/improved.o
	g++ -O3 -c src/intersect/naive.cpp -o bin/naive.o
	g++ -Wall -pipe -O3 -std=c++11 bin/*.o src/new_main.cpp -lm -o $(BUILD_DIR)/junctions

clean:
	rm -rf $(BUILD_DIR)
