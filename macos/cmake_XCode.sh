cd ../
mkdir -p build
cd build

ARCH=-DCMAKE_OSX_ARCHITECTURES="${CMAKE_OSX_ARCHITECTURES:$(uname -m)}"
cmake -G "Xcode" ${ARCH} ..
