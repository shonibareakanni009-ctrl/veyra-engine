# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-09

This file records implemented work and its verification status. Plans and acceptance criteria remain in ROADMAP.md. Installer/updater design is in INSTALLER_UPDATER.md.

## Overall status

| Metric | Current state |
|---|---|
| Current phase | **Phase 2 — Runtime launch + close verified; bgfx renderer is next** |
| Project stage | Early functional editor prototype (Phase 1 verified) |
| Core language | C++20 selected |
| Desktop editor | Qt 6 Widgets; Windows CI green + manual smoke test passed |
| Scene authoring | Rectangle, circle, triangle, and imported sprites — verified |
| Scene persistence | Versioned .vscene with validation — verified |
| Automated tests | Editor scene tests and runtime `.vscene` loader tests pass in Windows CI |
| Manual smoke test | **Passed** (A–G) on 2026-11-03 |
| Native runtime | Qt-free SDL3 runtime; manual launch passed (window opens, blank as expected, Escape and close both work) |
| Installer | Inno Setup 7.1.0 Windows x64 artifact built; CI install/uninstall tests passed; manual clean-machine test pending |
| Updater | Manifest schema + Ed25519/SHA-256 trust design committed; updater executable not implemented |
| Automated release packaging | Windows x64 installer workflow added; final installer artifact and lifecycle test pending |
| Browser prototypes | Standalone (Adventure + SFS) |
| Export targets | Not implemented |

**Reality statement (2026-11-09):**  
Phase 1 verified. Phase 2 runtime foundation is in place. On 2026-11-09 Akanni manually launched the packaged runtime: SDL window opened, content was blank (expected before renderer), and both Escape and the window close button exited cleanly. Next work is bgfx integration and shape rendering.

## Active Phase 2A task order (locked 2026-11-09)

| Order | Task | Status |
|------:|------|--------|
| 1 | Manual runtime launch test | **Passed** (Akanni — open, blank, Escape + close) |
| 2 | Integrate bgfx renderer | **Next** |
| 3 | Render rectangles, circles and triangles | Pending |
| 4 | Connect Editor Run/Stop buttons | Pending |
| 5 | End-to-end testing | Pending |

## Verified results

- Shared engine directory refactor verified: [Windows Editor Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685837063) and [Windows Runtime Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685729965) both passed.
- Website About section build validation passed: [Website Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685592982).
- Installer CI passed: [Windows Installer Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41686760636).
- Manual editor smoke test completed and passed by Akanni on 2026-11-03.
- Runtime Windows CI passed on commit `b6bd682b82d222f0e6cacc0a5cba2ae81428ed4d`.
- Manual runtime launch verified by Akanni on 2026-11-09: window opens, blank content expected, Escape and close both exit cleanly.

## Phase 2 priorities (broader)

- [x] Record six-checkpoint Phase 2 execution plan.
- [x] Move native editor and runtime under `engine/`.
- [x] About Veyra homepage section.
- [x] Verify editor, runtime, and website workflows after path changes.
- [x] Inno Setup per-user installer + combined Windows packaging workflow.
- [x] Updater manifest schema and trust model.
- [x] Installer compile, silent install/uninstall, user-data preservation tests.
- [ ] Manually install on a clean Windows computer and launch the editor GUI.
- [x] Manual runtime launch test (Task 1 — passed 2026-11-09).
- [ ] Integrate bgfx renderer (Task 2).
- [ ] Render rectangles, circles and triangles (Task 3).
- [ ] Connect Editor Run/Stop buttons (Task 4).
- [ ] End-to-end testing (Task 5).

See [ROADMAP.md](ROADMAP.md) for milestone acceptance criteria and [INSTALLER_UPDATER.md](INSTALLER_UPDATER.md) for the distribution design.

## Team update

- Two new staff members joined the project on 2026-11-03 (details pending)

## Not implemented yet

- bgfx renderer integration and scene-object rendering
- Editor Run/Stop runtime integration
- Project creation/opening and project settings
- Verified user-facing Windows installer (prototype script and CI are in progress)
- Updater executable, signed update verification, download/staging, and rollback
- Automated tagged-release packaging
- Parent/child hierarchy and prefabs
- Physics, animation, audio, particles, scripting
- Code editor and Lua
- Export pipeline
- Android, Linux, engine-produced Web targets
