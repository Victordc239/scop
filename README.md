*This project has been created as part of the 42 curriculum by vdiez-cu.*

# scop 🎨

[![C++](https://img.shields.io/badge/language-C%2B%2B17-555?style=flat&logo=cplusplus)](https://en.wikipedia.org/wiki/C%2B%2B)
[![OpenGL](https://img.shields.io/badge/OpenGL-3.3-blue?style=flat&logo=opengl)](https://www.opengl.org/)
[![GLFW](https://img.shields.io/badge/GLFW-window%20%2B%20input-8A2BE2?style=flat)](https://www.glfw.org/)
[![42 Curriculum](https://img.shields.io/badge/42-Curriculum-orange?style=flat)](https://www.42.fr/)

> A 42 school project: a small 3D model viewer written in C++ using OpenGL, GLFW and GLAD, with OBJ loading, transformations, perspective projection and texture support.

---

## 📌 Description

`scop` is a C++ graphics project developed as part of the 42 curriculum.

The program creates an OpenGL 3.3 Core window and renders a 3D model loaded from an `.obj` file. The project implements the main elements of a basic 3D rendering pipeline without relying on a high-level 3D engine.

The current implementation includes:

- OpenGL 3.3 Core rendering.
- GLFW window creation and keyboard input.
- GLAD OpenGL function loading.
- OBJ model loading.
- Support for vertices and texture coordinates.
- Triangulation of polygonal faces.
- Positive and negative OBJ indices.
- Automatic model centering using a bounding box.
- Automatic texture coordinates when an OBJ has no UVs.
- PPM P6 texture loading.
- OpenGL texture creation and mipmaps.
- Vertex and fragment shaders written in GLSL.
- Model, view and projection matrices.
- Translation and rotation on the three axes.
- Perspective projection.
- Depth testing.
- Runtime texture blending.
- Window resizing.

---

## 🎯 Subject Goals

The project demonstrates the complete path from an OBJ file to the final image displayed on screen:

```text
OBJ MODEL
    |
    v
+------------------+
|   Model Loader   |
|  vertices / UVs  |
|  face parsing    |
+--------+---------+
         |
         v
+------------------+
|     Buffers      |
|      VAO/VBO     |
+--------+---------+
         |
         v
+------------------+
|     Shaders      |
| Vertex / Fragment|
+--------+---------+
         |
         v
+------------------+
| Matrix Pipeline  |
| Model / View /   |
| Projection       |
+--------+---------+
         |
         v
+------------------+
|     OpenGL       |
|      Draw        |
+--------+---------+
         |
         v
      WINDOW
```

The implementation focuses on understanding the basic concepts behind 3D rendering rather than hiding them behind a graphics engine.

---

## 🎮 Controls

### Translation

| Key | Action |
|-----|--------|
| `A` | Move the model left on X |
| `D` | Move the model right on X |
| `W` | Move the model up on Y |
| `S` | Move the model down on Y |
| `Q` | Move the model forward on Z |
| `E` | Move the model backward on Z |

### Rotation

| Key | Action |
|-----|--------|
| `↑` | Rotate around X |
| `↓` | Rotate around X in the opposite direction |
| `←` | Rotate around Y |
| `→` | Rotate around Y in the opposite direction |
| `Z` | Rotate around Z |
| `X` | Rotate around Z in the opposite direction |

### Other controls

| Key | Action |
|-----|--------|
| `R` | Reset position and rotation |
| `T` | Toggle texture blending |
| `ESC` | Close the application |

The texture transition is smoothly interpolated instead of changing instantly.

---

## 🧮 3D Mathematics

The project contains its own mathematical module in `Math.hpp` and `Math.cpp`.

### Vectors

The implementation provides:

- Vector subtraction.
- Vector normalization.
- Cross product.

These operations are used to construct the camera coordinate system.

### Matrices

The project implements 4×4 matrices and provides:

- Identity matrix.
- Translation matrix.
- Rotation around X.
- Rotation around Y.
- Rotation around Z.
- Matrix multiplication.
- Perspective projection.
- View matrix.

The rendering pipeline uses:

```text
Model → View → Projection → Clip Space
```

and the vertex shader performs:

```glsl
gl_Position = projection * view * model * vec4(aPos, 1.0);
```

### Model matrix

The model matrix combines the object's translation and rotations.

The current implementation builds the rotation as:

```text
Rotation Y × Rotation X
Rotation Z × (Rotation Y × Rotation X)
Translation × Rotation
```

### View matrix

The initial camera position is:

```text
(0, 0, 3)
```

and it looks towards:

```text
(0, 0, 0)
```

The view matrix is constructed using forward, right and up vectors.

### Projection matrix

The application uses perspective projection with:

```text
FOV:  45°
Near: 0.1
Far:  100.0
```

The aspect ratio is recalculated from the framebuffer size.

---

## 🧱 OBJ Model Loading

The `Model` class contains a custom OBJ parser.

The loader currently handles:

```text
v   vertex positions
vt  texture coordinates
f   polygon faces
```

Comments beginning with `#` are ignored.

### Faces

Faces containing three or more vertices are triangulated using a triangle fan.

For example:

```text
(0,1,2)
(0,2,3)
...
```

This allows the resulting geometry to be rendered with:

```cpp
glDrawArrays(GL_TRIANGLES, 0, vertexCount);
```

### OBJ indices

Both positive and negative OBJ indices are supported.

```text
1  = first vertex
2  = second vertex
...
-1 = last vertex
-2 = previous vertex
...
```

### Texture coordinates

If an OBJ contains UV coordinates, they are used.

If it does not, planar coordinates are generated from the model position:

```text
u = x * 0.5 + 0.5
v = y * 0.5 + 0.5
```

---

## 📐 Model Centering

After loading the model, an axis-aligned bounding box is calculated.

The center is:

```text
center = (min + max) / 2
```

All vertices are then translated by the negative center, placing the model around:

```text
(0, 0, 0)
```

This makes transformations and camera positioning predictable.

---

## 🎨 Colors and Textures

Each face receives one of six generated grayscale colors:

```text
0.25
0.35
0.45
0.55
0.65
0.75
```

The vertex shader passes both color and texture coordinates to the fragment shader.

The fragment shader mixes the generated color and sampled texture:

```glsl
FragColor = mix(color, textureColor, textureMix);
```

The `T` key controls `textureMix`.

```text
textureMix = 0 → generated color
textureMix = 1 → texture
```

The transition between the two states is gradual.

---

## 🖥️ Rendering Pipeline

The project uses the OpenGL programmable pipeline.

### Vertex Shader

The vertex attributes are:

```text
location 0 → position
location 1 → color
location 2 → texture coordinates
```

The final vertex position is calculated with:

```glsl
gl_Position = projection * view * model * vec4(aPos, 1.0);
```

### Fragment Shader

The fragment shader samples the texture using:

```glsl
texture(textureSampler, texCoord)
```

and blends it with the generated vertex color.

---

## 🧩 OpenGL Objects

The `Model` class creates and manages the main OpenGL objects.

### VAO

Stores the vertex attribute configuration.

### VBO

Contains the final renderable vertex data.

Each vertex contains:

```text
position → 3 floats
color    → 3 floats
texCoord → 2 floats
```

Total:

```text
8 floats per vertex
```

### Texture

The texture is loaded into an OpenGL `GL_TEXTURE_2D`.

The project uses:

```text
GL_REPEAT
GL_LINEAR_MIPMAP_LINEAR
GL_LINEAR
```

and generates mipmaps with:

```cpp
glGenerateMipmap(GL_TEXTURE_2D);
```

---

## 📦 Models and Resources

The repository contains:

```text
models/
├── 42.obj
├── cube.obj
├── teapot2.obj
└── teapot.obj
```

The default model is:

```text
models/42.obj
```

The default texture is:

```text
textures/texture.ppm
```

The executable accepts custom model and texture paths from the command line.

---

## 🖥️ Usage

### Default

```bash
./scop
```

Loads:

```text
models/42.obj
textures/texture.ppm
```

### Custom model

```bash
./scop models/cube.obj
```

### Custom model and texture

```bash
./scop models/cube.obj textures/texture.ppm
```

The program prints the OpenGL version and selected resources:

```text
OpenGL: ...
Modelo: ...
Textura: ...
```

---

## 🔧 Installation & Compilation

### Requirements

- Linux / Unix-like environment.
- C++ compiler with C++17 support.
- `make`.
- GLFW.
- OpenGL.
- GLAD.

The Makefile links:

```text
-lglfw
-lGL
-ldl
```

### Build

```bash
git clone <repository_url>
cd scop
make
```

Then:

```bash
./scop
```

### Compilation flags

```text
-Wall -Wextra -Werror -std=c++17
```

Include paths:

```text
-Iinclude
-Ilibs/glad/include
```

---

## 🛠️ Makefile Rules

| Rule | Description |
|------|-------------|
| `make` | Compile the project |
| `make all` | Compile the project |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and executable |
| `make re` | Full recompilation |

Example:

```bash
make re
```

---

## 📂 Project Structure

```text
.
├── include
│   ├── Math.hpp
│   ├── Model.hpp
│   └── Shader.hpp
│
├── models
│   ├── 42.obj
│   ├── cube.obj
│   ├── teapot2.obj
│   └── teapot.obj
│
├── shaders
│   ├── fragment.glsl
│   └── vertex.glsl
│
├── src
│   ├── main.cpp
│   ├── Math.cpp
│   ├── Model.cpp
│   └── Shader.cpp
│
├── textures
│   ├── 42.mtl
│   └── texture.ppm
│
├── Makefile
├── README.md
└── actualizar.sh
```

### Source files

| File | Responsibility |
|------|----------------|
| `src/main.cpp` | GLFW initialization, input, render loop and matrix setup |
| `src/Math.cpp` | Vector operations and 4×4 matrix mathematics |
| `src/Model.cpp` | OBJ parsing, triangulation, centering, buffers and texture loading |
| `src/Shader.cpp` | GLSL loading, compilation, linking and uniform management |
| `include/Math.hpp` | Vector and matrix declarations |
| `include/Model.hpp` | Model and vertex declarations |
| `include/Shader.hpp` | Shader declarations |
| `shaders/vertex.glsl` | Vertex transformation and attribute forwarding |
| `shaders/fragment.glsl` | Texture/color blending |
| `models/*.obj` | 3D model resources |
| `textures/texture.ppm` | PPM texture resource |
| `Makefile` | Compilation and cleanup |

---

## 🏗️ Architecture

The project is divided into three main components.

### Math

```text
Math
├── Vec3
│   ├── subtraction
│   ├── normalization
│   └── cross product
│
└── Mat4
    ├── identity
    ├── translation
    ├── rotation X
    ├── rotation Y
    ├── rotation Z
    ├── multiplication
    ├── perspective
    └── view
```

### Shader

```text
Shader
├── Read GLSL files
├── Compile vertex shader
├── Compile fragment shader
├── Link program
├── Activate program
└── Set uniforms
```

### Model

```text
Model
├── Read OBJ
├── Parse vertices
├── Parse texture coordinates
├── Parse faces
├── Triangulate faces
├── Generate missing UVs
├── Center model
├── Create VAO/VBO
├── Load PPM texture
└── Draw model
```

`main.cpp` coordinates these components and controls the render loop.

---

## 🔄 Runtime Flow

At startup:

```text
main()
  |
  +--> glfwInit()
  |
  +--> Create OpenGL 3.3 window
  |
  +--> Load GLAD
  |
  +--> Create Shader
  |
  +--> Create Model
  |      |
  |      +--> loadOBJ()
  |      +--> centerModel()
  |      +--> createBuffers()
  |      +--> loadTexture()
  |
  +--> Enable depth testing
  |
  +--> Enter render loop
```

Each frame:

```text
Poll events
    |
    v
Process keyboard input
    |
    v
Update texture blending
    |
    v
Clear color + depth buffers
    |
    v
Build model matrix
    |
    v
Build projection matrix
    |
    v
Set shader uniforms
    |
    v
Draw model
    |
    v
Swap buffers
```

---

## 🔍 Error Handling

The implementation reports errors through `std::cerr`.

Examples include:

- GLFW initialization failure.
- Window creation failure.
- GLAD initialization failure.
- Missing shader files.
- Shader compilation errors.
- Shader linking errors.
- Missing OBJ files.
- Invalid OBJ position indices.
- Missing texture files.
- Unsupported texture formats.
- Invalid PPM headers.
- Texture read errors.
- Missing shader uniforms.

The renderer also checks that a model contains renderable vertices before drawing.

---

## 🧹 Resource Management

The project uses destructors to release OpenGL resources.

`Shader::~Shader()` releases the shader program.

`Model::~Model()` releases:

```text
VAO
VBO
Texture
```

The application destroys the GLFW window and terminates GLFW before exiting.

---

## 📚 Resources

- [OpenGL Documentation](https://www.opengl.org/documentation/)
- [OpenGL Wiki](https://www.khronos.org/opengl/wiki/)
- [Learn OpenGL](https://learnopengl.com/)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [Khronos GLSL Documentation](https://www.khronos.org/opengl/wiki/Core_Language_(GLSL))
- [Wavefront OBJ — Wikipedia](https://en.wikipedia.org/wiki/Wavefront_.obj_file)
- [3D Projection — Wikipedia](https://en.wikipedia.org/wiki/3D_projection)
- [Matrix Multiplication — Wikipedia](https://en.wikipedia.org/wiki/Matrix_multiplication)
- [42 Network](https://www.42network.org/)
- [42 Madrid](https://www.42madrid.com/)

---

## 🤖 AI Assistance

AI assistance was used during the development of this project for:

- Understanding OpenGL concepts.
- Clarifying the 3D rendering pipeline.
- Reviewing the project structure.
- Structuring and drafting this README.

The implementation should be understood and explainable by the student during the project evaluation.

---

## 📜 License

This project is part of the 42 curriculum.
