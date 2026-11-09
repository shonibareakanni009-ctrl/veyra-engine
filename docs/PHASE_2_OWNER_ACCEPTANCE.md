# Phase 2 — Owner Acceptance Checklist

This document separates checks that can run in GitHub Actions from checks that require a Windows desktop or a live browser. Do not mark an item complete based on compilation alone.

## 1. Runtime visual acceptance

**Prerequisite:** use the newest `veyra-runtime-windows-x64` artifact from a successful Windows Runtime Build for the current main commit. Do not use an artifact produced before the bgfx target/CMake fixes.

1. Extract the runtime package to a clean folder.
2. Open PowerShell in that folder.
3. Run:
   ```powershell
   .\VeyraRuntime.exe .\first-scene.vscene
   ```
4. Confirm the 1280×720 runtime opens and visibly draws the sample rectangle, circle, and triangle.
5. Confirm the triangle's rotation and all three object positions/dimensions look consistent with the editor scene.
6. Resize the window several times and confirm rendering follows the new client size.
7. Press Escape and repeat by closing the window's close button; both must exit cleanly.
8. If the app fails, copy the console output and the exact artifact/run URL into the result below.

| Result | Record |
|---|---|
| Runtime artifact / workflow run | |
| Rectangle visible | ☐ Pass / ☐ Fail |
| Circle visible | ☐ Pass / ☐ Fail |
| Triangle visible with expected rotation | ☐ Pass / ☐ Fail |
| Resize remains correct | ☐ Pass / ☐ Fail |
| Escape exits cleanly | ☐ Pass / ☐ Fail |
| Window close exits cleanly | ☐ Pass / ☐ Fail |
| Notes / error output | |

Sprites are not in this renderer milestone. Visible sprite objects are intentionally skipped and logged.

## 2. Editor Run/Stop acceptance

**Prerequisite:** use the newest combined installer artifact from a successful Windows Installer Build for the current main commit.

1. Install Veyra Engine in a clean Windows user account or test VM.
2. Open Veyra Editor.
3. Create a scene containing a rectangle, circle, and triangle; save it as a `.vscene` file.
4. Change one object and press F5. Confirm the editor saves the scene and launches the runtime.
5. Confirm the runtime shows the saved scene, not an older copy.
6. Return to the editor and press Shift+F5. Confirm only the runtime process started by this editor closes.
7. Confirm runtime output and exit status appear in the Output dock.
8. Try Run with a missing runtime executable (for example, temporarily point `VEYRA_RUNTIME_PATH` to a missing file) and confirm a clear error is shown.
9. Close the editor while runtime is running; confirm the editor closes its child runtime process.

| Result | Record |
|---|---|
| Installer artifact / workflow run | |
| Save → F5 opens correct scene | ☐ Pass / ☐ Fail |
| Shift+F5 stops owned runtime | ☐ Pass / ☐ Fail |
| Output dock captures logs and exit | ☐ Pass / ☐ Fail |
| Missing-runtime message is clear | ☐ Pass / ☐ Fail |
| Editor close stops owned runtime | ☐ Pass / ☐ Fail |
| Notes / error output | |

## 3. Installer acceptance

Automated workflow already exercises silent install, required application files/CLI help, silent uninstall, and preservation of a project marker outside the application folder. Additional owner checks:

1. Install on a clean Windows x64 VM/user account.
2. Launch the editor and launch the included runtime sample.
3. Install the same version again or run Repair (if exposed by Inno Setup) and confirm the install remains healthy.
4. Test an upgrade from the previous available Veyra version if that version is still available.
5. Confirm projects/assets outside the application folder remain untouched.
6. Uninstall and confirm the external projects remain.

| Result | Record |
|---|---|
| Clean install and editor launch | ☐ Pass / ☐ Fail |
| Runtime sample launch | ☐ Pass / ☐ Fail |
| Repair/reinstall | ☐ Pass / ☐ Fail |
| Upgrade | ☐ Pass / ☐ Fail / ☐ Not available |
| User project preservation | ☐ Pass / ☐ Fail |
| Uninstall | ☐ Pass / ☐ Fail |
| Notes / Windows version | |

## 4. Website acceptance

Open the production Veyra website, click **About Veyra**, and confirm it scrolls to the actual About section at `#about` on desktop and mobile-sized layouts.

| Result | Record |
|---|---|
| Production URL / deployment | |
| About navigation works | ☐ Pass / ☐ Fail |
| Mobile layout works | ☐ Pass / ☐ Fail |

## 5. Updater scope and production gate

The current Windows updater executable is a **local verification/staging prototype**. Its Windows CI passes the test signature, package size/SHA-256, tampering rejection and safe local staging tests. It does not perform remote download, archive extraction, installation replacement, rollback or production release discovery.

A production update service still needs a release endpoint, a fully conformant RFC 8785/JCS signed-payload implementation and tests, approved trusted-public-key provisioning, a separate apply helper, and transactional backup/rollback. Release signing has deliberately not been configured because it was deferred by the project owner. Do not point this prototype at real users as an auto-updater.

## Phase 2 completion decision

Phase 2 may be called complete only after:
- The renderer Windows build and its automated tests pass for the latest main commit.
- Sections 1–4 above have recorded passing owner results.
- The updater production gate is either implemented and verified or explicitly removed from Phase 2 scope by a documented project decision.

A blank runtime window, successful compile, or installer CLI smoke test does not satisfy visual or end-to-end acceptance.
