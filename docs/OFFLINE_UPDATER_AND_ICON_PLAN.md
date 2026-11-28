# Offline Updater and Veyra Application Icon — Implementation Plan

**Status:** Plan committed before implementation.
**Target:** Windows x64, offline update workflow.
**Existing baseline:** The current updater verifies a signed manifest and package digest, then stages a verified package. It does not apply updates.

## 1. User workflow

1. Veyra is installed normally.
2. The user obtains an offline update bundle on another device or through a separate download, then copies it to the PC or USB drive.
3. The user opens **Veyra Updater** from the Start Menu or installation folder.
4. The user selects the offline bundle.
5. The updater verifies release metadata and package integrity using a public key already trusted by the installed updater.
6. The updater checks product, Windows x64 architecture, channel, installed version, and compatibility.
7. The user sees the version/release notes and confirms.
8. The updater asks the user to close Veyra Editor and Runtime.
9. It stages the new files, backs up the current application version, applies the update, and runs post-update checks.
10. On failure, it restores the prior application files. Projects and user settings remain outside the update transaction.

The update process must not need network access. The update bundle and trust key must already be present locally.

## 2. Offline bundle contract

Define a versioned offline bundle format with:
- A signed release manifest.
- A package archive or payload tree.
- Explicit product/version/platform/architecture/channel compatibility.
- A cryptographic digest and byte size for the payload.
- A signature key ID checked against a public key installed with Veyra.
- A strict file allowlist/root layout so updates cannot write outside the application-owned directories.
- No arbitrary scripts, commands, or manifest-supplied executable hooks.

Reject malformed, unsigned, wrong-product, wrong-platform, stale/incompatible, path-traversing, oversized, or corrupted bundles before modifying the installation.

## 3. Transaction and rollback

- Never overwrite running executables.
- Stage and verify all files before touching the active installation.
- Back up only application-owned files.
- Apply from the staging area using a controlled transaction.
- Run post-update executable/version checks.
- Restore the backup if application or validation fails.
- Keep logs and failed bundles for diagnosis; do not log secrets.
- Never modify projects, scenes, source assets, or user settings.

## 4. UI and packaging

- Provide a simple native Windows updater window with Select Update Bundle, bundle/version details, Verify, Install Update, progress/status, and error reporting.
- Add Start Menu shortcut **Veyra Updater**.
- Keep the updater independent from the editor so it can replace editor/runtime files after those processes close.
- Installer includes the updater, its trust configuration, and documentation.

## 5. Veyra icon

- Add one original scalable vector icon source under `assets/branding/`.
- Visual direction: a strong geometric V mark with a subtle orbital/engine motif, readable at small sizes, dark indigo/blue foundation with a bright cyan accent, transparent background.
- Generate Windows ICO sizes for the editor executable, installer wizard/setup icon, and shortcuts from this single source.
- Set the Qt application/window icon and Inno Setup icon metadata consistently.
- Keep the SVG as the canonical editable source.

## 6. Implementation and verification

1. Add this plan and icon source, commit/push.
2. Implement the offline bundle parser, signature/integrity checks, safe payload extraction, path validation, staged apply, backup, and rollback.
3. Implement the standalone updater UI and command-line diagnostics.
4. Integrate updater and trust files into the installer and add Start Menu shortcut.
5. Generate/apply application icon resources in CI and installer packaging.
6. Add automated tests for valid bundle, tampered manifest/payload, wrong platform/product, path traversal, interrupted apply, and rollback.
7. Run Windows updater, editor, and installer workflows; record exact results in `docs/PROGRESS.md`.

## 7. Security boundaries

- No network calls during the offline update operation.
- No private signing key in the application, repository, or CI logs.
- No trusting a public key supplied by the update bundle itself.
- No arbitrary scripts or commands from the bundle.
- Executable Authenticode signing remains deferred as previously decided; update-manifest authenticity remains mandatory and is a separate control.

## 8. Acceptance criteria

- A user can apply a valid offline bundle from local disk or USB with Wi-Fi disabled.
- The updater clearly identifies the installed and target versions before confirmation.
- A tampered/invalid bundle is rejected without changing installed files.
- A successful update replaces editor/runtime/updater files and launches the updated editor.
- A failed update restores the previous working installation.
- User projects and settings survive both successful updates and rollback.
- Editor executable, window, installer, and shortcuts display the same Veyra icon.
- Windows CI and install/update/rollback tests pass.
