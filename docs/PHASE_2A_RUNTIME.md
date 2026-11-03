# Phase 2A — Native Runtime Implementation Plan

**Status:** Implementation started  
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

1. Add a separately configured C++20 runtime build under `runtime/`; do not make the editor a runtime dependency.
2. Pin dependency versions and verify downloaded archives with SHA-256.
3. Add an SDL3 application lifecycle: initialize, create a window, process close and Escape events, then destroy resources and shut down cleanly.
4. Add a Qt-free version 1 `.vscene` loader using a small standalone JSON dependency.
5. Expose a command-line scene path so the editor can later launch the runtime with the saved scene.
6. Add tests for valid scenes and malformed/unsafe/unsupported data.
7. Add a Windows x64 GitHub Actions workflow that configures, builds, tests, and packages the runtime.

**Checkpoint 1 acceptance:** Windows CI compiles the separate runtime and passes scene-loader tests. Running the app with a valid scene opens an SDL window and reports the loaded object count. This checkpoint does not claim that scene objects are rendered.

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
- [ ] Separate runtime target.
- [ ] SDL3 window and event lifecycle.
- [ ] Qt-free `.vscene` parser and validation tests.
- [ ] Windows x64 runtime CI.
- [ ] bgfx initialization and first rendered scene object.
- [ ] Editor Run/Stop controls.

## Verification record

No runtime build or runtime behavior is claimed as verified until the Windows runtime workflow has completed successfully.