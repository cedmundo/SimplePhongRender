# SimplePhongRender

A very simplistic GLTF loader and render using phong shader.

## Building

First install all dependencies needed to build GLFW3, KTX and cGLTF from source, then fetch code with submodules:

```sh
git clone --recurse-submodules https://github.com/cedmundo/SimplePhongRender.git
```

Build with CMake:
```sh
mkdir build
cmake -B build -S .
```

Done.
