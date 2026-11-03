# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-03

This file records implemented work and its verification status. Plans and acceptance criteria remain in ROADMAP.md. Installer/updater design is in INSTALLER_UPDATER.md.

## Overall status

| Metric | Current state |
|---|---|
| Current phase | **Phase 2 — Planned / implementation not yet verified** |
| Project stage | Early functional editor prototype (Phase 1 verified) |
| Core language | C++20 selected |
| Desktop editor | Qt 6 Widgets; Windows CI green + manual smoke test passed |
| Scene authoring | Rectangle, circle, triangle, and imported sprites — verified |
| Scene persistence | Versioned .vscene with validation — verified |
| Automated tests | Scene-document tests pass in Windows CI |
| Manual smoke test | **Passed** (A–G) on 2026-11-03 |
| Native runtime | Not implemented / not verified |
| Installer | Windows application-folder packaging exists; user-facing installer not implemented |
| Updater | Not implemented |
| Automated release packaging | Not implemented / not verified |
| Browser prototypes | Standalone (Adventure + SFS) |
| Export targets | Not implemented |

**Reality statement (2026-11-03):**  
Phase 1 has been fully verified. The packaged Windows editor successfully passed the complete manual smoke test (launch, primitives, transforms, sprite import, save/reopen, unsaved-change protection, and invalid path rejection). Phase 2 planning now covers the native runtime plus Windows installation, automated release packaging, secure updates, and recovery. These Phase 2 items are plans, not completed features.

## Verified results

- Windows CI green on head `5fd5f22`
- CTest (including sprite path validation) passed
- Editor application-folder packaging includes Qt plugins + MSVC CRT DLLs
- Manual smoke test completed and passed by owner on 2026-11-03
- Sections A–F: full pass
- Section G: pass (scene rejected on unsafe path with generic but safe error)

## Phase 2 priorities

1. Build the smallest useful native runtime vertical slice: load an editor-authored .vscene, create a window, render one object, process input, and shut down cleanly.
2. Integrate editor Run/Stop controls with that runtime.
3. In parallel, define the Windows installation layout and installer technology.
4. Automate versioned release packaging and define a signed/verified update manifest.
5. Implement staged updates with a separate helper process and rollback/recovery.
6. Verify installation, upgrade, update, recovery, and uninstall without damaging user projects.

See [ROADMAP.md](ROADMAP.md) for milestone acceptance criteria and [INSTALLER_UPDATER.md](INSTALLER_UPDATER.md) for the distribution design.

## Team update

- Two new staff members joined the project on 2026-11-03 (details pending)

## Not implemented yet

- Native engine runtime and renderer integration
- Editor Run/Stop runtime integration
- Project creation/opening and project settings
- User-facing Windows installer
- Updater, release manifest, signed update verification, and rollback
- Automated tagged-release packaging
- Parent/child hierarchy and prefabs
- Physics, animation, audio, particles, scripting
- Code editor and Lua
- Export pipeline
- Android, Linux, engine-produced Web targets
