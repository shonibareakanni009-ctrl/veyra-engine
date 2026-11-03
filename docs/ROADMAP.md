# Veyra Engine — Roadmap

**Last updated:** 2026-11-03  
**Status:** Phase 1 Verified. Phase 2 — Runtime, installation, and update foundation — is CURRENT.

## Guiding rule

Do not move to the next milestone until the current milestone has working code, automated checks where practical, and documented limitations. A successful compile alone does not prove runtime behavior. Plans are not implementation evidence; progress status must only change after verification.

## Phase 0 — Technical foundation

**Status: Completed**

| Item | Result |
|---|---|
| Core language | C++20 |
| Build system | CMake |
| Desktop editor framework | Qt 6 Widgets |
| Scene-file baseline | Versioned JSON .vscene document |
| Development workflow | GitHub Actions Windows build and test pipeline |

## Phase 1 — Functional 2D editor and scene authoring

**Status: VERIFIED (2026-11-03)**

| ID | Work item | Status | Acceptance evidence |
|---|---|---|---|
| 1.1 | Qt desktop editor shell and dockable panels | Verified | Windows CI + manual test |
| 1.2 | Scene document with stable object IDs | Verified | Tests + manual test |
| 1.3 | Create, select, move, rename, and delete scene objects | Verified | Manual smoke test |
| 1.4 | Position, rotation, scale, width, and height inspector | Verified | Manual smoke test |
| 1.5 | Save/load .vscene with validation and atomic writes | Verified | CI + manual test |
| 1.6 | Protect unsaved edits on new/open/close | Verified | Manual smoke test |
| 1.7 | Reliable Windows x64 application folder packaging | Verified | Green CI packaging |
| 1.8 | Manual editor smoke test | **Passed** | Akanni executed 2026-11-03 |
| 1.9 | Runtime loads editor-authored scenes | Moved to Phase 2 | Not started |

### Phase 1 exit criteria — Met

- Windows editor build and scene-document tests pass.
- Full application folder contains Qt plugins and required runtime libraries.
- Manual test confirms creating, editing, saving, closing, and reopening scenes.
- Invalid asset paths are rejected.
- Scene format and current limitations are documented.

## Phase 2 — Runtime, installation, and update foundation (CURRENT)

Phase 2 delivers the first native run path and establishes a dependable way to install and maintain Veyra. Runtime work is the critical path; installer/updater architecture and automation are planned in parallel. The first supported distribution target is Windows x64. Other targets must be added only after their build and packaging paths are validated.

### 2A — Runtime vertical slice

| ID | Work item | Acceptance evidence |
|---|---|---|
| 2A.1 | Add a separate runtime executable/target | Runtime builds independently of the Qt editor |
| 2A.2 | Implement SDL3 window and application lifecycle | Window opens, handles close/events, and shuts down cleanly |
| 2A.3 | Load a project and editor-authored .vscene | Valid scene loads; invalid input produces actionable errors |
| 2A.4 | Initialize the selected rendering backend and render a first object | Known test scene renders correctly on the validated desktop target |
| 2A.5 | Add basic input handling | Keyboard input is processed through the platform abstraction |
| 2A.6 | Add editor Run/Stop workflow | Editor launches the project runtime, reports launch failure, and can stop it safely |

**First runtime vertical slice:** Open project → Load .vscene → Create window → Render one object → Process input → Close cleanly.

### 2B — Installer foundation

| ID | Work item | Acceptance evidence |
|---|---|---|
| 2B.1 | Define installation layout and version identity | Documented locations for editor, runtime, updater, shared dependencies, and user data |
| 2B.2 | Evaluate and select Windows installer technology | Decision recorded with licensing, maintenance, upgrade, and uninstall trade-offs |
| 2B.3 | Build a Windows x64 installer | Clean-machine installation launches editor and runtime |
| 2B.4 | Implement install/upgrade/uninstall behavior | Reinstall and upgrade are repeatable; uninstall does not silently delete user projects |
| 2B.5 | Separate application files from user data | Engine updates do not overwrite projects, settings, or user assets |

### 2C — Updater and release foundation

| ID | Work item | Acceptance evidence |
|---|---|---|
| 2C.1 | Define version manifest and release channels | Stable/beta/developer channel metadata has a documented schema |
| 2C.2 | Automate release packaging with GitHub Actions | Tagged release produces versioned Windows x64 packages and release notes |
| 2C.3 | Verify update metadata and package integrity | Invalid, altered, or untrusted packages are rejected before installation |
| 2C.4 | Implement update check, download, and staging | Updater detects compatible updates and stages them without replacing live files |
| 2C.5 | Apply updates through a separate helper process | Application closes cleanly before files are replaced |
| 2C.6 | Implement failure recovery/rollback | Interrupted or failed update restores the previous working version |
| 2C.7 | Add user-visible update controls | User can check, review, defer, and apply updates; release channel is visible |

### 2D — End-to-end verification

- Test clean install, first launch, update, restart, uninstall, and reinstall on a clean Windows x64 environment.
- Test upgrade from at least one prior released version.
- Test corrupted package, invalid manifest, interrupted download, insufficient permissions, and failed replacement.
- Verify projects, assets, and user settings survive upgrades and remain after uninstall unless the user explicitly chooses to remove them.
- Record test environment, package version, results, and known limitations.

### Phase 2 exit criteria

- The runtime loads and renders a scene created by the editor.
- The editor can launch and stop the runtime.
- Windows x64 installation and upgrade are repeatable.
- Update packages are authenticated/verified before application.
- A failed update can recover without corrupting the previous installation.
- Automated release packaging and documented end-to-end tests pass.
- No platform is described as supported until its own build, install, launch, and update path is tested.

## Phase 3 — Core 2D systems

- Texture and sprite asset import improvements
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
- Project templates and broader platform installers/updaters
- Signed public releases and release-channel maturity beyond the initial Windows x64 implementation

## Timeline honesty

No release dates are promised until the build, packaging, installation, update, and recovery processes are reliable and repeatable. A planned milestone is not complete until its acceptance evidence exists.
