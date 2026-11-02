# ADR-002: Validate the Browser Target First

- **Status:** Accepted implementation order
- **Date:** 2026-11-02
- **Decision owner:** Project owner
- **Related documents:** docs/TECH_STACK.md, docs/ARCHITECTURE.md, docs/decisions/ADR-001-implementation-stack.md

## Context

Veyra is intended to become a cross-platform 2D and 3D game engine. Windows 10 and Windows 11 remain important desktop targets, but a suitable Windows development machine is not currently available for regular use.

The project can make progress by validating a browser build first. The browser target offers a practical way to compile the native C++ core to WebAssembly and inspect the first running prototype through a modern browser.

## Decision

1. The first implementation and test target will be the browser.
2. C++20 remains the engine implementation language.
3. Emscripten is selected as the initial C++ to WebAssembly toolchain.
4. SDL3 remains the initial platform and application-lifecycle layer.
5. The desktop editor remains a separate native application; this browser prototype is not the editor.
6. Windows 10 and Windows 11 remain planned desktop targets and will be tested when suitable hardware is available.
7. Linux and Android remain planned targets; they have not been removed from the project vision.
8. The prototype may use SDL_Renderer for a minimal first visual test. This is temporary and does not replace bgfx as the selected long-term rendering backend.

## Technical consequences

- The first browser application must yield control between frames instead of trapping the browser in a permanent game loop.
- Browser output consists of web assets such as HTML, JavaScript, and WebAssembly rather than a Windows executable.
- The output must be served over HTTP or HTTPS; directly opening the generated HTML file is not a reliable test method.
- Browser filesystem, networking, input, threading, graphics, and security restrictions must be treated as platform constraints.
- The Emscripten toolchain and dependency revisions must be pinned for repeatable builds.
- Browser compilation alone does not prove that the application runs correctly in a browser.
- The bgfx WebGL 2 path must be validated separately before Veyra claims browser rendering support.

## Validation gates

The browser-first decision is validated incrementally:

1. Configure the prototype with CMake through emcmake.
2. Build the C++20 source and pinned SDL3 dependency.
3. Produce HTML, JavaScript, and WebAssembly outputs.
4. Serve the output over HTTP and load it in a browser.
5. Verify input, update timing, rendering, resizing, refresh, and clean startup.
6. Record actual browser/device results.
7. Integrate and validate bgfx before treating the browser renderer as an engine capability.

## Revisit this order if

- A required browser API or dependency makes the target impractical.
- The available cloud build path cannot reliably build the prototype.
- A suitable Windows development environment becomes available and desktop validation becomes the more valuable next step.
- Prototype evidence shows that another target should be prioritized.

This decision changes implementation order only. It does not narrow Veyra's cross-platform vision or remove Windows 10 and Windows 11 from the planned desktop targets.
