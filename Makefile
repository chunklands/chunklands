prog: build/Makefile
	make -C build -j8

build:
	mkdir -p build

build/Makefile: .clang.js .env | build
	node .clang.js > build/Makefile

.env:
	touch .env

clean:
	rm -rf build

all: clean prog

.PHONY: clean