# geometry-central-tutorials
Tutorials for the geometry-central geometry processing library.

Each tutorial lives in a subdirectory of 

### Building and running

On unix-like machines, use:
```
git clone --recurse-submodules https://github.com/nmwsharp/geometry-central-tutorials
cd geometry-central-tutorials
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4

./bin/load-mesh ../assets/spot.obj
```

The codebase also builds on Visual Studio 2017 & 2019, by using CMake to generate a Visual Studio solution file.
