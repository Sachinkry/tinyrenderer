# TinyRenderer

A minimal software rasterizer in C++

## Features

- Supports flat, Gouraud, and Phong shading
- Loads `.obj` models and `.tga` textures
- Outputs rendered images

## Sample Outputs

Wireframes:

<div style="display: flex; gap: 10px;">
  <img src="samples/image-1.png" width="300">
  <img src="samples/image-2.png" width="300">
  <img src="samples/image.png" width="400">
</div>

Here are some sample outputs demonstrating different shading techniques:

Flat Shading ⬇️
<img src="samples/image-3.png" width="200">
Gouraud Shading⬇️
<img src="samples/image-4.png" width="200">
Phong Shading⬇️
<img src="samples/image-5.png" width="200">

Diablo3 Character:
<img src="samples/diable3_pose_nm.png" width="400">

1.  Clone the repository:

    ```
    git clone https://github.com/Sachinkry/tinyrenderer.git
    cd tinyrenderer
    ```

2.  **Build the project using CMake:**

    ```
    mkdir build
    cd build
    cmake ..
    make
    ```

3.  **Run the executable:**

    ```
    ./main
    ```

## Dependencies

- A C++ compiler with C++17 support (e.g., g++)
- CMake (version 3.10 or higher)

## Notes

- The rendered images are saved in the `assets/outputs` directory.
- The `.vscode` directory and the `main` executable are ignored by Git (see `.gitignore`).

```
 g++ -std=c++17 -ggdb -g -pg -O0 -Iinclude -o main src/main.cpp src/tgaimage.cpp src/model.cpp src/shaders.cpp
```

```
./main
```
