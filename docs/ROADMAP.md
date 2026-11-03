# Veyra Engine — Roadmap

**Last updated:** 2026-11-03  
**Status:** Active development; one vertical slice at a time.

## Guiding rule

Do not move to the next milestone until the current milestone has working code, automated checks where practical, and documented limitations. A successful compile alone does not prove runtime behavior.

## Phase 0 — Technical foundation

**Status: Completed for the current editor prototype.**

| Item | Result |
|---|---|
| Core language | C++20 |
| Build system | CMake |
| Desktop editor framework | Qt 6 Widgets |
| Scene-file baseline | Versioned JSON .vscene document |
| Development workflow | GitHub Actions Windows build and test pipeline |

SDL3 and bgfx remain part of the longer-term runtime direction; they are not yet integrated into this editor authoring prototype.

## Phase 1 — Functional 2D editor and scene authoring (current)

| ID | Work item | Status | Acceptance evidence |
|---|---|---|---|
| 1.1 | Qt desktop editor shell and dockable panels | Implemented | Windows compile succeeded; packaged runtime pending |
| 1.2 | Scene document with stable object IDs | Implemented | Source plus automated tests; CI pending |
| 1.3 | Create, select, move, rename, and delete scene objects | Implemented in editor source | Manual packaged-app test pending |
| 1.4 | Position, rotation, scale, width, and height inspector | Implemented in editor source | Manual packaged-app test pending |
| 1.5 | Save/load .vscene with validation and atomic writes | Implemented in editor source | Automated tests added; CI pending |
| 1.6 | Protect unsaved edits on new/open/close | Implemented in editor source | Windows compile and manual UI check pending |
| 1.7 | Reliable Windows x64 application folder packaging | In progress | CI must stage Qt plugin and required MSVC DLLs |
| 1.8 | Manual editor smoke test | Not started | Launch package and verify edit/save/reopen |
| 1.9 | Runtime loads editor-authored scenes | Not started | Play a saved scene outside the editor |

### Phase 1 exit criteria

- Windows editor build and scene-document tests pass.
- Full application folder contains Qt plugins and required runtime libraries.
- Manual test confirms creating, editing, saving, closing, and reopening scenes.
- Scene format and current limitations are documented.
- No user project data is stored inside the engine installation folder.

## Phase 2 — Runtime foundation

Only after Phase 1 exit criteria are met:

- Runtime executable and game loop
- Platform lifecycle and input abstraction
- Load .vscene from a game project
- Camera and first runtime-rendered 2D objects
- Run/stop workflow from the editor

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
