# ADR-001: Select the Initial Implementation and Tooling Stack

- **Status:** Accepted as the initial project baseline; pending prototype validation
- **Date:** 2026-11-02
- **Decision owner:** Project owner
- **Related documents:** docs/ARCHITECTURE.md, docs/TECH_STACK.md, docs/CODING_STANDARDS.md

## Context

Veyra aims to become a cross-platform 2D/3D game engine with a visual editor, a code editor, and multiple gameplay scripting languages. Planned game targets include Windows, Linux, Android, and the Web.

The engine needs a performant native runtime, an extensible platform layer, a rendering strategy that can reach the planned targets, and a stable boundary between the native engine and embedded scripting runtimes.

The project has no working engine implementation yet. This decision is a selected starting baseline, not a proven production architecture.

## Decision

Select the following starting stack:

- **C++20** for the native engine core.
- **CMake** for build configuration and orchestration.
- **Ninja** as the default local build generator where available.
- **SDL3** for initial windowing, input, events, and selected platform services.
- **bgfx** as the initial graphics-backend abstraction.
- **Qt 6 Widgets** for the first desktop editor UI.
- **Lua 5.5** as the first embedded gameplay scripting language.
- **CTest** for test orchestration.
- A **versioned C-compatible engine API** as the intended native boundary for language adapters and native extensions.

Adopt the comment-heavy policy in docs/CODING_STANDARDS.md from the first source file onward.

## Rationale

### C++20

A native C++ core provides direct integration with graphics and operating-system APIs, control over resource lifetimes, and a broad ecosystem of libraries and tooling. C++20 is modern enough for the project's abstractions while avoiding a requirement to use the newest language standard before the toolchain is proven.

The engine will expose a C-compatible boundary where interoperability matters. This prevents future scripting adapters from depending on compiler-specific C++ ABI details.

### CMake and Ninja

CMake supports multiple generators and platform toolchains. Ninja provides a lightweight default build executor. SDL3's documentation includes CMake-based build paths and describes Android and Emscripten workflows.

### SDL3

SDL3 is selected as the first platform service provider because its documented platform list includes Windows, Linux, Android, and Emscripten. Veyra will wrap it behind internal interfaces to prevent direct SDL dependencies from spreading throughout the engine.

### bgfx

bgfx is selected because its documented backend and platform matrix includes technologies relevant to Veyra's goals, including Direct3D, Vulkan, OpenGL ES, WebGL 2, Android, Linux, Windows, and WebAssembly/Emscripten.

bgfx does not replace the engine's own renderer systems. Veyra remains responsible for 2D/3D rendering features, scene integration, materials, shaders/assets, resource ownership, and editor workflows.

### Qt 6 Widgets

Qt is selected for the desktop editor because the initial editor requires a complex desktop interface with dockable panels, inspectors, tree views, dialogs, and project tools. The initial editor target is desktop Windows and Linux; it is separate from the exported game runtime.

The exact Qt modules and licensing route must be reviewed before distribution. Qt's open-source offering includes LGPL and GPL terms, and some modules are not available under LGPL.

### Lua 5.5

Lua is selected as the first scripting language because it is designed for embedding and exposes a C API. It gives the project one practical language integration to validate before supporting additional runtimes.

The project will not assume Lua support automatically proves Python, JavaScript, Kotlin, or Java support.

## Consequences

### Positive

- The core has a clear native implementation baseline.
- Build and platform responsibilities have clear starting libraries.
- Rendering is not tied immediately to one graphics API.
- The scripting design has a clear C-compatible integration boundary.
- The desktop editor can be developed separately from exported games.
- Code documentation expectations are explicit from the beginning.

### Costs and risks

- C++ requires careful ownership, lifetime, concurrency, and error-handling discipline.
- SDL3 and bgfx still need target-by-target integration and testing.
- bgfx's upstream build uses GENie; the CMake integration route must be proven and pinned.
- Qt introduces module-selection and licensing-compliance work.
- Lua is one supported scripting runtime, not a solution for every future target/language combination.
- C-compatible API design and versioning add initial engineering work.

## Alternatives considered

### Rust as the core language

Rust remains technically viable and may offer memory-safety advantages. C++ was selected for the initial baseline because the project is prioritizing mature native game/graphics integration and a broad C/C++ interoperability ecosystem. Rust may still be used for isolated tools or future components if a clear benefit justifies the added cross-language boundary.

### Kotlin or Java as the core language

These languages remain relevant to future tooling and scripting goals, but their managed runtime and platform-specific execution models make them less suitable as the single native core shared by desktop, Android, and Web targets.

### One scripting language for all engine implementation

Rejected. Gameplay scripting and engine implementation have different responsibilities. The engine core remains native C++; scripting languages are adapters over the common engine API.

### A different renderer

A custom renderer or another abstraction library may ultimately be appropriate. bgfx is the starting choice because its published backend matrix aligns with the target goals. A small rendering prototype is required before treating it as permanent.

## Validation required

This ADR is not fully validated until the project can:

1. Configure with CMake on a desktop host.
2. Build a small C++20 executable.
3. Create a window and process an input/close event through SDL3.
4. Initialize bgfx using the SDL-created native window/surface information.
5. Clear the render surface and shut down cleanly.
6. Run a repeatable test/build command.
7. Demonstrate a Lua script calling one engine API through the intended binding boundary.
8. Record the actual platform, compiler, dependency revisions, and test results.

Android and Web builds require separate validation. A desktop success does not prove support for those targets.

## Revisit this decision if

- A required target cannot be built or run using the selected stack.
- bgfx integration prevents a necessary rendering feature or platform path.
- Qt licensing or technical requirements conflict with project distribution goals.
- The chosen toolchain cannot be maintained by the project.
- Prototype measurements show a material performance or reliability problem.
- New evidence is strong enough to outweigh the cost of changing the baseline.

## References

- [SDL3 platform documentation](https://github.com/libsdl-org/SDL/blob/main/docs/README-platforms.md)
- [SDL3 CMake documentation](https://github.com/libsdl-org/SDL/blob/main/docs/README-cmake.md)
- [bgfx overview](https://bkaradzic.github.io/bgfx/overview.html)
- [bgfx build instructions](https://bkaradzic.github.io/bgfx/build.html)
- [bgfx.cmake](https://github.com/bkaradzic/bgfx.cmake)
- [Qt supported platforms](https://doc.qt.io/qt-6/supported-platforms.html)
- [Qt licensing](https://doc.qt.io/qt-6/licensing.html)
- [Lua 5.5 reference manual](https://www.lua.org/manual/5.5/)
- [Lua license](https://www.lua.org/license.html)
- [CMake features](https://cmake.org/features/)
