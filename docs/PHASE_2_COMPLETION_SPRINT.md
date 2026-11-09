# Phase 2 Completion Sprint — Execution Plan

**Status:** Plan committed before further implementation.  
**Branch:** `main`  
**Target:** Windows x64 first; preserve Qt-free runtime and version-1 `.vscene` compatibility.

This sprint consolidates the remaining Phase 2 work into a single ordered execution. It does not waive manual acceptance criteria.

## Verified starting point (2026-11-09)

- Shared `engine/` directory and independent editor/runtime builds are already verified.
- Editor scene authoring and version-1 scene persistence are verified.
- Packaged runtime has been manually opened by the owner; the blank SDL3 window is expected because drawing is not implemented.
- Renderer-independent CPU geometry for rectangle/circle/triangle is committed.
- Windows Runtime Build #20 passed on commit `9daebe0e2be866f06496e1abbfdc6c3e5ffebab4`, including compilation, CTest, and package staging.
- Inno Setup installer prototype and silent install/uninstall CI passed.
- Updater only has a schema and security design; no updater executable or configured release endpoint exists.
- About Veyra website section is implemented and website CI passed; live click-through is still an owner-side check.

## Work order

### A. Renderer and runtime (Checkpoint 2)
1. Keep the pinned bgfx.cmake commit and submodules immutable.
2. Add SDL3 native-window handle adapter for Windows; initialize bgfx after window creation.
3. Compile and embed the minimal vertex/fragment shaders with the pinned shaderc tool.
4. Upload transformed CPU geometry, submit draw calls, respond to resize, and shut down bgfx before destroying SDL.
5. Diagnose unsupported sprite rendering explicitly.
6. Add CI builds/tests and manual visual verification instructions.

**Acceptance:** packaged runtime visibly renders the sample rectangle, circle, and triangle; resize, Escape, and close work. CI compile alone is not visual acceptance.

### B. Editor Run/Stop (Checkpoint 3)
1. Add runtime executable discovery from a configured path and predictable sibling install layout.
2. Save the scene before launching; never pass an unsaved or ambiguous path.
3. Launch with QProcess argument arrays, capture stdout/stderr/exit status, and stop only the child process owned by this editor.
4. Prevent duplicate launches and stop the child safely when the editor closes.
5. Surface missing-runtime and failed-launch errors in the UI.

**Acceptance:** manually create/save a scene, Run from the editor, see it in runtime, then Stop from the editor.

### C. Installer and packaging (Checkpoint 4)
1. Package editor, runtime, Qt plugins, bgfx/shader runtime resources, MSVC runtime, sample scene, and license/notice files.
2. Keep per-user install and external project data isolation.
3. Add file-layout and command-line smoke checks to installer CI.
4. Do not claim clean-machine GUI launch, repair, or upgrade until manually tested.

**Acceptance:** clean Windows x64 install launches both applications; uninstall preserves external projects.

### D. Updater and release pipeline (Checkpoint 5)
1. Implement strict manifest parsing and compatibility checks against the existing JSON schema.
2. Implement SHA-256 package verification, bounded download, and safe staging.
3. Implement a separate apply helper, backup/rollback, and fault-injection tests.
4. Verify Ed25519 signatures against a built-in public key, with a test-only key pair for automated tests.
5. Keep private release keys out of source control and ordinary CI. Without approved key custody and an official HTTPS release endpoint, updater may be tested with local fixtures but production update distribution remains pending.

**Acceptance:** valid test updates stage/apply/rollback; invalid signatures, hashes, platform/channel/version, malformed manifests, interrupted downloads, and traversal packages fail closed.

### E. End-to-end verification and website (Checkpoint 6)
1. Run editor/runtime tests, renderer CI, installer lifecycle tests, and updater security tests.
2. Record manual tests separately from CI.
3. Keep About Veyra content truthful and verify website CI; owner manually checks live navigation.

## Completion reporting rules

- A successful CI build is not a manual GUI test.
- A local fixture update test is not a production release/update service.
- A clean install/uninstall workflow is not proof of repair/upgrade GUI acceptance.
- Preserve user projects; never replace or delete user content during update/rollback.
- Record every incomplete gate and its exact required next action. Phase 2 is not marked complete until every required acceptance item has evidence.

## Initial implementation checkpoint
The independent CPU geometry layer and tests are implemented. The latest known Windows Runtime Build #20 succeeded. No bgfx renderer, editor Run/Stop integration, updater executable, production release endpoint, or end-to-end manual test is claimed complete at the time this plan was written.
