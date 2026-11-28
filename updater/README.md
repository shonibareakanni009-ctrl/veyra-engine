# Veyra Updater

**Current status:** Windows x64 local verification and staging prototype (0.1.0).

## Current capabilities

- Validates the local release manifest against the strict JSON schema.
- Verifies the Ed25519 signature using a trusted public key supplied by the caller.
- Checks the package byte size and SHA-256 digest.
- Stages a verified local package for inspection.

## Not implemented yet

This prototype does not download updates, unpack update archives, install or replace application files, launch a helper process, perform transactional rollback, or provide the integrated editor UI. Do not describe it as a complete offline installer/updater yet.

## Security requirements

- The trusted public key must come from a trusted local installation/configuration path. Never trust a key supplied by the update bundle or manifest.
- Keep private release signing keys outside this repository and outside CI logs.
- Reject invalid signatures and package digest/size mismatches before staging.
- Preserve the separation between application files and user projects/settings.

## Build and test (Windows x64)

```powershell
cmake -S updater -B build/updater -A x64 -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build/updater --config Release --parallel 2
ctest --test-dir build/updater -C Release --output-on-failure --verbose
```

The Windows Updater Build workflow runs these security tests and packages the CLI prototype.
