prog: build/Makefile
	make -C build -j8

build:
	mkdir -p build

build/Makefile: .clang.js | build
	node .clang.js > build/Makefile