# 3d-dev-learning

A starting point for 3D applications in C. It opens a window with an OpenGL 4.1
core context and draws a rotating triangle, and it builds on Windows, Linux and
macOS.

| Key                  | Action                       |
| -------------------- | ---------------------------- |
| `F11` or `Alt+Enter` | Toggle windowed / fullscreen |
| `Esc`                | Quit                         |

On macOS `F11` is usually taken by the system ("Show Desktop"), so use
`Option+Enter` there.

## What is inside

| Path                          | Purpose                                              |
| ----------------------------- | ---------------------------------------------------- |
| `src/main.c`                  | The application: window, input, shaders, render loop |
| `CMakeLists.txt`              | Build description for all platforms                  |
| `third_party/glad/`           | OpenGL function loader (generated, checked in)       |
| `.github/workflows/build.yml` | Builds every push on Windows, Linux and macOS        |

[GLFW](https://www.glfw.org) 3.4 (window, OpenGL context, input) is downloaded
and built automatically the first time CMake configures the project.

## Prerequisites

All platforms need [CMake](https://cmake.org/download/) 3.21 or newer, Git, and
a C compiler.

- **Windows:** Visual Studio 2022 with the "Desktop development with C++"
  workload (it includes the C compiler and CMake).
- **macOS:** Xcode command line tools (`xcode-select --install`), and CMake,
  for example via `brew install cmake`.
- **Linux:** a compiler plus the X11 and Wayland development packages.
  - Debian / Ubuntu:
    `sudo apt install build-essential cmake git libwayland-dev libxkbcommon-dev xorg-dev libgl1-mesa-dev`
  - Fedora:
    `sudo dnf install gcc cmake git wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel mesa-libGL-devel`

## Build and run

The commands are the same everywhere:

```sh
cmake -S . -B build
cmake --build build
```

Then start the program:

- Linux / macOS: `./build/app3d`
- Windows: `build\Debug\app3d.exe`

On Windows you can also open the folder directly in Visual Studio
(*File > Open > Folder*), or open the generated `build/app3d.sln`.

For an optimised build, use `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` on
Linux and macOS, and `cmake --build build --config Release` on Windows.

## Notes

- **OpenGL version:** 4.1 core is the newest version macOS supports, so the
  template targets it on every platform. Shaders therefore start with
  `#version 410 core`.
- **C standard:** the project is set to C17 because it is the newest standard
  that MSVC supports. To use C23 with GCC or Clang, change `CMAKE_C_STANDARD`
  in `CMakeLists.txt` to `23`.
- **Regenerating glad:** the loader was generated with
  [glad 2](https://github.com/Dav1dde/glad) for OpenGL 4.1 core without
  extensions. To change the version or add extensions:

  ```sh
  pip install glad2
  python -m glad --api gl:core=4.1 --extensions "" --out-path third_party/glad c
  ```
