prog: build/Makefile
	make -C build chunklands.node

build:
	mkdir -p build

build/Makefile: .clang.js | build
	node .clang.js > build/Makefile