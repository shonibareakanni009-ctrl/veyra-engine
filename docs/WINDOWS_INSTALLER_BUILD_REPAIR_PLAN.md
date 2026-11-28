# Windows Installer Build Repair — MSVC/dav1d

**Status:** Repair plan committed before code changes.
**Incident:** Windows Installer Build #52 (run 46137497067), commit `08c31d2b08a8103bc4bf02599fb46bc790c0b885`.
**Scope:** Restore the Windows x64 editor/runtime/installer pipeline without weakening runtime functionality or skipping tests.

## Observed failure

The editor/runtime build step failed while compiling the pinned bgfx.cmake dependency's bimg dav1d sources. MSVC reported errors in `bimg/3rdparty/dav1d/include/compat/msvc/stdatomic.h`:

- C7780: function-like macro `__has_include_next` is not defined.
- C1017: invalid integer constant expression.

The runtime executable itself reached a successful link, but the overall dependency build failed. Engine tests, updater build/tests, installer packaging, and install smoke tests were consequently skipped.

The separate Windows Editor Build #60 passed. The failure is therefore isolated to the installer workflow's full editor+runtime dependency build path, not the icon-generation step.

## Repair approach

1. Reproduce and trace the exact pinned bgfx.cmake/bimg/dav1d configuration and identify the supported upstream CMake option or compiler-specific compatibility setting that avoids the invalid MSVC preprocessor branch.
2. Prefer a narrow, documented dependency configuration fix over editing fetched dependency source or globally weakening compiler flags.
3. Preserve required bgfx Direct3D 11 rendering and existing runtime capabilities; do not disable image/texture functionality merely to make CI green unless the project explicitly accepts that trade-off.
4. Add a short explanatory comment beside the CMake configuration so future dependency updates retain the fix.
5. Push the repair and trigger the Windows Installer Build plus the standalone Windows Updater Build.
6. Inspect job steps and logs. Only mark the repair complete when engine tests, updater security tests, installer compilation, install/uninstall smoke tests, and artifacts all pass.

## Acceptance

- Windows Editor Build remains green.
- Windows Installer Build passes dependency compilation, editor/runtime builds, all engine tests, updater build/security tests, installer compilation, and install/uninstall smoke tests.
- Standalone Windows Updater Build passes after a fresh run.
- `docs/PROGRESS.md` records exact run IDs and any remaining limitations.

## Constraints

- Do not hide or skip the failing dependency build.
- Do not remove tests or mark skipped steps as passing.
- Do not change the offline updater's documented implementation status: it remains a verifier/stager prototype until apply/rollback is implemented and tested.
