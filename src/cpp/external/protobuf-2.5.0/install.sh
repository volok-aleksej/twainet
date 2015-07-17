#!/bin/bash
#compile protobuf
mkdir -p ../../../../output/lib/Debug
mkdir -p ../../../../output/lib/Release
svn checkout https://github.com/google/protobuf/tags/v2.5.0 v2.5.0
cd v2.5.0
./autogen.sh
./configure
./autogen.sh

#compile Release library
C=/usr/bin/gcc ./configure -disable-shared CXXFLAGS="-fPIC"
make
rm ../../../../../output/lib/Release/libprotobuf.a
cp src/.libs/libprotobuf.a ../../../../../output/lib/Release

make clean

#compile Debug library
C=/usr/bin/gcc ./configure -disable-shared CXXFLAGS="-g3 -ggdb3 -fPIC"
make
cp src/.libs/libprotobuf.a ../../../../../output/lib/Debug
cd ..
rm -rf v2.5.0

#checkout src
rm -rf src
svn checkout https://github.com/google/protobuf/tags/v2.5.0/src src
