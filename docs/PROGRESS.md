# Veyra Engine — Progress Tracker

**Last updated:** 2026-11-20

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
| bgfx renderer | Pinned bgfx dependency, embedded DXBC shaders, SDL3 Win32 swap-chain hookup, Direct3D 11 renderer and frame loop committed; first renderer compile exposed bgfx API mismatches, corrected on 2026-11-20; corrected Windows runtime and installer builds passed |
| Editor Run/Stop | F5 / Shift+F5, QProcess argument-array launch, owned-process stop, and output dock implemented; Windows Editor Build passed |
| Installer | Inno Setup per-user prototype; earlier silent install/uninstall and external-data preservation tests passed; updater included; latest Windows installer package and silent install/uninstall smoke tests passed on 2026-11-20 |
| Updater | Local prototype validates manifest structure, Ed25519 signature, SHA-256/size, and stages verified local package; Windows updater build/security tests passed on clean source commit `866c9e0` and dependency-notice package run `42659501624`; the additional 2 GiB hashing cap is awaiting its own CI run |
| Release/update service | Not complete — no remote downloader, installer/apply helper, rollback, release endpoint, or release key policy |
| Website About section | Implemented; website CI passed and Vercel deployment previously reported READY; live click-through remains owner-side |

## Phase 2 work completed or implemented

- [x] Record Phase 2 execution plan and six checkpoints.
- [x] Move editor/runtime into shared `engine/` with independent CMake targets.
- [x] Owner manually launches packaged SDL runtime window (2026-11-09); blank is expected before renderer.
- [x] Add renderer-independent geometry module and tests.
- [x] Pass Windows Runtime Build #20: [run 42655582427](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42655582427) (geometry CMake/test checkpoint).
- [x] Add bgfx integration, shader inputs and SDL3 Win32 HWND renderer implementation; visual and renderer Windows build and tests passed on 2026-11-20; owner visual acceptance pending.
- [x] Add Editor Run/Stop actions and output capture; Windows Editor Build passed: [run 42657773169](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42657773169).
- [x] Add Windows updater project, strict local manifest validation, Ed25519 verification, SHA-256 package verification, and local verified staging.
- [x] Updater security CI passed after the stack-buffer fix and debug-output cleanup: [run 42659055214](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42659055214). The separate updater package including third-party license notices passed: [run 42659501624](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42659501624).
- [x] Include updater in installer staging and add post-install file/CLI checks; latest installer run passed.
- [x] Add and push the Phase 2 Completion Sprint plan: [PHASE_2_COMPLETION_SPRINT.md](PHASE_2_COMPLETION_SPRINT.md).

## Phase 2 acceptance still open

| Checkpoint | Remaining evidence |
|---|---|
| Runtime renderer | Windows Runtime Build passed (run 44698749487); owner must run packaged sample and confirm all three shapes render, resize, Escape and close |
| Editor Run/Stop | Owner must manually save a scene, run it from editor, see runtime window, then stop it from editor |
| Installer | Windows Installer Build passed (run 44698749510), including updater build/tests and silent install/uninstall smoke tests; owner still needs clean-machine GUI install, repair/upgrade and launch checks |
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


## 2026-11-20 renderer correction update

- Investigated failing renderer Windows builds and extracted the exact MSVC diagnostics. The errors were API mismatches against the pinned bgfx submodule: `PlatformData::nwh` and `Init::resolution` do not exist in this revision, `bgfx::reset` has a flags + optional `SwapChain*` signature, and the scene vertex member is `rgba`.
- Verified the exact upstream header from bgfx submodule commit `7346c3e731bd65f35c7e6a99819840e554f5b748` (bgfx.cmake remains pinned at `eb88f6be55ae223966698c802542c24b0d93fd25`).
- Corrected `engine/runtime/src/runtime_renderer.cpp` to initialize and resize through `bgfx::Init::swapChain` / `bgfx::reset(flags, &swapChain)` and to upload `GeometryVertex::rgba`.
- Code commit: `4d4c3ea47cca7825b2a468e4d2745755aa411e30`.
- Documentation plan and implementation record commits: `6422e638dd973a18c40f4f196ece37f6a6fea012`, `14ae0caf45a151eb52563a7980cc1ba4d76d65ce`.
- Corrected CI passed: [Windows Runtime Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/44698749487) (configure, compile, scene tests, package upload) and [Windows Installer Build](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/44698749510) (editor/runtime/updater builds, all engine tests, Inno Setup compile, silent install/smoke-test/uninstall, artifact upload).
- Artifacts are available: runtime package `veyra-runtime-windows-x64` (2,449,011 bytes) and installer `veyra-engine-installer-windows-x64` (41,949,148 bytes).
- Editor Run/Stop implementation is present in `engine/editor/src/editor_main_window.cpp`; owner-side F5/Shift+F5 acceptance remains open.
- The local updater remains a verifier/stager prototype, not a production remote updater. Code signing was deferred by the project owner; no production release signing key or endpoint is configured.

## 2026-11-28 editor direct manipulation update

- Added eight screen-space selection handles around the selected object's transformed bounds in the editor viewport.
- Dragging inside an object continues to move it; dragging an edge/corner handle changes the authored width/height, clamps dimensions to at least 10 world units, and shifts the object center so the opposite edge remains anchored.
- Resize pointer deltas are evaluated against the initial object transform so rotated and non-uniformly scaled objects do not drift as the object is resized.
- Added resize/move cursor feedback and updated the viewport hint text.
- Moved the Inspector dock from the right side to the top dock area below the menu/toolbar region. Its transform controls now use a compact horizontal arrangement; the dock remains movable and floatable.
- Plan: [EDITOR_DIRECT_MANIPULATION_PLAN.md](EDITOR_DIRECT_MANIPULATION_PLAN.md).
- Implementation commits: `fef04fb77ff9789d970bf6e0ecb26a942b72ec18`, `bb373c456249275e2f14f376a197c83c17cf8fd6`, `c79692c659b21cc99881faf356093483838edc26`, `9ea690e9e461c41f39e60ce34289ab94de936d54`, `da6900f216b2a7019329e0b525688e83a916bde7`.
- Windows Editor Build: [run 46136904061](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/46136904061) — queued/in progress at the time of this update.
- Windows Installer Build: [run 46136904172](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/46136904172) — pending at the time of this update.
- Owner-side visual acceptance is still required after a successful build. Verify move, all eight resize handles, Inspector live values, and the Inspector's top placement in the packaged editor.

