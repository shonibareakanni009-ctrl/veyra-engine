# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-09

This tracker separates implementation, automated verification, and owner-side manual acceptance. A green build is not treated as a visual or end-to-end GUI test.

## Overall status

| Area | Current state |
|---|---|
| Shared engine layout | **Verified** — editor and runtime are separate subprojects under `engine/` |
| Editor | Qt 6 editor; Windows Editor Build passes; prior manual editor smoke test passed |
| Scene authoring | Rectangle, circle, triangle, and imported sprite authoring verified |
| Scene persistence | Versioned `.vscene` v1 loader/save validation in place |
| Runtime base | Qt-free SDL3 runtime; owner manually confirmed its window opens and Escape/window-close exit on 2026-11-09 |
| CPU scene geometry | Rectangle/circle/triangle mesh generation implemented; Windows Runtime Build #20 passed |
| bgfx renderer | Pinned dependency, shader compilation, SDL HWND hookup, Direct3D 11 renderer, and frame loop committed; latest renderer build is still awaiting CI completion |
| Editor Run/Stop | F5 / Shift+F5, QProcess argument-array launch, owned-process stop, and output dock implemented; Windows Editor Build passed |
| Installer | Inno Setup per-user prototype; earlier silent install/uninstall and external-data preservation tests passed; current package is being expanded to include updater |
| Updater | Local prototype validates manifest structure, Ed25519 signature, SHA-256/size, and stages verified local package; Windows updater build/security tests passed on clean source commit `866c9e0` and dependency-notice package run `42659501624`; the additional 2 GiB hashing cap is awaiting its own CI run |
| Release/update service | Not complete — no remote downloader, installer/apply helper, rollback, release endpoint, or release key policy |
| Website About section | Implemented; website CI passed and Vercel deployment previously reported READY; live click-through remains owner-side |

## Phase 2 work completed or implemented

- [x] Record Phase 2 execution plan and six checkpoints.
- [x] Move editor/runtime into shared `engine/` with independent CMake targets.
- [x] Owner manually launches packaged SDL runtime window (2026-11-09); blank is expected before renderer.
- [x] Add renderer-independent geometry module and tests.
- [x] Pass Windows Runtime Build #20: [run 42655582427](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42655582427) (geometry CMake/test checkpoint).
- [x] Add bgfx integration, shader inputs and SDL3 Win32 HWND renderer implementation; visual and latest renderer CI acceptance pending.
- [x] Add Editor Run/Stop actions and output capture; Windows Editor Build passed: [run 42657773169](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42657773169).
- [x] Add Windows updater project, strict local manifest validation, Ed25519 verification, SHA-256 package verification, and local verified staging.
- [x] Updater security CI passed after the stack-buffer fix and debug-output cleanup: [run 42659055214](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42659055214). The separate updater package including third-party license notices passed: [run 42659501624](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42659501624).
- [x] Include updater in installer staging and add post-install file/CLI checks; newest installer run is still pending.
- [x] Add and push the Phase 2 Completion Sprint plan: [PHASE_2_COMPLETION_SPRINT.md](PHASE_2_COMPLETION_SPRINT.md).

## Phase 2 acceptance still open

| Checkpoint | Remaining evidence |
|---|---|
| Runtime renderer | Latest bgfx-enabled Windows build/test must pass; owner must run packaged sample and confirm all three shapes render, resize, Escape and close |
| Editor Run/Stop | Owner must manually save a scene, run it from editor, see runtime window, then stop it from editor |
| Installer | Current installer build/package CI must pass; owner still needs clean-machine GUI install, repair/upgrade and launch checks |
| Updater | Local verifier/stager tests pass; remote download, safe archive/installer application, helper process, rollback and production update endpoint are not implemented |
| Release trust | No private release signing key, trusted production public key provisioning, key rotation, or protected release signing process. Release signing was deferred by the project owner |
| End-to-end | Install → edit/save → Run/Stop → update/rollback → uninstall/reinstall sequence not yet passed |
| Website | Live About navigation click-through still pending |

## Verified historical results

- Shared engine refactor: [Windows Editor Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685837063) and [Windows Runtime Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685729965).
- Installer prototype baseline: [Windows Installer Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41686760636); silent install/uninstall and outside-user-data marker preservation passed.
- Website About section build: [run 41685592982](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41685592982). The production deployment was previously reported READY; no live click-through evidence is recorded here.
- Manual editor smoke test passed on 2026-11-03.
- Manual runtime launch passed on 2026-11-09: window opened (blank before renderer), and both Escape and the window close button exited cleanly.
- Geometry CI passed in Runtime Build #20.
- Editor Run/Stop Windows compile/tests passed in run 42657773169.
- Updater local security tests and packaging passed in runs 42659055214 and 42659501624; a further package-size bound was committed after those passes and is pending CI.

## Not implemented / outside current Phase 2 scope

- Production updater HTTP download, archive extraction, installer/apply helper, transactional version switching, rollback, release hosting and key rotation.
- Sprite texture rendering in the native runtime.
- Project creation/opening/settings, export pipeline, physics, animation, audio, particles, scripting and Lua.
- Linux, Android and engine-produced Web targets.

**Phase 2 is not marked complete yet.** The remaining gates are explicitly listed above; code being present does not substitute for a required CI or manual acceptance result.
