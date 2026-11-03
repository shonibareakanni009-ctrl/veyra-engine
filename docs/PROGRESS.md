# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-03

This file records implemented work and its verification status. Plans and acceptance criteria remain in ROADMAP.md. Installer/updater design is in INSTALLER_UPDATER.md.

## Overall status

| Metric | Current state |
|---|---|
| Current phase | **Phase 2 — Shared engine layout verified by Windows editor/runtime CI; renderer next** |
| Project stage | Early functional editor prototype (Phase 1 verified) |
| Core language | C++20 selected |
| Desktop editor | Qt 6 Widgets; Windows CI green + manual smoke test passed |
| Scene authoring | Rectangle, circle, triangle, and imported sprites — verified |
| Scene persistence | Versioned .vscene with validation — verified |
| Automated tests | Editor scene tests and runtime `.vscene` loader tests pass in Windows CI |
| Manual smoke test | **Passed** (A–G) on 2026-11-03 |
| Native runtime | Qt-free SDL3 runtime under `engine/runtime/`; Windows configure/build/tests/package CI passed after move; manual window launch pending |
| Installer | Inno Setup 7.1.0 Windows x64 artifact built; CI install/uninstall and external-data preservation tests passed; manual clean-machine test pending |
| Updater | Manifest schema + Ed25519/SHA-256 trust design committed; updater executable not implemented |
| Automated release packaging | Windows x64 installer workflow added; final installer artifact and lifecycle test pending |
| Browser prototypes | Standalone (Adventure + SFS) |
| Export targets | Not implemented |

**Reality statement (2026-11-03):**  
Phase 1 has been fully verified. The packaged Windows editor successfully passed the complete manual smoke test (launch, primitives, transforms, sprite import, save/reopen, unsaved-change protection, and invalid path rejection). Phase 2 runtime implementation has started: the separate Qt-free runtime target, SDL3 lifecycle code, `.vscene` loader, tests, and Windows package workflow are in place. Windows CI passed on 2026-11-03. The SDL window has not yet been manually launched, scene objects are not rendered, and editor Run/Stop is not integrated. An Inno Setup per-user installer script and combined Windows packaging workflow are now in the repository; the installer build and CI install/uninstall smoke test passed. A manual clean-machine install and editor GUI launch remain pending. The updater manifest schema and trust model are documented, but signature verification, downloads, staged application, and rollback are not implemented. The editor and runtime now share the `engine/` directory and both independent Windows x64 CI workflows passed after the move. The website has a real About Veyra section at `/#about`; website CI passed and Vercel reports the latest production deployment as READY. A manual browser click-through has not been performed here.

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

## Phase 2 priorities

- [x] Record six-checkpoint Phase 2 execution plan in `docs/PHASE_2_EXECUTION_PLAN.md`.
- [x] Move native editor and runtime source trees under `engine/` and add selective top-level CMake entry point.
- [x] Add About Veyra homepage section and `#about` navigation destination.
- [x] Verify editor, runtime, and website workflows after the path/navigation changes.
- [x] Add an Inno Setup per-user installer definition and combined Windows x64 packaging workflow.
- [x] Define the updater manifest schema and cryptographic trust model.
- [x] Pass installer compile, silent install/uninstall, and user-data preservation tests.
- [ ] Manually install on a clean Windows computer and launch the editor GUI.


1. Akanni runs the packaged runtime on Windows: open `first-scene.vscene`, confirm the SDL window and object count, and verify Escape/window-close shutdown.
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
- Verified user-facing Windows installer (prototype script and CI are in progress)
- Updater executable, signed update verification, download/staging, and rollback
- Automated tagged-release packaging
- Parent/child hierarchy and prefabs
- Physics, animation, audio, particles, scripting
- Code editor and Lua
- Export pipeline
- Android, Linux, engine-produced Web targets
