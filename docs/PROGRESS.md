# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-03

This file records implemented work and its verification status. Plans remain in ROADMAP.md.

## Overall status

| Metric | Current state |
|---|---|
| Current phase | Phase 1 — 2D editor and authoring vertical slice |
| Project stage | Early functional editor prototype |
| Core language | C++20 |
| Desktop editor | Qt 6 Widgets application; Windows CI green on current head |
| Scene authoring | Rectangle, circle, triangle, and imported image sprites can be created; manual end-to-end validation still pending |
| Scene persistence | Versioned .vscene JSON with atomic writes and input validation; primitive shape and project-relative sprite asset references are serialized and regression-tested |
| Automated tests | Scene-document tests (including sprite path validation) pass in current Windows CI |
| Runtime integration | Not implemented |
| Browser SFS prototype | Implemented as a separate HTML Canvas 2D flight game; browser/device playtest pending |
| Export targets | Engine-produced targets not implemented |

**Reality statement (2026-11-03):** The repository contains a C++20/Qt 6 editor codebase, an editable 2D scene document and viewport, a Windows build workflow, and standalone browser prototypes. The current head (`5fd5f22`) produced a successful Windows Editor Build (compile + CTest + packaging with Qt plugins and MSVC CRT DLLs). Manual smoke testing of the packaged editor is still required before treating the package as fully verified.

## Implemented in source

| Item | Evidence | Verification status |
|---|---|---|
| Qt desktop editor shell and dock panels | editor/src/editor_main_window.cpp | Source present; Windows CI green |
| Versioned scene document with stable object IDs | editor/src/scene_document.cpp | Source present; tests pass in CI |
| Atomic .vscene save and validated load | editor/src/scene_document.cpp | Source present; tests pass in CI |
| 2D viewport with grid, object selection, and drag movement | editor/src/scene_viewport.cpp | Source present; manual runtime verification pending |
| Hierarchy rename and selection synchronization | editor/src/editor_main_window.cpp | Source present; manual runtime verification pending |
| Common scene shortcuts | editor/src/editor_main_window.cpp | Wired; CI green |
| Inspector for position, rotation, scale, width, and height | editor/src/editor_main_window.cpp | Source present; manual runtime verification pending |
| Primitive custom objects | editor/src/scene_document.h, editor/src/scene_viewport.cpp | Implemented; CI green |
| Sprite import and rendering | editor/src/editor_main_window.cpp, editor/src/scene_viewport.cpp | Implemented with path validation; CI green |
| 2D Space Flight Simulator browser prototype | website/sfs/ | Standalone browser prototype; playtest pending |
| Scene test projects and acceptance criteria | docs/testing/2d-engine-test-projects.md | Documented |

## Verified results

- Current head (`5fd5f22` — “Improve Windows test diagnostics and package notes”) has a successful Windows Editor Build run.
- CTest (scene document tests, including sprite asset path validation) passed.
- Packaging step confirmed Qt platform plugin and required MSVC runtime DLLs are present in the artifact.
- Earlier packaging failures related to missing CRT DLLs have been addressed in the current workflow.

## In progress / still required

| Item | Next proof required |
|---|---|
| Manual editor smoke test | Launch packaged app; create, transform, save, close, reopen, and inspect a scene including sprites |
| Runtime integration | Load an editor-authored .vscene in a separate game runtime |
| Project system | Create/open Veyra project, startup scene, run from editor |

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

1. Perform and record the manual smoke test of the packaged Windows editor.
2. Only after manual verification, consider publishing the next alpha package.
3. Do not start runtime work until Phase 1 exit criteria are fully met.
4. Keep progress claims tied to code and verifiable CI or runtime evidence.
