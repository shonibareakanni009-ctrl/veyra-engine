# Veyra Engine — Installer and Updater Design

**Status:** Phase 2 design baseline; installer technology provisionally selected, implementation and updater still pending.  
**Last updated:** 2026-11-03  
**Initial target:** Windows x64.

## 1. Purpose and scope

Veyra needs a dependable way for users to install, launch, update, repair, and uninstall the editor and native runtime. Installation and updates are product capabilities, not an afterthought.

The initial implementation should support Windows x64. Linux, Android, and Web distribution paths require separate designs and validation; they must not be implied by the Windows installer.

This document records the intended behavior and security requirements. Specific installer technology, signing provider, hosting arrangement, and manifest encoding remain decisions to validate and record before implementation.

## 2. Product boundaries

Keep these responsibilities separate:

- **Veyra Editor:** desktop authoring application.
- **Veyra Runtime:** executable/libraries required to run games.
- **Veyra Installer:** installs and removes application-owned files.
- **Veyra Updater:** discovers, verifies, stages, and applies application updates.
- **User projects and data:** projects, source assets, settings, and user-created content.

The editor and runtime may share engine libraries, but exported games must not require Qt merely because the editor uses Qt.

An engine update must never silently replace, migrate, or delete a user's project content. Project-format migrations belong to explicit project migration workflows with backups and user consent.

## 3. Initial installation layout

Before implementation, choose and document exact Windows paths and per-user versus per-machine installation behavior. The layout must distinguish:

- Versioned application binaries and libraries.
- Updater/bootstrapper executable and its recovery data.
- Shared resources that are safe to share across application versions.
- Per-user settings, logs, caches, and update preferences.
- User projects and source assets, which remain outside the application installation directory by default.

Installation must not require administrator privileges unless the selected install scope genuinely requires them. The installer should explain requested permissions and provide a supported per-user option where practical.

## 4. Installer technology decision

Evaluate at least Inno Setup, WiX Toolset, and CPack (including whether CPack alone meets the required Windows installer experience). Record a decision before committing to one.

Evaluation criteria:

- Windows x64 support and compatibility with the project's CMake build.
- Per-user and per-machine installation.
- Upgrade, repair, and uninstall behavior.
- File replacement and running-process handling.
- Signing support and SmartScreen/publisher identity implications.
- Silent/automated install options for CI and testing.
- Licensing, maintenance burden, documentation, and contributor familiarity.
- Ability to bundle Qt runtime dependencies, SDL3/bgfx runtime dependencies, and the Veyra updater safely.

Do not select a tool merely because it creates an installer quickly; test the complete lifecycle.

## 5. Release manifest and channels

Define a versioned manifest schema before the updater implementation. At minimum, a release entry should identify:

- Manifest schema version.
- Product and release version.
- Release channel (stable, beta, or developer).
- Target operating system and CPU architecture.
- Minimum updater/bootstrapper version.
- Package URL and package size.
- Cryptographic package digest.
- Signature metadata and key identifier.
- Release notes URL or content reference.
- Optional minimum supported/current version and rollout metadata.

The updater must validate schema, product identity, channel, platform, architecture, version compatibility, and signature before trusting a package location or attempting installation.

The manifest must be authenticated, not merely downloaded over HTTPS. Choose a signature format and key-management/rotation policy before release implementation. Never embed a private signing key in the application, repository, or CI logs.

## 6. Update workflow

Expected update sequence:

1. Read the user's configured release channel.
2. Fetch the manifest from the configured official release source.
3. Validate manifest structure, product identity, channel, platform, architecture, and compatibility.
4. Verify the manifest signature against a trusted public key/key policy.
5. Compare installed and available versions.
6. Show the available version and release notes; allow the user to defer.
7. Download to a staging directory, not over installed files.
8. Verify package size and cryptographic digest; validate package signature/trust metadata.
9. Close or ask the user to close Veyra processes.
10. Launch a separate updater helper with narrowly scoped install instructions.
11. Preserve the previous working version until the new version passes post-install checks.
12. Record the result and offer restart.

The updater must fail closed: if trust, integrity, compatibility, or package validation cannot be established, do not install the update.

## 7. Atomic application and recovery

Do not overwrite live executable files while the editor or runtime is running. Use a staged version directory or equivalent transactional strategy:

- Download and validate the complete package first.
- Prepare the new version separately.
- Keep the current version available until replacement is validated.
- Switch the active version using a controlled operation suitable for Windows filesystem semantics.
- Run basic post-update checks.
- If staging, replacement, or post-update validation fails, restore the previous version.
- Keep recovery logs that explain the failure without recording secrets.
- Define how stale staging directories and old versions are cleaned up.

Rollback must not roll back or overwrite user projects, source assets, or settings.

## 8. Security requirements

