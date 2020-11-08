prog: build/Makefile
	make -f build/Makefile -j8

# test: build/Makefile
#   make -f build/Makefile -j8 build/chunklands_test.node

build:
	mkdir -p build

build/Makefile: .clang.js .env | build
	node .clang.js > build/Makefile

.env:
	touch .env

clean:
	rm -rf build

all: clean prog

setup: setup-deps npm-ci

setup-deps:
	@cd deps && \
		./install.sh && \
		./build.sh

npm-ci:
	npm ci

.PHONY: clean setup setup-git npm-ci test