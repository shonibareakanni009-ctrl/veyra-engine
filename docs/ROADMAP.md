# Veyra Engine — Roadmap

**Last updated:** 2026-11-03  
**Status:** Phase 1 Verified. Active development continuing into Phase 2.

## Guiding rule

Do not move to the next milestone until the current milestone has working code, automated checks where practical, and documented limitations. A successful compile alone does not prove runtime behavior.

## Phase 0 — Technical foundation

**Status: Completed**

| Item | Result |
|---|---|
| Core language | C++20 |
| Build system | CMake |
| Desktop editor framework | Qt 6 Widgets |
| Scene-file baseline | Versioned JSON .vscene document |
| Development workflow | GitHub Actions Windows build and test pipeline |

## Phase 1 — Functional 2D editor and scene authoring

**Status: VERIFIED (2026-11-03)**

| ID | Work item | Status | Acceptance evidence |
|---|---|---|---|
| 1.1 | Qt desktop editor shell and dockable panels | Verified | Windows CI + manual test |
| 1.2 | Scene document with stable object IDs | Verified | Tests + manual test |
| 1.3 | Create, select, move, rename, and delete scene objects | Verified | Manual smoke test |
| 1.4 | Position, rotation, scale, width, and height inspector | Verified | Manual smoke test |
| 1.5 | Save/load .vscene with validation and atomic writes | Verified | CI + manual test |
| 1.6 | Protect unsaved edits on new/open/close | Verified | Manual smoke test |
| 1.7 | Reliable Windows x64 application folder packaging | Verified | Green CI packaging |
| 1.8 | Manual editor smoke test | **Passed** | Owner executed 2026-11-03 |
| 1.9 | Runtime loads editor-authored scenes | Not started | Moved to Phase 2 |

### Phase 1 exit criteria — Met

- Windows editor build and scene-document tests pass.
- Full application folder contains Qt plugins and required runtime libraries.
- Manual test confirms creating, editing, saving, closing, and reopening scenes.
- Invalid asset paths are rejected.
- Scene format and current limitations are documented.

## Phase 2 — Runtime foundation (CURRENT)

Only after Phase 1 exit criteria are met (now complete):

- Runtime executable and game loop
- Platform lifecycle and input abstraction
- Load .vscene from a game project
- Camera and first runtime-rendered 2D objects
- Run/stop workflow from the editor

**Recommended first vertical slice:**
Open project → Load .vscene → Create window → Render one object → Process input → Close cleanly.

## Phase 3 — Core 2D systems

- Texture and sprite asset import
- Layering and animation
- Physics and collision
- Audio
- UI and input actions
- Lua scripting integration
- Save-game system and debugging tools

## Phase 4 — Test-game coverage and export

- Veyra Adventure browser baseline (existing standalone game; not yet engine-built)
- Space Flight Simulator
- Top-Down Adventure, Puzzle Game, and Racing Game
- Windows and Linux exports, then Android and engine-produced Web builds

## Later phases

- Multiplayer and networking
- 3D renderer and editor workflows
- Plugin ecosystem and expanded scripting options
- Project templates, installer, updater, and signed releases

## Timeline honesty

No release dates are promised until the build, packaging, and test process is reliable and repeatable.
