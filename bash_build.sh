set -e

rm -rf build_sylar

cmake -G "Unix Makefiles" -S . -B build_sylar -DCMAKE_BUILD_TYPE=Debug
cmake --build build_sylar -j 24
# cmake --install build_sylar