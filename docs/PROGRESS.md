# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-09

This file records implemented work and its verification status. Plans and acceptance criteria remain in ROADMAP.md. Installer/updater design is in INSTALLER_UPDATER.md.

## Overall status

| Metric | Current state |
|---|---|
| Current phase | **Phase 2 — Runtime launch manually confirmed; bgfx renderer is next** |
| Project stage | Early functional editor prototype (Phase 1 verified) |
| Core language | C++20 selected |
| Desktop editor | Qt 6 Widgets; Windows CI green + manual smoke test passed |
| Scene authoring | Rectangle, circle, triangle, and imported sprites — verified |
| Scene persistence | Versioned .vscene with validation — verified |
| Automated tests | Editor scene tests and runtime `.vscene` loader tests pass in Windows CI |
| Manual smoke test | **Passed** (A–G) on 2026-11-03 |
| Native runtime | Qt-free SDL3 runtime under `engine/runtime/`; Windows configure/build/tests/package CI passed after move; manual window launch passed (owner-reported; blank window expected) |
| Installer | Inno Setup 7.1.0 Windows x64 artifact built; CI install/uninstall and external-data preservation tests passed; manual clean-machine test pending |
| Updater | Manifest schema + Ed25519/SHA-256 trust design committed; updater executable not implemented |
| Automated release packaging | Windows x64 installer workflow added; final installer artifact and lifecycle test pending |
| Browser prototypes | Standalone (Adventure + SFS) |
| Export targets | Not implemented |

**Reality statement (2026-11-09):**  
Phase 1 has been fully verified. The packaged Windows editor successfully passed the complete manual smoke test. Phase 2 runtime foundation is in place (SDL3 lifecycle, scene loader, CI packaging). The packaged runtime was manually launched by the project owner on 2026-11-09: the SDL window opened successfully and was blank, as expected before renderer implementation. The next ordered work is bgfx integration and shape rendering, then Editor Run/Stop, then end-to-end testing.

## Active Phase 2A task order (locked 2026-11-09)

| Order | Task | Status |
|------:|------|--------|
| 1 | Manual runtime launch test | **Complete — owner-reported; blank window expected** |
| 2 | Integrate bgfx renderer | **Next — plan committed in [PHASE_2B_BGFX_RENDERER_PLAN.md](PHASE_2B_BGFX_RENDERER_PLAN.md)** |
| 3 | Render rectangles, circles and triangles | Pending |
| 4 | Connect Editor Run/Stop buttons | Pending |
| 5 | End-to-end testing | Pending |

## Verified results

- Shared engine directory refactor verified: [Windows Editor Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685837063) and [Windows Runtime Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685729965) both passed configure/build/tests/package.
- Website About section build validation passed: [Website Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685592982). Vercel production deployment for commit `13650cfa706908d8a0cae848a967792d84f50d61` reported READY; live browser click-through remains unverified.
- Artifacts from the refactor validation: `veyra-editor-windows-x64-folder` and `veyra-runtime-windows-x64`.
- Installer CI passed: [Windows Installer Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41686760636). It compiled the Windows x64 setup, silently installed it, checked editor/runtime/sample files, ran runtime `--help`, silently uninstalled it, and confirmed an external user-project marker remained. Artifact: `veyra-engine-installer-windows-x64` (41,498,089 bytes; retained until 2026-11-17).

- Windows CI green on head `5fd5f22`
- CTest (including sprite path validation) passed
- Editor application-folder packaging includes Qt plugins + MSVC CRT DLLs
- Manual editor smoke test completed and passed by Akanni on 2026-11-03
- Runtime Windows CI passed on commit `b6bd682b82d222f0e6cacc0a5cba2ae81428ed4d`: CMake configure, runtime + tests compile, CTest scene validation, runtime package staging, and artifact upload
- Runtime package artifact: [veyra-runtime-windows-x64](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41682363212)
- Sections A–F: full pass
- Section G: pass (scene rejected on unsafe path with generic but safe error)

## Phase 2 priorities (broader)

- [x] Record six-checkpoint Phase 2 execution plan in `docs/PHASE_2_EXECUTION_PLAN.md`.
- [x] Move native editor and runtime source trees under `engine/` and add selective top-level CMake entry point.
- [x] Add About Veyra homepage section and `#about` navigation destination.
- [x] Verify editor, runtime, and website workflows after the path/navigation changes.
- [x] Add an Inno Setup per-user installer definition and combined Windows x64 packaging workflow.
- [x] Define the updater manifest schema and cryptographic trust model.
- [x] Pass installer compile, silent install/uninstall, and user-data preservation tests.
- [ ] Manually install on a clean Windows computer and launch the editor GUI.
- [x] Manual runtime launch test (owner-reported: SDL window opened; blank before rendering, 2026-11-09).
- [ ] Integrate bgfx renderer (Task 2).
- [ ] Render rectangles, circles and triangles (Task 3).
- [ ] Connect Editor Run/Stop buttons (Task 4).
- [ ] End-to-end testing (Task 5).

See [ROADMAP.md](ROADMAP.md) for milestone acceptance criteria and [INSTALLER_UPDATER.md](INSTALLER_UPDATER.md) for the distribution design.

## Team update

- Two new staff members joined the project on 2026-11-03 (details pending)

## Not implemented yet

- bgfx renderer integration and scene-object rendering
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
