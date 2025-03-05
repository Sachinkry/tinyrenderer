# TinyRenderer

A minimal software rasterizer in C++

## Features

- Supports flat, Gouraud, and Phong shading
- Loads `.obj` models and `.tga` textures
- Outputs rendered images

## Sample Outputs

Here are some sample outputs demonstrating different shading techniques:

<table>
  <tr>
    <td><img src="https://pplx-res.cloudinary.com/image/upload/w_200,c_fit/v1741199849/user_uploads/bzhZyzqHunpRHBV/Screenshot-2025-03-06-at-12.07.12-AM.jpg" width="200"></td>
    <td><img src="https://pplx-res.cloudinary.com/image/upload/w_200,c_fit/v1741200673/user_uploads/olQkorxSooIWhLQ/Screenshot-2025-03-06-at-12.20.36-AM.jpg" width="200"></td>
    <td><img src="https://pplx-res.cloudinary.com/image/upload/w_200,c_fit/v1741200691/user_uploads/fJDmoQcbTYCGsBr/Screenshot-2025-03-06-at-12.20.36-AM.jpg" width="200"></td>
  </tr>
  <tr>
    <td><img src="https://pplx-res.cloudinary.com/image/upload/w_200,c_fit/v1741200691/user_uploads/fJDmoQcbTYCGsBr/Screenshot-2025-03-06-at-12.20.36-AM.jpg" width="200"></td>
    <td><img src="https://pplx-res.cloudinary.com/image/upload/w_200,c_fit/v1741200691/user_uploads/fJDmoQcbTYCGsBr/Screenshot-2025-03-06-at-12.20.36-AM.jpg" width="200"></td>
    <td><img src="https://pplx-res.cloudinary.com/image/upload/w_200,c_fit/v1741200691/user_uploads/fJDmoQcbTYCGsBr/Screenshot-2025-03-06-at-12.20.36-AM.jpg" width="200"></td>
  </tr>
  <tr>
    <td>Flat Shading</td>
    <td>Gouraud Shading</td>
    <td>Phong Shading</td>
  </tr>
</table>
## Installation & Usage

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
