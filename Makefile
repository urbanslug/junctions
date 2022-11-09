FOLDER_BUILD=build

ifeq ($(shell uname),Darwin)
    EXT := dylib
else
    EXT := so
endif

all: target/debug/libjunctions.$(EXT)
	g++ ./cpp/src/main.cpp  -L ./rs/target/debug/ -ljunctions -o build/junctions
	LD_LIBRARY_PATH=./rs/target/debug/ ./build/junctions

setup:
	@mkdir -p $(FOLDER_BUILD)

target/debug/libjunctions.$(EXT): rs/src/lib.rs rs/Cargo.toml
	cd rs; \
	cargo build

clean:
	rm -rf $(FOLDER_BUILD)
	rm -rf rs/target
	rm -rf run