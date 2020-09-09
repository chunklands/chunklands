mkdir boost
cd boost
curl -L https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz | tar xz --strip-components=1
./bootstrap.sh
# ./b2 link=static variant=debug cxxflags="-fPIC" linkflags="-fPIC"