# Roadmap (summary)

## Phase 0 — Technical foundation
**Status: Complete**  
C++20, CMake, Qt editor shell, `.vscene` baseline, Windows CI.

## Phase 1 — Functional 2D editor
**Status: Verified (2026-11-03)**  
Create/edit/save/reopen scenes; packaging; manual smoke test passed.

## Phase 2 — Runtime, installation, update (CURRENT)

| Part | Focus |
|------|--------|
| 2A | Runtime vertical slice (window, load, render, Run/Stop) |
| 2B | Installer foundation |
| 2C | Updater and release foundation |
| 2D | End-to-end verification |

## Phase 3 — Core 2D systems
Assets, layering, animation, physics, audio, UI/input, Lua, save/debug tools.

## Phase 4 — Test games and export
Engine-built test games (including Veyra Draw Bridge), Windows/Linux exports, then Android and Web.

## Later phases
- Multiplayer
- 3D renderer and workflows
- Interoperability (FBX, glTF, easy tool switching)
- Plugins, templates, signed public releases

Full detail: `docs/ROADMAP.md`
