# Phase 2B — bgfx Runtime Renderer Implementation Plan

**Status:** Plan committed; implementation not started.
**Scope:** Windows x64 first; integrate bgfx with the existing Qt-free SDL3 runtime.
**Prerequisite:** The packaged runtime was manually launched by the project owner on 2026-11-09. The SDL window opened successfully and was blank, which is the expected current behavior because scene rendering is not implemented.

## Goal

Render the supported primitive objects from a validated version 1 `.vscene` document in the standalone runtime. The first vertical slice covers rectangles, circles, and triangles, including visibility, position, dimensions, rotation, and scale. This milestone does not include sprite textures, physics, animation, or editor Run/Stop integration.

## Existing repository baseline inspected before implementation

- Runtime entry point: `engine/runtime/src/main.cpp`.
- Runtime build: `engine/runtime/CMakeLists.txt`; SDL3 3.4.18 and nlohmann/json 3.12.0 are pinned and SHA-256 verified.
- Scene contract and validation: `engine/runtime/src/scene_document.h/.cpp`.
- Runtime remains Qt-free and independently buildable.
- The existing Phase 2 execution plan already records the preferred bgfx.cmake release and immutable commit, SDL3 HWND extraction approach, and first-primitive scope.

## Implementation sequence

1. **Record the manual launch result**
   - Mark the SDL window open/close test as owner-reported and passed.
   - Explicitly record that the window was blank and that this is not a renderer pass.

2. **Integrate the pinned bgfx dependency**
   - Use bgfx.cmake commit `eb88f6be55ae223966698c802542c24b0d93fd25` with recursive submodules.
   - Configure only the required bgfx libraries and shader compiler; disable unrelated examples, tests, and tools.
   - Keep all dependency versions immutable and avoid floating branches/tags.
   - Confirm the actual CMake options exposed by the pinned revision before setting them; do not guess option names.

3. **Add a platform-window adapter**
   - Obtain the SDL3 native Windows HWND from SDL window properties.
   - Populate bgfx platform data before initialization.
   - Isolate Win32-specific handle extraction so platform code does not leak into scene or geometry code.
   - Fail with an actionable diagnostic if the native handle or renderer initialization is unavailable.

4. **Add renderer-independent scene geometry**
   - Convert validated rectangle, circle, and triangle scene objects into deterministic vertex/index data.
   - Apply visibility, position, dimensions, rotation, and scale consistently with the editor's scene coordinate convention.
   - Add unit tests for geometry counts, transforms, hidden objects, and degenerate/invalid dimensions.
   - Keep sprite assets explicitly unsupported in this milestone and report them rather than drawing misleading placeholders.

5. **Add bgfx rendering lifecycle**
   - Initialize after SDL window creation; set view size and clear state.
   - Compile/embed the minimal shaders through the pinned shader compiler path.
   - Upload frame geometry and submit draw calls.
   - Handle resize, frame progression, shutdown ordering, and all initialization failure paths.
   - Ensure SDL and bgfx resources are released exactly once.

6. **Update build and CI**
   - Update runtime CMake and Windows runtime workflow for recursive dependency checkout and shader compilation.
   - Keep editor build independent from bgfx.
   - Add geometry tests to CTest and ensure the runtime CI compiles/packages the renderer.
   - Preserve dependency hashes/pins and avoid unnecessary CI build targets.

7. **Manual visual acceptance**
   - Run the packaged runtime with the sample scene.
   - Confirm the rectangle, circle, and triangle are visibly rendered with expected transforms.
   - Resize the window, then close it and use Escape; verify clean shutdown.
   - Record screenshots or a concise owner test result if available.

## Acceptance criteria

- Windows x64 runtime builds with bgfx and remains Qt-free.
- Deterministic geometry tests pass.
- A scene authored by the editor visibly renders rectangles, circles, and triangles.
- Resize and shutdown work without crashes or resource-lifecycle errors.
- Unsupported sprite assets are clearly diagnosed and are not misrepresented as rendered.
- CI success is recorded separately from manual visual acceptance.

## Out of scope

- Editor Run/Stop process integration (next checkpoint).
- Sprite texture loading/rendering.
- Other operating systems and graphics backends.
- Physics, animation, audio, particles, scripting, and export.

## Risk controls

- Validate the exact pinned bgfx.cmake options and generated targets before changing build configuration.
- Keep the scene loader and geometry conversion testable without a GPU/window.
- Do not claim visual rendering based on compilation or headless tests alone.
- If the dependency or shader build proves unsuitable, document evidence and revise this plan before switching dependencies.
