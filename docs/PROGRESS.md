# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-03

This file records implemented work and its verification status. Plans and acceptance criteria remain in ROADMAP.md. Installer/updater design is in INSTALLER_UPDATER.md.

## Overall status

| Metric | Current state |
|---|---|
| Current phase | **Phase 2 — Runtime foundation started; Checkpoint 1 automated CI passed, manual launch pending** |
| Project stage | Early functional editor prototype (Phase 1 verified) |
| Core language | C++20 selected |
| Desktop editor | Qt 6 Widgets; Windows CI green + manual smoke test passed |
| Scene authoring | Rectangle, circle, triangle, and imported sprites — verified |
| Scene persistence | Versioned .vscene with validation — verified |
| Automated tests | Editor scene tests and runtime `.vscene` loader tests pass in Windows CI |
| Manual smoke test | **Passed** (A–G) on 2026-11-03 |
| Native runtime | Separate Qt-free SDL3 runtime builds in Windows CI; scene-loader tests pass; manual window launch pending |
| Installer | Windows application-folder packaging exists; user-facing installer not implemented |
| Updater | Not implemented |
| Automated release packaging | Not implemented / not verified |
| Browser prototypes | Standalone (Adventure + SFS) |
| Export targets | Not implemented |

**Reality statement (2026-11-03):**  
Phase 1 has been fully verified. The packaged Windows editor successfully passed the complete manual smoke test (launch, primitives, transforms, sprite import, save/reopen, unsaved-change protection, and invalid path rejection). Phase 2 runtime implementation has started: the separate Qt-free runtime target, SDL3 lifecycle code, `.vscene` loader, tests, and Windows package workflow are in place. Windows CI passed on 2026-11-03. The SDL window has not yet been manually launched, scene objects are not rendered, and editor Run/Stop is not integrated. Windows installer/updater, signed updates, and recovery remain planned.

## Verified results

- Windows CI green on head `5fd5f22`
- CTest (including sprite path validation) passed
- Editor application-folder packaging includes Qt plugins + MSVC CRT DLLs
- Manual editor smoke test completed and passed by owner on 2026-11-03
- Runtime Windows CI passed on commit `b6bd682b82d222f0e6cacc0a5cba2ae81428ed4d`: CMake configure, runtime + tests compile, CTest scene validation, runtime package staging, and artifact upload
- Runtime package artifact: [veyra-runtime-windows-x64](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41682363212)
- Sections A–F: full pass
- Section G: pass (scene rejected on unsafe path with generic but safe error)

## Phase 2 priorities

1. Owner-run the packaged runtime on Windows: open `first-scene.vscene`, confirm the SDL window and object count, and verify Escape/window-close shutdown.
2. Integrate bgfx with SDL and render the first editor-authored scene object.
3. Integrate editor Run/Stop controls with the runtime.
4. In parallel, define the Windows installation layout and installer technology.
5. Automate versioned release packaging and define a signed/verified update manifest.
6. Implement staged updates with a separate helper process and rollback/recovery, then verify install/upgrade/update/uninstall without damaging user projects.

See [ROADMAP.md](ROADMAP.md) for milestone acceptance criteria and [INSTALLER_UPDATER.md](INSTALLER_UPDATER.md) for the distribution design.

## Team update

- Two new staff members joined the project on 2026-11-03 (details pending)

## Not implemented yet

- bgfx renderer integration and scene-object rendering
- Manual SDL window launch verification
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
