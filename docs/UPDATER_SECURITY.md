# Veyra Updater — Trust and Manifest Design

**Status:** Design baseline and JSON Schema committed; updater executable and cryptographic verification are not implemented.

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

**Current limitation:** No release endpoint, signing key, public-key rotation policy, updater executable, package download, or update application is configured yet. The schema is not a live release manifest.
