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
| `CMakePresets.json`           | Debug and Release build setups for CMake and editors |
| `.vscode/extensions.json`     | VS Code extensions recommended for this project      |
| `third_party/glad/`           | OpenGL function loader (generated, checked in)       |
| `.github/workflows/build.yml` | Builds every push on Windows, Linux and macOS        |

[GLFW](https://www.glfw.org) 3.4 (window, OpenGL context, input) is downloaded
and built automatically the first time CMake configures the project.

## Prerequisites

All platforms need [CMake](https://cmake.org/download/) 3.21 or newer, Git, and
a C compiler.

- **Windows:** one of these, each with the "Desktop development with C++"
  workload selected in the installer:
  - **Visual Studio 2026**, if you want the full IDE.
  - **Build Tools for Visual Studio 2026**, if you only want the compiler and
    will use VS Code as the editor.

  Both include a copy of CMake, but neither puts it on the `PATH`, so `cmake`
  does not work in an ordinary terminal. The simplest fix is to install the
  official CMake as well, which adds itself to the `PATH`:

  ```
  winget install -e --id Kitware.CMake
  ```

  Use CMake 4.2 or newer; older versions cannot find Visual Studio 2026.
- **macOS:** Xcode command line tools (`xcode-select --install`), and CMake,
  for example via `brew install cmake`.
- **Linux:** a compiler plus the X11 and Wayland development packages.
  - Debian / Ubuntu:
    `sudo apt install build-essential cmake git libwayland-dev libxkbcommon-dev xorg-dev libgl1-mesa-dev`
  - Fedora:
    `sudo dnf install gcc cmake git wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel mesa-libGL-devel`

## Build and run

In every editor, open the **repository folder itself** (the one containing
`CMakeLists.txt`). The `build/` folder is generated output: never open it as a
project and never edit anything in it. You can delete it at any time to start a
clean build.

### Visual Studio Code (Windows, Linux, macOS)

1. Install [VS Code](https://code.visualstudio.com/).
2. *File > Open Folder…* and select the repository folder. VS Code offers to
   install the recommended extensions, **C/C++** and **CMake Tools**; accept.
3. CMake Tools finds `CMakePresets.json` and asks for a configure preset.
   Choose **Debug**. (Later you can switch between Debug and Release in the
   CMake panel in the sidebar.)
4. Build with `F7`. To start the program, use the buttons in the status bar
   at the bottom: the bug icon runs it in the debugger (so breakpoints work),
   the triangle runs it without the debugger.

If CMake Tools reports that it cannot find a compiler on Windows, close VS
Code, open the **Developer Command Prompt for VS 2026** from the Start menu,
and start VS Code from there with `code .` inside the repository folder.

### Visual Studio 2026 (Windows)

*File > Open > Folder…* and select the repository folder. Visual Studio reads
`CMakePresets.json`; choose the Debug or Release preset in the toolbar, then
press `F5`.

### Command line

```sh
cmake --preset debug
cmake --build --preset debug
```

Then start the program:

- Linux / macOS: `./build/debug/app3d`
- Windows: `build\debug\Debug\app3d.exe`

For an optimised build, replace `debug` with `release` in all of the above.

On Windows this works in any terminal once `cmake` is on the `PATH` (see
Prerequisites). The presets use CMake's Visual Studio generator, which finds
the compiler by itself, so the Developer Command Prompt is not needed.

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
