# Phase 2A — Native Runtime Implementation Plan

**Status:** Runtime foundation and shared-folder build verified in Windows CI; manual desktop launch remains  
**Last updated:** 2026-11-03  
**Scope:** First Windows x64 runtime vertical-slice foundation

This plan turns the Phase 2A roadmap into small, verifiable implementation checkpoints. It does not mark a checkpoint complete until its code is built and its tests pass in CI.

## Existing contract to preserve

The editor writes version 1 scene documents with this top-level shape:

- `format`: `"veyra-scene"`
- `formatVersion`: `1`
- `objects`: array of objects
- Object fields: `id`, `name`, `shape`, `assetPath`, `x`, `y`, `width`, `height`, `rotation`, `scaleX`, `scaleY`, and `visible`

The runtime must load this format without linking Qt. Unsafe relative asset paths, duplicate IDs, unsupported shapes, invalid transforms, malformed JSON, and unsupported format versions must be rejected.

## Checkpoint 1 — Standalone runtime and scene loader

1. Keep a separately configured C++20 runtime build under `engine/runtime/`; do not make the editor a runtime dependency.
2. Pin dependency versions and verify downloaded archives with SHA-256.
3. Add an SDL3 application lifecycle: initialize, create a window, process close and Escape events, then destroy resources and shut down cleanly.
4. Add a Qt-free version 1 `.vscene` loader using a small standalone JSON dependency.
5. Expose a command-line scene path so the editor can later launch the runtime with the saved scene.
6. Add tests for valid scenes and malformed/unsafe/unsupported data.
7. Add a Windows x64 GitHub Actions workflow that configures, builds, tests, and packages the runtime.

**Checkpoint 1 automated acceptance:** Passed on Windows x64: the standalone runtime configured and compiled, scene-loader tests passed, and a distributable runtime package was staged and uploaded. The CI job does not launch a GUI window.

**Checkpoint 1 manual acceptance still open:** Run the packaged executable with `first-scene.vscene`, confirm the SDL window opens and reports three loaded objects, then confirm Escape and the window close button both exit cleanly. Scene-object rendering is not part of this checkpoint and is not claimed.

## Checkpoint 2 — Renderer integration

1. Integrate the selected bgfx backend with the SDL-created native window.
2. Add platform-window-handle extraction behind a runtime platform adapter.
3. Initialize/shut down bgfx in the correct order and handle resize/device failures.
4. Render a deterministic first primitive from the loaded scene.
5. Add a test scene and documented manual verification steps.

**Acceptance:** the validated Windows x64 runtime visibly renders at least one editor-authored scene object and exits cleanly.

## Checkpoint 3 — Editor Run/Stop integration

1. Add runtime executable discovery/configuration to the editor.
2. Save or require a saved scene before launch.
3. Launch the runtime with the active scene path and capture process output/exit status.
4. Add Stop behavior that terminates only the process started by the editor.
5. Report missing runtime and launch failures without crashing the editor.

**Acceptance:** an owner-run manual test creates/saves a scene, launches it from the editor, confirms the runtime window, then stops it from the editor.

## Dependency and trust rules

- Dependencies must use immutable version pins and checked archive hashes.
- The runtime must not link Qt.
- The scene loader must parse into a temporary document and replace its output only after full validation.
- Scene paths are data, not permission to load arbitrary files; only safe project-relative asset paths are accepted.
- No downloaded or project-provided executable code is run by the loader.
- CI must build/test the runtime independently from the editor workflow.

## Current implementation status

- [x] Repository and current editor scene format inspected.
- [x] Implementation plan recorded before code changes.
- [x] Separate Qt-free runtime target — Windows x64 build passed.
- [x] SDL3 window/event lifecycle implementation compiles — manual window behavior test pending.
- [x] Qt-free `.vscene` parser and validation tests — CI tests passed.
- [x] Windows x64 runtime CI — configure, compile, tests, and package upload passed.
- [ ] Manual launch/close smoke test using the packaged sample scene.
- [ ] bgfx initialization and first rendered scene object.
- [ ] Editor Run/Stop controls.
- [x] Shared `engine/` layout and independent Windows x64 editor/runtime CI verified.
- [x] Move editor and runtime source trees under shared `engine/` directory (new-path CI verification pending).

## Verification record

### Shared engine layout CI verification — 2026-11-03

- Editor workflow: [Windows Editor Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685837063) — success, including configure, compile, scene tests, Qt deployment, and artifact upload.
- Runtime workflow: [Windows Runtime Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685729965) — success, including configure, compile, scene tests, package staging, and artifact upload.
- The workflows build the editor and runtime independently from the shared engine root. The runtime remains Qt-free.
- This does not verify manual SDL window behavior or object rendering.


### Windows CI verification — 2026-11-03

- Workflow: [Build and test Veyra Runtime for Windows x64](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41682363212)
- Verified commit: `b6bd682b82d222f0e6cacc0a5cba2ae81428ed4d`
- Result: **success** — CMake configure, C++ build, scene-loader tests, package staging, and artifact upload all passed.
- Artifact: `veyra-runtime-windows-x64` (includes `VeyraRuntime.exe`, app-local MSVC runtime DLLs, README, and `first-scene.vscene`).
- The workflow verifies compilation and scene validation; it does not launch the SDL window. Manual GUI behavior is still unverified.

The runtime source now lives under `engine/runtime/`. The shared top-level CMake entry point can configure editor and runtime independently. The editor and runtime now live under the shared `engine/` directory. The top-level `engine/CMakeLists.txt` selects either application independently. Windows x64 CI passed after the move for both editor and runtime. The runtime currently validates and loads scene data, then opens an SDL window and processes close/Escape events in its application loop. It does **not** render scene objects yet. bgfx integration and the manual window smoke test remain open.