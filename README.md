# Chunklands

> This page is designed for developers.

If you want to read more of the project, goals, etc. please have a look at [chunklands.de](https://chunklands.de).

## Notice: Breaking change

Former Chunklands architecture (single-threaded) had performance issues.
POC of `executor-architecture` was merged into master to see daily process on master.

Still the set of features is not fully implemented, but we decided to merge it now.

It will take some time to support all old features.
Please also consider the documentation to be out-dated.

## Technologies

![C++](doc/cpp.svg)
![NodeJS](doc/nodejs.svg)
![N-API](doc/napi.svg)
![boost](doc/boost.svg)
![OpenGL](doc/opengl.svg)
![clang](doc/clang.svg)


## Installation

Requirements:

- node.js 14+
- clang-10+
- cmake 3.0+ (for glfw)

**Linux**

```bash
make setup
```

**MacOS**

```bash
make setup
```

MacOS ships apple clang. It's likely to be incompatible, so another vendor-free clang version is needed.

```bash
brew install llvm

# add this to your .env file
cat .env
# CLANG_BIN=/usr/local/opt/llvm/bin/clang
# CLANG_TIDY_BIN=/usr/local/opt/llvm/bin/clang-tidy
```

**Windows:**

*not supported, sorry*

## Development

### Build / Run
#### Variant 1 - dev build, watch and run

1. watch files and build dev
```bash
npm run dev
```

2. start game
```bash
npm start
```

#### Variant 2 - dev build and run

build dev and run
```bash
npm run build:dev && npm start
```

#### Variant 3 - prod build and run

build prod and run
```bash
npm run build:prod && npm start
```

or simply:

```bash
npm run play
```

### Other commands

#### Clean

remove /build directory
```bash
make clean
```

## Links

- [GitHub Issues](https://github.com/chunklands/chunklands/issues)
- [GitHub Project](https://github.com/chunklands/chunklands/projects/1)


## Progress

Current status:

![Current Result](./status.png)
