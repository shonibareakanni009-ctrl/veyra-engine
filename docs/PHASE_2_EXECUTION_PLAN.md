# Phase 2 — Execution Plan (six checkpoints)

**Status:** In progress; Checkpoint 1 verified by Windows CI and About section passes website CI.
**Scope:** Windows x64 first. Keep editor and runtime as sibling components under one `engine/` directory.

## Goals and constraints

- Place the native editor and runtime in one shared `engine/` directory while preserving separate executables, dependency boundaries, and independently testable targets.
- Preserve the existing version 1 `.vscene` document contract and backwards compatibility.
- Complete a rendered runtime scene and editor Run/Stop workflow.
- Deliver a Windows x64 installer and a secure updater with authenticated metadata, package integrity checks, staging, and rollback.
- Validate clean install, launch, update, recovery, and uninstall without deleting user projects.
- Fix the public website's missing About Veyra destination with a real section and working navigation.
- Do not claim a GUI, installer, update, or platform capability until it has been tested.

## Checkpoint 1 — Shared engine directory

**Status: VERIFIED (2026-11-03).** The editor and runtime source trees are under `engine/`, the root CMake project can select them independently, and both Windows x64 CI workflows passed after the move.

Target structure:

```text
engine/
  CMakeLists.txt
  editor/
    CMakeLists.txt
    src/
    tests/
  runtime/
    CMakeLists.txt
    src/
    tests/
    examples/
```

Actions:
1. Move the existing `editor/` and `runtime/` source trees under `engine/`.
2. Add a top-level engine CMake project that configures editor and runtime as separate targets/subprojects.
3. Update editor/runtime CI workflows, packaging paths, source references, and documentation.
4. Keep Qt exclusive to the editor; runtime remains Qt-free.
5. Run both Windows x64 build/test workflows and verify produced artifacts.

Acceptance: both applications build and their tests pass from the new paths; no source or test content is lost.

## Checkpoint 2 — Runtime renderer

1. Confirm a pinned bgfx integration strategy compatible with SDL3 and Windows x64.
2. Add a platform-window adapter and renderer lifecycle with resize and shutdown handling.
3. Render the supported scene primitives using the validated `.vscene` transform data.
4. Handle unsupported assets/shapes safely and provide actionable diagnostics.
5. Add deterministic tests where possible and document a manual desktop smoke test.

Acceptance: a Windows x64 runtime visibly renders an editor-authored scene and exits cleanly. A successful compile alone is not sufficient.

## Checkpoint 3 — Editor Run/Stop

1. Add runtime discovery/configuration.
2. Save or require a saved scene before launching.
3. Launch the runtime with the active scene path using safe argument handling.
4. Capture process output and exit status.
5. Stop only the runtime process started by this editor instance.
6. Surface missing-runtime and launch errors without crashing.

Acceptance: manual test creates/saves a scene, runs it from the editor, sees the runtime render it, and stops it from the editor.

## Checkpoint 4 — Windows installer

**Status: AUTOMATED PROTOTYPE VERIFIED.** Inno Setup 7.1.0 per-user installer artifact built successfully. CI passed silent install, installed-file checks, runtime CLI smoke test, silent uninstall, and external user-data preservation. Manual clean-machine editor GUI launch, repair, and upgrade tests remain.

1. Finalize installer technology after validating the Inno Setup prototype against upgrade/uninstall, signing, automation, and maintenance needs.
2. Define per-user/per-machine behavior and exact installation/data locations.
3. Package editor, runtime, Qt deployment files, and required runtime dependencies.
4. Keep projects, assets, settings, and logs outside replaceable application files.
5. Add clean install, repair/reinstall, upgrade, and uninstall tests.

Acceptance: clean Windows x64 installation launches both editor and runtime; uninstall preserves user projects by default.

## Checkpoint 5 — Updater and release pipeline

**Status: DESIGN BASELINE ADDED.** The JSON Schema and Ed25519/SHA-256 trust design are committed; updater code, signing keys, release hosting, and rollback are not implemented.

1. Define versioned release manifest and stable/beta/developer channels.
2. Authenticate manifest metadata and verify package digest/signature before trusting or applying packages.
3. Keep signing private keys out of source control and ordinary CI; use protected release credentials only if signing is configured.
4. Download and stage updates separately from the active installation.
5. Apply through a separate helper process after application shutdown.
6. Retain the previous version until post-update checks pass; implement rollback.
7. Automate versioned Windows x64 release packaging and publish release metadata only after required checks pass.

Acceptance: valid updates apply; invalid, altered, incompatible, or interrupted updates fail closed and leave the current installation usable.

## Checkpoint 6 — End-to-end verification and About page

**About section implementation: committed; website CI passed; Vercel production deployment reports READY. Manual browser click-through remains to be confirmed.**

### End-to-end verification
- Test clean install, first launch, scene creation, editor Run/Stop, update, restart, rollback, uninstall, and reinstall.
- Test invalid/unsigned manifest, wrong platform/channel, corrupted package, interrupted download, insufficient permissions, and failed replacement.
- Verify user projects and assets survive updates and remain after uninstall unless explicitly selected for deletion.
- Record environment, build/release IDs, test results, and known limitations.

### About Veyra website fix
- Add a real About Veyra section to the homepage with a stable `#about` anchor.
- Add an About navigation link that points to that section.
- Explain what Veyra is today, what is in development, and the longer-term direction without presenting roadmap items as completed features.
- Check responsive layout, internal navigation, and website CI/build.

Acceptance: the About link navigates to meaningful content and website validation passes.

## Completion gate

Phase 2 is complete only when all six checkpoints have evidence:
- Editor and runtime share the `engine/` directory and build independently.
- Runtime renders an editor-authored scene; editor Run/Stop works.
- Windows x64 installer installs and uninstalls safely.
- Authenticated, integrity-checked updates apply through a staged helper and can roll back.
- End-to-end tests pass and user projects are preserved.
- The About Veyra link works and website CI passes.

**Historical baseline at plan creation:** editor and runtime are still separate root-level directories; runtime scene rendering and editor Run/Stop are not implemented; user-facing installer/updater are not implemented; About Veyra navigation/content is missing. Existing runtime scene-loader Windows CI has passed, but manual SDL window launch remains unverified.
