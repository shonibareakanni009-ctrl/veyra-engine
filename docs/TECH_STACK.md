# Veyra Engine — Selected Technical Stack

**Decision date:** 2026-11-02  
**Status:** Selected baseline; subject to prototype validation  
**Purpose:** Record the initial technology choices and reasons behind them.

This document records the stack selected for Veyra Engine. It does not claim that these tools have already been integrated, built, or validated in Veyra.

## 1. Selected stack

| Area | Selection | Role |
|---|---|---|
| Engine implementation | **C++20** | Core runtime, engine systems, platform adapters, native integrations |
| Build configuration | **CMake** | Project configuration, target definitions, build orchestration |
| Default build generator | **Ninja** | Local build execution where available |
| Windowing/platform services | **SDL3** | Window/surface creation, input, events, selected platform services |
| Rendering abstraction | **bgfx** | Cross-platform rendering backend for Veyra's 2D/3D systems |
| Desktop editor UI | **Qt 6 Widgets** | Dockable panels, inspectors, dialogs, project tools |
| First scripting integration | **Lua 5.5** | First embedded gameplay scripting language |
| Language interoperability | **Versioned C-compatible engine API** | Stable boundary for language adapters and native extensions |
| Test orchestration | **CTest** | Registration and execution of tests through CMake |
| Code documentation | **Doxygen-style API comments** | Navigable documentation from source comments |

The selected C++ standard is C++20. Compiler-specific extensions must not become a requirement for ordinary engine code unless a documented platform adapter needs them.

## 2. Why C++20 for the engine core

C++20 is selected because Veyra needs a native foundation capable of controlling memory, performance-sensitive data structures, platform APIs, rendering integration, and native runtime embedding.

Reasons:
- It is widely used in game engines and native graphics tooling.
- It integrates with C libraries and operating-system APIs.
- It gives explicit control over resource lifetimes while supporting modern abstractions.
- It can expose a C-compatible boundary to scripting runtimes and native extensions.
- It can be compiled for desktop, Android native targets, and WebAssembly through suitable toolchains.

C++20 is a project baseline, not permission to make every subsystem complicated. Prefer understandable, explicit code. Use advanced language features only when they improve correctness or maintainability.

### Engine-to-language boundary

The engine's internal implementation may use C++, but external runtime adapters must not depend on a compiler-specific C++ ABI.

The intended shared boundary is a versioned C-compatible API with:
- Opaque handles for engine-owned objects.
- Fixed-width integer types for ABI-visible values.
- Explicit ownership and lifetime rules.
- Explicit creation, release, and error-reporting functions.
- Versioned API structures and capability queries.
- Documented callback and threading rules.
- No STL containers, C++ exceptions, compiler-specific templates, or C++ class layouts crossing the ABI boundary.

This is a design requirement. It has not yet been implemented.

## 3. Build system: CMake + Ninja

CMake will describe the project, modules, options, dependencies, platform configurations, and tests. Ninja will be the default build executor for local development where installed and supported.

CMake supports multiple generators and platform toolchains. SDL3 documents CMake build paths for environments including Android and Emscripten. Ninja provides a focused build executor while CMake keeps the project generator-independent.

**Initial baseline:** CMake 3.28 or newer is proposed. The exact minimum must be tested against selected dependency versions and Android/Emscripten toolchains before the first stable build configuration is declared.

### Dependency acquisition

Do not download unpinned dependency versions during ordinary builds. Dependencies must be pinned to an immutable release or commit and have their license notices recorded.

We will choose between CMake FetchContent, Git submodules, a package manager, or a small combination after proving the first dependency set. The project must not silently switch to a newer dependency revision.

## 4. Platform services: SDL3

SDL3 is selected for:
- Window and graphics-surface creation.
- Keyboard, mouse, controller, and touch input.
- Event handling.
- Application lifecycle integration where applicable.
- Selected audio and device services where the engine's needs match SDL's capabilities.

