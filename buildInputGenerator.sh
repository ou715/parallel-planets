NINJA_PATH="$1"

/usr/bin/cmake -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_MAKE_PROGRAM="$NINJA_PATH"  \
  -G Ninja \
  -S . \
  -B cmake-build-release \
&& \
/usr/bin/cmake \
  --build cmake-build-release \
  --target input-generator -j 4
