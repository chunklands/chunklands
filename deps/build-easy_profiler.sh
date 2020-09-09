cd easy_profiler
mkdir build
cd build

CMAKE_BUILD_TYPE=${1:-Debug}

echo "using CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE --build .. && make
