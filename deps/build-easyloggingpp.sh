cd easyloggingpp
mkdir build
cd build

CMAKE_BUILD_TYPE=${1:-Debug}

echo "using CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
cmake -Dbuild_static_lib=true -DCMAKE_CXX_FLAGS="-DELPP_THREAD_SAFE" -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE --build .. && make