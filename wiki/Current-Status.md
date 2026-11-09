# Current Status

**Last updated:** 2026-11-09

## Phase overview

| Phase | Status |
|-------|--------|
| Phase 0 — Technical foundation | Complete |
| Phase 1 — 2D editor + scene authoring | **Verified** |
| Phase 2 — Runtime, install, update | **In progress** |
| Phase 3 — Core 2D systems | Not started |
| Phase 4 — Test games + export | Not started |
| Later — 3D, FBX, networking, etc. | Planned |

## Active Phase 2A task order

| Order | Task | Status |
|------:|------|--------|
| 1 | Manual runtime launch test | **Passed** (open, blank, Escape + close) |
| 2 | Integrate bgfx renderer | **Next** |
| 3 | Render rectangles, circles and triangles | Pending |
| 4 | Connect Editor Run/Stop buttons | Pending |
| 5 | End-to-end testing | Pending |

## Working today

- C++20 + CMake project layout under `engine/`
- Qt 6 desktop editor with scene authoring
- `.vscene` save/load with validation
- SDL3 runtime lifecycle + scene loader
- Windows CI for editor, runtime, installer, website
- Installer prototype (Inno Setup) + CI install/uninstall checks

## Not working yet

- Any on-screen rendering in the runtime
- Editor Run/Stop integration
- Physics, audio, animation, scripting
- Export pipeline
- 3D, FBX, multiplayer

For the full tracker see `docs/PROGRESS.md` in the repository.
