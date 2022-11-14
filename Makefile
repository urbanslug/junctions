BUILD_DIR=build

all:
	@mkdir -p $(BUILD_DIR)
	g++ src/main.cpp -o build/junctions

clean:
	rm -rf $(BUILD_DIR)
