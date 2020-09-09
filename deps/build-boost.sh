cd boost

CMAKE_BUILD_TYPE=${1:-Debug}

if [ "$CMAKE_BUILD_TYPE" = "Release" ]; then
    VARIANT=release
else
    VARIANT=debug
fi

echo "using variant=$VARIANT"
./b2 link=static variant=$VARIANT cxxflags="-fPIC" linkflags="-fPIC"