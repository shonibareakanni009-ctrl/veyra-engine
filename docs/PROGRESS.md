# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-03

This file records implemented work and its verification status. Plans remain in ROADMAP.md.

## Overall status

| Metric | Current state |
|---|---|
| Current phase | Phase 1 — 2D editor and authoring vertical slice |
| Project stage | Early functional editor prototype |
| Core language | C++20 |
| Desktop editor | Qt 6 Widgets application; compilation verified in Windows CI |
| Scene authoring | Rectangle, circle, triangle, and imported image sprites can be created; manual end-to-end validation still pending |
| Scene persistence | Versioned .vscene JSON with atomic writes and input validation; primitive shape and project-relative sprite asset references are serialized and regression-tested |
| Automated tests | Round-trip, unsupported-version, invalid-dimension, duplicate-ID, and modified-state tests added; Windows CI result pending |
| Runtime integration | Not implemented |
| Browser SFS prototype | Implemented as a separate HTML Canvas 2D flight game; browser/device playtest pending |
| Export targets | Engine-produced targets not implemented |

**Reality statement (2026-11-03):** The repository contains a C++20/Qt 6 editor codebase, an editable 2D scene document and viewport, a Windows build workflow, and a standalone browser game called Veyra Adventure. The latest Windows workflow compiled the editor successfully, but package creation failed because the MSVC runtime DLLs were not copied beside the executable. A passing compile is not the same as a verified portable editor release.

## Implemented in source

| Item | Evidence | Verification status |
|---|---|---|
| Qt desktop editor shell and dock panels | editor/src/editor_main_window.cpp | Source present; Windows compile passed |
| Versioned scene document with stable object IDs | editor/src/scene_document.cpp | Source present; automated tests added, CI pending |
| Atomic .vscene save and validated load | editor/src/scene_document.cpp | Source present; automated tests added, CI pending |
| 2D viewport with grid, object selection, and drag movement | editor/src/scene_viewport.cpp | Source present; manual runtime verification pending |
| Hierarchy rename and selection synchronization | editor/src/editor_main_window.cpp | Source present; manual runtime verification pending |
| Common scene shortcuts | editor/src/editor_main_window.cpp | Ctrl+N, Ctrl+O, Ctrl+S, Ctrl+Shift+S, Ctrl+Shift+N, and Delete wired; CI pending |
| Inspector for position, rotation, scale, width, and height | editor/src/editor_main_window.cpp | Source present; manual runtime verification pending |
| Primitive custom objects | editor/src/scene_document.h, editor/src/scene_viewport.cpp | Rectangle, circle, triangle creation, rendering, hit-testing and .vscene shape persistence implemented; CI pending |
| Sprite import and rendering | editor/src/editor_main_window.cpp, editor/src/scene_viewport.cpp | Imports PNG/JPG/BMP/GIF/WebP images into a scene-local Assets folder, creates sprite objects, persists relative paths, and draws the image in the viewport; CI pending |
| 2D Space Flight Simulator browser prototype | website/sfs/ | Canvas flight controls, gravity, thrust, fuel, beacon objective, HUD, pause/restart and touch controls implemented; browser playtest pending |
| Scene test projects and acceptance criteria | docs/testing/2d-engine-test-projects.md | Documented |

## Verified results

- Windows CI configured the Qt 6 editor and compiled the editor executable successfully at commit 69412f48252b162a5f58b2bad2731792c01419ad.
- The same run failed during runtime packaging: windeployqt staged the Qt files but did not copy vcruntime140.dll beside the executable. A later workflow revision adds explicit CRT copying and validation; its result must be checked before calling packaging fixed.
- The standalone website/game is present under website/game/; README identifies it as Veyra Adventure. It is not yet produced by the engine. The new Veyra SFS browser prototype is under website/sfs/ and is also separate from the native engine runtime.

## In progress

| Item | Next proof required |
|---|---|
| Portable Windows editor package | A green Windows Editor Build run with Qt platform plugin and MSVC CRT DLL checks passing |
| Scene document automated tests | Passing CTest output in Windows CI |
| Unsaved-change protection | Compile and test through the Windows editor build; manual UI check still required |
| Manual editor smoke test | Launch packaged app; create, transform, save, close, reopen, and inspect a scene |
| Runtime integration | Load an editor-authored .vscene in a separate game runtime |

## Not implemented yet

- Engine runtime and renderer integration
- Project creation/opening and project settings
- Arbitrary custom polygon authoring and a searchable/thumbnail asset browser
- Parent/child scene hierarchy and reusable prefabs
- Runtime physics, animation, audio, particles, and game scripting
- Code editor and Lua execution
- Game build/export pipeline
- Android, Linux, and engine-produced Web targets
- Installer, updater, uninstaller, and release signing
- Multiplayer/networking and 3D editor workflows

## Immediate next actions

1. Confirm the latest Windows build, sprite persistence tests, and complete-folder packaging run are green.
2. Manually smoke-test the packaged editor before inviting downloads.
3. Connect the .vscene document format to a real game runtime.
4. Continue preparing the Level 1 test projects: Veyra Adventure (browser baseline) and Veyra SFS (browser prototype now exists; native engine integration remains future work).
5. Keep progress claims tied to code and verifiable CI or runtime evidence.