- Authenticate update metadata with a documented signature scheme and trusted key policy.
- Verify package integrity before execution or installation.
- Use HTTPS for transport, but do not treat transport security alone as package authenticity.
- Protect signing keys; keep private keys out of source control and ordinary build artifacts.
- Reject unexpected product IDs, channels, architectures, versions, paths, and package types.
- Prevent path traversal and archive extraction outside the intended staging/install directory.
- Apply least privilege; elevate only when required for the selected install scope.
- Do not execute arbitrary scripts or commands supplied by an untrusted manifest.
- Make update source and release channel visible to the user.
- Keep security-relevant logs, but never log credentials or signing secrets.

## 9. User experience requirements

Users should be able to:

- Install Veyra and understand where it is installed.
- Launch the editor without needing to understand runtime packaging.
- Check for updates manually.
- Configure automatic update checks and release channel.
- Review version and release notes before applying an update.
- Defer an update and continue using the current working version where safe.
- See useful errors and a recovery path if an update fails.
- Uninstall Veyra without silently deleting projects or user data.

The first release may use a simple settings/dialog workflow; background downloads and advanced rollout controls can come later.

## 10. Automated release pipeline

GitHub Actions should eventually:

- Build the editor and runtime for the declared Windows x64 target.
- Run automated tests.
- Assemble a clean release staging directory.
- Produce installer and updater artifacts with versioned names.
- Generate package digests and release metadata.
- Sign release metadata/packages using protected signing credentials when configured.
- Publish artifacts and release notes only after required checks pass.
- Preserve build provenance and test results.

Pull-request CI must not receive production signing secrets. Release signing should run only in a protected release workflow with narrowly scoped credentials and approvals.

## 11. Required test matrix

| Scenario | Expected result |
|---|---|
| Clean install | Editor and runtime launch |
| Upgrade from previous version | New version installs; projects/settings remain intact |
| Same-version reinstall/repair | Installation is repaired without deleting user content |
| Uninstall | Application files are removed; projects remain unless user explicitly opts in to delete them |
| Invalid or unsigned manifest | Update is rejected |
| Wrong product/channel/platform/architecture | Update is rejected |
| Corrupted or truncated package | Package is rejected before application |
| Interrupted download | Current installation remains usable; download can be retried |
| Application still running | Updater waits or asks for closure; no unsafe in-place overwrite |
| Insufficient permissions | Clear failure; existing installation remains usable |
| Failed replacement/post-update check | Previous version is restored |
| No network connection | Installed version remains usable; update check reports unavailable |
| Old updater version | Manifest compatibility rule requests a safe bootstrapper update or explains incompatibility |

Record the operating system version, architecture, installed version, package version, test result, and logs for each release qualification run.

## 12. Installer technology decision and remaining open decisions

### Provisional selection: Inno Setup 7.1.0 for the first Windows x64 installer

Inno Setup is the preferred initial installer technology because it can produce a standalone Windows setup executable, supports both administrative and non-administrative installation modes, and provides a complete uninstall workflow. The first implementation will default to per-user installation using `PrivilegesRequired=lowest`; a per-machine option can be evaluated later.

Why not the alternatives for the first release:
- **WiX Toolset:** a strong MSI option with per-user and per-machine scope support, but it adds MSI authoring/toolchain complexity that is not currently needed for this first distribution target.
- **CPack:** useful as a packaging integration layer and supports both Inno Setup and WiX generators, but its generator-specific feature coverage and packaging behavior still need validation. We will use a direct Inno Setup script initially so install paths, shortcuts, and user-data exclusions are explicit.

This is a provisional engineering decision, not evidence that an installer exists or that install/uninstall behavior has passed testing. It may be revisited if the prototype exposes a concrete limitation.

References:
- Inno Setup capabilities and supported install modes: https://jrsoftware.org/isinfo.php
- Inno Setup downloads and current version information: https://jrsoftware.org/isdl.php
- Inno Setup non-administrative install mode: https://jrsoftware.org/isfaq.php
- CPack Inno Setup generator: https://cmake.org/cmake/help/latest/cpack_gen/innosetup.html
- CPack WiX generator and install scope: https://cmake.org/cmake/help/latest/cpack_gen/wix.html

### Remaining open decisions

1. Release hosting and manifest delivery location.
2. Manifest format and signature scheme.
3. Signing key custody, rotation, and recovery.
4. Versioning policy and stable/beta/developer channel rules.
5. Staged-directory layout and rollback retention policy.
6. Whether automatic checks are enabled by default and how users control them.
7. Exact minimum supported Windows version and clean-machine test image.

## 13. Phase 2 completion gate

The installer/updater portion of Phase 2 is complete only when:

- A clean Windows x64 machine can install and launch Veyra.
- A released version can be detected and updated through the updater.
- Update metadata and packages are authenticated and integrity-checked.
- A failed/interrupted update does not corrupt the existing working installation.
- Upgrade and uninstall preserve user projects and settings according to documented behavior.
- Automated release packaging and repeatable tests provide evidence for the claimed support.

**Current truth:** Inno Setup 7.1.0 is the provisional installer choice. The user-facing installer, updater, signed update manifest, rollback mechanism, and automated release publishing are not yet implemented or verified.