SDL3 is not Veyra's platform architecture. Veyra will wrap it behind internal interfaces so game systems do not call SDL APIs directly.

The platform adapter must account for desktop application lifecycle, Android activity lifecycle, and browser event/lifecycle restrictions.

SDL's documentation lists Windows, Linux, Android, and Emscripten among its known platforms. SDL also notes that its maintainers do not test every listed platform. This upstream list is not proof of Veyra support.

## 5. Rendering: bgfx

bgfx is selected as the initial rendering backend layer because it provides a graphics-API-agnostic rendering interface and documents backends/platforms relevant to Veyra's goals, including Direct3D, Vulkan, OpenGL ES, WebGL 2, Android, Linux, Windows, and WebAssembly/Emscripten.

Veyra will still own its renderer-facing abstractions and rendering systems:
- Render-device and resource lifetime rules.
- 2D cameras, sprites, tilemaps, batching, and layers.
- 3D cameras, meshes, materials, lighting, and render passes.
- Shader and material asset workflows.
- Render diagnostics and capability reporting.

bgfx is a backend layer, not a complete 2D/3D engine. It does not replace Veyra's scene model, materials, asset pipeline, editor, or renderer design.

### Build integration risk

bgfx's upstream build documentation uses GENie. A CMake integration project, bgfx.cmake, is available and describes a CMake build route for using bgfx as a C++ library. We will prototype this route with pinned dependencies first.

If the integration proves unreliable for a required target, we will integrate the upstream build route through a controlled CMake external-build adapter rather than silently dropping a target.

The first graphics prototype must validate at least one desktop backend. Android and Web must be tested separately before they are described as supported.

## 6. Editor: Qt 6 Widgets

Qt 6 Widgets is selected for the initial desktop editor interface. It is suitable for:
- Dockable editor panels.
- Scene hierarchy and inspector layouts.
- Menus, toolbars, dialogs, and project settings.
- Asset browser and property views.
- Debug output and profiling panels.
- A first embedded code-editing surface.

The initial editor target is Windows and Linux desktop. The game runtime/export targets remain separate: exported games must not need Qt merely because the editor uses it.

### Licensing gate

Qt is available under commercial and open-source licensing options. Open-source Qt use involves LGPL/GPL obligations, and some modules are GPL-only for open-source users.

Before distributing the editor, we must select a compatible licensing route, inspect the exact Qt modules used, and satisfy the applicable obligations. The first editor prototype should use only the modules it actually needs.

## 7. First scripting language: Lua 5.5

Lua 5.5 is selected as the first scripting language.

Reasons:
- It is designed to be embedded in applications.
- Its implementation is a compact C library.
- It exposes a C API callable from the native engine.
- It gives us one practical language integration to validate before supporting additional runtimes.

Lua scripts will call Veyra's engine-facing API through a dedicated adapter. Lua must not reach arbitrary private C++ engine objects or retain unsafe raw pointers.

The first Lua milestone must prove:
1. Runtime startup and shutdown.
2. Loading a script from a project.
3. Calling one documented engine API.
4. Passing values between Lua and the engine.
5. Returning a useful script error and traceback.
6. Managing engine object references without dangling pointers.
7. Building the integration for the first desktop target.

The exact Lua 5.5 patch release will be pinned when integration starts.

## 8. Later scripting languages

The original vision includes Python, JavaScript, Kotlin, and Java. They remain long-term goals but will not be implemented simultaneously.

Each adapter must declare compatibility per target:
- Python's native interpreter route and browser/mobile packaging need separate evaluation.
- JavaScript needs a chosen embeddable runtime and a defined relationship to browser-native JavaScript.
- Kotlin and Java need target-aware runtime choices; Android ART cannot be assumed to exist on desktop or in a browser.
- A language being installable on the editor host does not prove it can be bundled with a game for every target.

No language is supported until its adapter, engine API bindings, runtime packaging, diagnostics, and target tests exist.

