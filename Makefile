.PHONY: all
all: format build run

.PHONY: format
format:
	clang-format src/* include/* -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	make

.PHONY: clean
clean:
	rm -rf build

.PHONY: run
run:
	./build/monitor

.PHONY: sudorun
sudorun:
	sudo ./build/monitor

.PHONY: gdb
gdb:
	sudo gdb ./build/monitor
