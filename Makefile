BUILD_DIR=bin


all:
	@mkdir -p $(BUILD_DIR)
	g++ -O3 -c src/core/core.cpp -o bin/core.o
	g++ -O3 -c src/core/suffix_tree.cpp -o bin/suffix_tree.o
	g++ -O3 -c src/eds/eds.cpp -o bin/eds.o
	g++ -O3 -c src/graph/graph.cpp -o bin/graph.o
	g++ -O3 -c src/intersect/improved.cpp -o bin/improved.o
	g++ -O3 -c src/intersect/naive.cpp -o bin/naive.o
	g++ -O3 -c src/cli/cli.cpp -o bin/cli.o
	g++ -O3 -c src/cli/argvparser.cpp -o bin/argvparser.o
	g++ -O3 -c src/cli/parseCmdArgs.cpp -o bin/parseCmdArgs.o
	g++ -Wall -pipe -O3 -std=c++11 bin/*.o src/main.cpp -lm -o $(BUILD_DIR)/junctions

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
	g++ -g -Wall -Wextra -DDEBUG -pipe -std=c++11 bin/*.o src/main.cpp -lm -o $(BUILD_DIR)/junctions

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
	g++ -g -Wall -Wextra -DDEBUG -pipe -std=c++11 bin/*.o tests/tests.cpp -lm -o $(BUILD_DIR)/junctions


clean:
	rm -rf $(BUILD_DIR)
