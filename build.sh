NINJA_PATH="$1"

/usr/bin/cmake -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_MAKE_PROGRAM="$NINJA_PATH"  \
  -G Ninja \
  -S . \
  -B cmake-build-debug \
&& \
/usr/bin/cmake \
  --build cmake-build-debug \
  --target parallel-planets -j 22
