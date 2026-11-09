# Getting Started

## What you can do today

### Desktop editor (Windows)
- Create and edit 2D scenes
- Add rectangles, circles, triangles, and sprites
- Transform objects (position, rotation, scale, size)
- Save and load versioned `.vscene` files
- Windows CI packages a runnable editor folder

### Runtime (Windows)
- Standalone Qt-free executable
- Loads a `.vscene` from the command line
- Opens an SDL window
- Escape and window close exit cleanly
- **Rendering is not implemented yet** (window is blank by design until bgfx is integrated)

### Browser demos
- [Veyra Adventure](https://github.com/shonibareakanni009-ctrl/veyra-engine/tree/main/website/game) — standalone platform demo
- [Space Flight Simulator](https://github.com/shonibareakanni009-ctrl/veyra-engine/tree/main/website/sfs) — standalone 2D flight prototype

These browser games are **not** built by the C++ engine yet.

## Building from source

Requirements (typical):
- CMake
- C++20 compiler (MSVC recommended on Windows)
- Qt 6 (editor only)
- SDL3 (runtime)

See repository `README.md` and `engine/` CMake files for the exact configure/build flow used by CI.

## Sample scene

The runtime package includes `first-scene.vscene`. Launch example:

```text
VeyraRuntime.exe first-scene.vscene
```

Expect a blank window until Task 2 (bgfx) and Task 3 (shape rendering) are complete.
