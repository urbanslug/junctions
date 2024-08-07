BUILD_DIR=bin
CMAKE_BUILD_DIR=build
CPP_STANDARD=c++17
COMMON_COMPILER_FLAGS=-pipe -Wall -Wextra -Wpedantic -march=native
RELEASE_COMPILER_FLAGS=${COMMON_COMPILER_FLAGS} -O3 -std=${CPP_STANDARD}
DEBUG_COMPILER_FLAGS=-g ${COMMON_COMPILER_FLAGS} -O0 -DDEBUG -std=${CPP_STANDARD}
STATIC_BUILD_FLAGS=-static -static-libgcc -static-libstdc++
AVX_COMPILER_FLAGS=-mavx2
AVX_MACRO=-DAVX2_SUPPORTED


# plain `make` without a target runs the first target it encounters in the makefile, which in this case is `all`
all: dynamic

# with dynamic linking
dynamic: build
	g++ ${RELEASE_COMPILER_FLAGS} bin/*.o src/main.cpp -lm -o $(BUILD_DIR)/junctions

# with static linking
static: build
	g++ ${RELEASE_COMPILER_FLAGS} ${STATIC_BUILD_FLAGS} bin/*.o src/main.cpp -lm -o $(BUILD_DIR)/junctions

# compile with debugging info
debug: debug_build
	g++ ${DEBUG_COMPILER_FLAGS} bin/*.o src/main.cpp -lm -o $(BUILD_DIR)/junctions

# run tests
test: debug_build
	g++ ${DEBUG_COMPILER_FLAGS} bin/*.o tests/tests.cpp -lm -o $(BUILD_DIR)/junctions

clean:
	rm -rf $(BUILD_DIR) ${CMAKE_BUILD_DIR}

build:
	@mkdir -p $(BUILD_DIR) ${CMAKE_BUILD_DIR}
	g++ ${RELEASE_COMPILER_FLAGS} -c src/core/core.cpp -o bin/core.o
	g++ ${RELEASE_COMPILER_FLAGS} -c src/core/utils.cpp -o bin/utils.o
	g++ ${RELEASE_COMPILER_FLAGS} -c src/core/suffix_tree.cpp -o bin/suffix_tree.o
	g++ ${RELEASE_COMPILER_FLAGS} -c src/eds/eds.cpp -o bin/eds.o
	g++ ${RELEASE_COMPILER_FLAGS} -c src/eds/common.cpp -o bin/common.o
ifeq ($(WITH_MSA),true)
	g++ ${AVX_MACRO} ${RELEASE_COMPILER_FLAGS} -c src/cli/cli.cpp -o bin/cli.o
	g++ ${RELEASE_COMPILER_FLAGS} ${AVX_MACRO} -c src/cli/app.cpp -o bin/app.o
	g++ ${RELEASE_COMPILER_FLAGS} ${AVX_COMPILER_FLAGS} -c src/eds/msa.cpp -o bin/msa.o
else
	g++ ${RELEASE_COMPILER_FLAGS} -c src/cli/cli.cpp -o bin/cli.o
	g++ ${RELEASE_COMPILER_FLAGS} -c src/cli/app.cpp -o bin/app.o
endif
	g++ ${RELEASE_COMPILER_FLAGS} -c src/graph/graph.cpp -o bin/graph.o
	g++ ${RELEASE_COMPILER_FLAGS} -c src/graph/operations.cpp -o bin/operations.o
	g++ ${RELEASE_COMPILER_FLAGS} -c src/intersect/improved.cpp -o bin/improved.o
	g++ ${RELEASE_COMPILER_FLAGS} -c src/intersect/naive.cpp -o bin/naive.o



debug_build:
	@mkdir -p $(BUILD_DIR) ${CMAKE_BUILD_DIR}
	g++ ${DEBUG_COMPILER_FLAGS} -c src/core/core.cpp -o bin/core.o
	g++ ${DEBUG_COMPILER_FLAGS} -c src/core/utils.cpp -o bin/utils.o
	g++ ${DEBUG_COMPILER_FLAGS} -c src/core/suffix_tree.cpp -o bin/suffix_tree.o
	g++ ${DEBUG_COMPILER_FLAGS} -c src/eds/eds.cpp -o bin/eds.o
	g++ ${DEBUG_COMPILER_FLAGS} -c src/eds/common.cpp -o bin/common.o
ifeq ($(WITH_MSA),true)
	g++ ${AVX_MACRO} ${DEBUG_COMPILER_FLAGS} -c src/cli/cli.cpp -o bin/cli.o
	g++ ${DEBUG_COMPILER_FLAGS} ${AVX_MACRO} -c src/cli/app.cpp -o bin/app.o
	g++ ${DEBUG_COMPILER_FLAGS} ${AVX_COMPILER_FLAGS} -c src/eds/msa.cpp -o bin/msa.o
else
	g++ ${DEBUG_COMPILER_FLAGS} -c src/cli/cli.cpp -o bin/cli.o
	g++ ${DEBUG_COMPILER_FLAGS} -c src/cli/app.cpp -o bin/app.o
endif
	g++ ${DEBUG_COMPILER_FLAGS} -c src/graph/graph.cpp -o bin/graph.o
	g++ ${DEBUG_COMPILER_FLAGS} -c src/graph/operations.cpp -o bin/operations.o
	g++ ${DEBUG_COMPILER_FLAGS} -c src/intersect/improved.cpp -o bin/improved.o
	g++ ${DEBUG_COMPILER_FLAGS} -c src/intersect/naive.cpp -o bin/naive.o
