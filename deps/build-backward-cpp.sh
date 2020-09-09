cd backward-cpp
mkdir build
cd build

CMAKE_BUILD_TYPE=${1:-Debug}

echo "using CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
cmake -DBACKWARD_HAS_BFD=1 -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE --build .. && make
