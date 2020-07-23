cd deps/boost
./bootstrap.sh
./b2 link=static variant=debug cxxflags="-fPIC" linkflags="-fPIC"