## 9. Testing

CTest is selected as the initial test orchestration layer because it integrates with CMake.

Test layers:
- Unit tests for isolated core behavior.
- Contract tests for platform, rendering, and scripting interfaces.
- Integration tests for engine subsystem interactions.
- Regression tests for fixed defects and project-format migrations.
- Target tests for actual exported game builds.
- Performance tests with recorded machine and configuration details.

The test framework itself will be selected when the first real C++ test suite is added. CTest is the test runner/orchestration layer, not the assertion library.

## 10. Platform intent versus verified support

| Target | Engine intent | SDL3 upstream path | bgfx upstream path | Veyra status |
|---|---|---|---|---|
| Windows | Native C++ | Documented | Documented | Not yet built or tested |
| Linux | Native C++ | Documented | Documented | Not yet built or tested |
| Android | Android native toolchain | Documented | Documented | Not yet built or tested |
| Web | WebAssembly/Emscripten | Documented | Documented | Not yet built or tested |

"Documented" refers to upstream project documentation, not a successful Veyra build. Browser builds have special constraints around filesystem access, threading, networking, and runtime execution.

## 11. Known trade-offs and review triggers

| Selection | Benefit | Risk/cost | Reconsider if |
|---|---|---|---|
| C++20 | Native integration and control over resource lifetimes | Ownership and concurrency complexity | Toolchain coverage or maintainability becomes unacceptable |
| CMake + Ninja | Cross-platform build configuration and efficient execution | Toolchain/dependency configuration needs discipline | Required platforms cannot share the build model |
| SDL3 | Broad platform/input/window integration | Platform lifecycle details remain | A target requires functionality SDL cannot provide |
| bgfx | Multiple graphics backends behind a common renderer API | Integration, shader pipeline, backend behavior need validation | A required target or feature fails the prototype |
| Qt 6 Widgets | Mature desktop editor UI | Licensing obligations and dependency size | Terms or technical integration do not fit |
| Lua 5.5 | Embeddable C library and approachable first integration | Dynamic typing and adapter maintenance | Target testing exposes a blocking compatibility issue |

## 12. First validation milestone

The first vertical slice should be a small Windows or Linux desktop application that:
1. Configures and builds through CMake.
2. Creates a window and handles a close/input event through SDL3.
3. Initializes bgfx using the SDL-created native window/surface information.
4. Clears the render surface to a known color.
5. Shuts down rendering and platform services in a documented order.
6. Includes repeatable build and run instructions.
7. Has comments explaining module ownership, initialization order, and shutdown order.

The next scripting slice should embed Lua and call one engine API. Do not build the full editor, physics, audio, Android exporter, Web exporter, or every language adapter before these narrow paths are proven.

## 13. Upstream references

- [SDL3 platform documentation](https://github.com/libsdl-org/SDL/blob/main/docs/README-platforms.md)
- [SDL3 CMake build documentation](https://github.com/libsdl-org/SDL/blob/main/docs/README-cmake.md)
- [SDL3 Android integration notes](https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md)
- [SDL3 Emscripten integration notes](https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-emscripten.md)
- [bgfx overview and supported backends](https://bkaradzic.github.io/bgfx/overview.html)
- [bgfx upstream build instructions](https://bkaradzic.github.io/bgfx/build.html)
- [bgfx.cmake integration](https://github.com/bkaradzic/bgfx.cmake)
- [Qt 6 supported platforms](https://doc.qt.io/qt-6/supported-platforms.html)
- [Qt 6 licensing](https://doc.qt.io/qt-6/licensing.html)
- [Lua 5.5 reference manual](https://www.lua.org/manual/5.5/)
- [Lua license](https://www.lua.org/license.html)
- [CMake features and generators](https://cmake.org/features/)

---

**Current truth:** These are selected technology decisions and a validation plan. No dependency has yet been integrated into Veyra, and no platform build or prototype is being claimed as complete.
