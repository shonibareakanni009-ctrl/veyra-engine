# Veyra Updater — Trust and Manifest Design

**Status:** A Windows local verifier/stager prototype is implemented. Windows build, security tests, package staging, and artifact upload passed on runs [42659055214](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42659055214), [42659501624](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42659501624), and [42659872220](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42659872220). Production update distribution/apply/rollback is not implemented.

## Trust model

The updater must treat both release metadata and downloaded packages as untrusted until verified. HTTPS is transport protection, not proof that a release is authentic.

### Manifest contract

The first manifest schema is in [manifest.schema.json](../updater/manifest.schema.json). It currently defines a Windows x64 release with:
- Product identity and schema version.
- Semantic version and stable/beta/developer channel.
- Minimum compatible updater version.
- HTTPS package URL, byte size, and SHA-256 digest.
- Detached Ed25519 signature metadata and key identifier.

The schema validates structure only. It does not verify a signature, package hash, compatibility, or trust.

### Signature input

Before implementation, the signing input must be frozen as a deterministic canonical representation:
1. Parse the JSON manifest and validate it against the schema.
2. Remove the top-level signature member.
3. Canonicalize the remaining JSON using RFC 8785 JSON Canonicalization Scheme (JCS).
4. Verify the detached Ed25519 signature over the exact UTF-8 canonical bytes.
5. Resolve keyId only against public keys compiled into or securely provisioned to the updater.
6. Reject unknown keys, algorithms, schema versions, products, channels, platforms, architectures, and versions.

This prevents whitespace or property-order changes from changing the signed payload and prevents the manifest from choosing its own trust anchor.

### Key custody and rotation

- The Ed25519 private signing key must never be committed to Git, included in an installer, or exposed to pull-request CI.
- Keep the release private key offline or in a protected signing service / protected release environment.
- Embed only public verification keys in the updater.
- Key rotation must be staged: a release trusted by an existing key can introduce a next public key with an explicit validity/activation policy before the old key is retired.
- A key ID is an identifier, not a trust proof.

### Package handling

- Verify declared size and SHA-256 before extraction.
- Extract only into a newly created staging directory.
- Reject absolute paths, drive prefixes, parent traversal, symlinks/reparse points, duplicate normalized paths, and entries that escape the staging root.
- Never execute commands or scripts supplied by a manifest.
- Keep the current installation intact until the staged version passes validation.
- Apply through a separate helper after the editor/runtime have exited.
- Preserve the previous version until post-install checks pass; rollback must affect application files only, never user projects.

## Implementation gates

1. Add a schema-validation test suite with valid and invalid manifest fixtures.
2. Add an Ed25519 verifier with a test public/private key pair used only in tests.
3. Add SHA-256 package verification and bounded download handling.
4. Add safe archive extraction tests, including traversal and symlink cases.
5. Add a staged updater helper with rollback and fault-injection tests.
6. Add protected release signing only after key custody and recovery procedures are approved.

**Historical design baseline:** The initial schema commit did not contain an updater executable or cryptographic verifier. The current local prototype and its remaining limits are documented in the implementation section below.


## Implemented local prototype (2026-11-09)

The repository now contains:

- `updater/CMakeLists.txt`: Windows x64 CMake target with pinned nlohmann/json 3.12.0 and Monocypher 4.0.3 (immutable commit `ab2b16dd619ad5f6979a4fbe69cfa324a6fcc35f`).
- `updater/src/manifest_verifier.cpp`: strict allow-list/type/field validation for the current manifest contract, Ed25519 detached-signature verification, Windows CNG SHA-256 streaming hash, byte-count checking, and no-overwrite verified local staging.
- `updater/src/main.cpp`: `verify-local` CLI. It accepts an explicit trusted key ID/public key from a trusted local caller, validates a local manifest and local package, then stages a verified copy.
- `updater/tests/manifest_verifier_tests.cpp`: deterministic test-only key pair, valid signature, package hash/size, tampering, wrong public key/key ID, wrong platform/scheme, malformed JSON, and no-overwrite staging tests.
- `.github/workflows/windows-updater-build.yml`: Windows x64 configure/build/test/package workflow.
- The installer build workflow now builds the updater, stages its executable/schema, and checks the installed CLI.

**CI evidence:** [Windows Updater Build run 42658814633](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/42658814633) succeeded on commit `6f6c8d2608a2c0b26be7716d577fbaf5440c78c2`, including CTest and artifact upload. The subsequent commit `866c9e0` removes temporary debug output; its own CI run must also pass before treating that cleaned source snapshot as verified.

### Explicit limitations

This is a local verification and staging prototype, **not a live self-updater**. It does not fetch a remote manifest/package, extract an archive, launch an installer/helper, replace the installed app, migrate data, or roll back a failed update. No release endpoint, trusted production public key, release private key, key rotation process, or protected signing job is configured. The project owner previously deferred release signing; no private key is requested or stored here.

The canonical signed payload implementation currently relies on the constrained manifest's fixed field set, ASCII string values and integer-valued numeric fields with nlohmann/json's compact sorted-key serialization. Before using real release signatures, this serialization must be tested against RFC 8785/JCS test vectors or replaced with a fully conformant JCS implementation. It must not be advertised as general-purpose JCS support.

Staging copies and re-hashes a verified local package into a new staging directory. It does not extract archive members, so archive traversal/symlink protections and transactional apply/rollback remain future work.
