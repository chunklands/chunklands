CMAKE_BUILD_TYPE=${1:-Debug}

./build-backward-cpp.sh $CMAKE_BUILD_TYPE
./build-boost.sh $CMAKE_BUILD_TYPE
./build-easy_profiler.sh $CMAKE_BUILD_TYPE
./build-easyloggingpp.sh $CMAKE_BUILD_TYPE
./build-glfw.sh $CMAKE_BUILD_TYPE
./build-glm.sh $CMAKE_BUILD_TYPE
./build-node.sh $CMAKE_BUILD_TYPE
./build-stb.sh $CMAKE_BUILD_TYPE