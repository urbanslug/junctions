BUILD_DIR=bin
CMAKE_BUILD_DIR=build
CPP_STANDARD=c++14
COMPILER_FLAGS=-Wall -Wextra -Wpedantic -pipe -O3 -std=${CPP_STANDARD}

all:
	@mkdir -p $(BUILD_DIR)
	g++ ${COMPILER_FLAGS} -c src/core/core.cpp -o bin/core.o
	g++ ${COMPILER_FLAGS} -c src/core/suffix_tree.cpp -o bin/suffix_tree.o
	g++ ${COMPILER_FLAGS} -c src/eds/eds.cpp -o bin/eds.o
	g++ ${COMPILER_FLAGS} -c src/graph/graph.cpp -o bin/graph.o
	g++ ${COMPILER_FLAGS} -c src/intersect/improved.cpp -o bin/improved.o
	g++ ${COMPILER_FLAGS} -c src/intersect/naive.cpp -o bin/naive.o
	g++ ${COMPILER_FLAGS} -c src/cli/cli.cpp -o bin/cli.o
	g++ ${COMPILER_FLAGS} -c src/cli/argvparser.cpp -o bin/argvparser.o
	g++ ${COMPILER_FLAGS}   -c src/cli/parseCmdArgs.cpp -o bin/parseCmdArgs.o
	g++ ${COMPILER_FLAGS} bin/*.o src/main.cpp -lm -o $(BUILD_DIR)/junctions

# compile with debugging info
debug:
	@mkdir -p $(BUILD_DIR)
	g++ -g -c src/core/core.cpp -o bin/core.o
	g++ -g -c src/core/suffix_tree.cpp -o bin/suffix_tree.o
	g++ -g -c src/eds/eds.cpp -o bin/eds.o
	g++ -g -c src/graph/graph.cpp -o bin/graph.o
	g++ -g -c src/intersect/improved.cpp -o bin/improved.o
	g++ -g -c src/intersect/naive.cpp -o bin/naive.o
	g++ -g -c src/cli/cli.cpp -o bin/cli.o
	g++ -g -c src/cli/argvparser.cpp -o bin/argvparser.o
	g++ -g -c src/cli/parseCmdArgs.cpp -o bin/parseCmdArgs.o
	g++ -g -Wall -Wextra -DDEBUG -pipe -std=c++14 bin/*.o src/main.cpp -lm -o $(BUILD_DIR)/junctions

# run tests
test:
	@mkdir -p $(BUILD_DIR)
	g++ -g -c src/core/core.cpp -o bin/core.o
	g++ -g -c src/core/suffix_tree.cpp -o bin/suffix_tree.o
	g++ -g -c src/eds/eds.cpp -o bin/eds.o
	g++ -g -c src/graph/graph.cpp -o bin/graph.o
	g++ -g -c src/intersect/improved.cpp -o bin/improved.o
	g++ -g -c src/intersect/naive.cpp -o bin/naive.o
	g++ -g -c src/cli/cli.cpp -o bin/cli.o
	g++ -g -c src/cli/argvparser.cpp -o bin/argvparser.o
	g++ -g -c src/cli/parseCmdArgs.cpp -o bin/parseCmdArgs.o
	g++ -g -Wall -Wextra -DDEBUG -pipe -std=c++14 bin/*.o tests/tests.cpp -lm -o $(BUILD_DIR)/junctions


clean:
	rm -rf $(BUILD_DIR) ${CMAKE_BUILD_DIR}
