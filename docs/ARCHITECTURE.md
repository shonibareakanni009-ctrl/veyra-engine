# Veyra Engine — Technical Architecture

**Status:** Proposed baseline — not an implementation specification  
**Last updated:** 2026-11-02  
**Scope:** Engine runtime, editor, scripting, platform support, rendering, and export

This document describes the architectural boundaries Veyra should preserve while technical choices are evaluated. It does not select a core implementation language, graphics API, windowing library, physics library, or build system. Those decisions require comparative evaluation and, where practical, small prototypes.

The project vision in the root README remains authoritative: Veyra aims to be a 2D/3D engine with visual and code workflows, multiple scripting languages, and Windows, Linux, Android, and Web targets.

---

## 1. Architectural goals

Veyra's architecture should:

1. Support 2D and 3D without treating either as an unrelated engine.
2. Keep the engine runtime separate from editor-only functionality.
3. expose one documented engine-facing API to all supported scripting languages.
4. Separate host tools from game target runtimes.
5. Allow platform and graphics backends to evolve behind stable interfaces.
6. Make unsupported language/platform combinations explicit instead of implying universal compatibility.
7. Keep modules testable in isolation and make integration failures diagnosable.
8. Avoid committing the project to technology choices before prototypes provide evidence.

### Quality attributes

| Attribute | Architectural response |
|---|---|
| Portability | Platform services behind explicit interfaces; target-specific implementations |
| Performance | Measure critical paths; allow native implementations where justified |
| Extensibility | Versioned interfaces for scripting and plugins |
| Reliability | Explicit lifecycle, resource ownership, error propagation, and tests |
| Security | Treat project scripts, plugins, and imported assets as untrusted input |
| Maintainability | Narrow module responsibilities, dependency rules, and architecture decisions |
| Developer experience | Consistent project format, diagnostics, editor tools, and documentation |

These are goals to validate, not claims that they have already been achieved.

---

## 2. System overview

Veyra is best treated as a set of cooperating products and layers rather than one executable containing every feature.

```text
                    VEYRA PROJECT
                         |
        +----------------+----------------+
        |                                 |
   VEYRA EDITOR                      GAME BUILD
        |                                 |
  Project tools                    Export pipeline
  Visual editor                          |
  Code editor                     Target package
  Asset tools                            |
  Debugger                         GAME RUNTIME
  Profiling tools                         |
        |                     +-----------+-----------+
        +---- Engine APIs -----| Core + Engine Systems |
                              +-----------+-----------+
                                          |
                             Platform and Render APIs
                                          |
                           Target-specific implementations
                                          |
                    Windows / Linux / Android / Web
```

The diagram is conceptual. The editor may reuse runtime libraries, but editor-only systems must not become required dependencies of exported games.

### Main architectural areas

- **Core runtime:** lifecycle, time, identifiers, event dispatch, scene/entity model, resource references, serialization, and shared utilities.
- **Platform layer:** operating-system services such as files, clocks, input, windows/surfaces, threading, and application integration.
- **Rendering:** graphics-facing interfaces, render-resource management, 2D and 3D pipelines, and graphics backends.
- **Engine systems:** physics, audio, animation, navigation, networking, and other gameplay services.
- **Scripting:** common engine API, language adapters, runtime discovery/management, script loading, execution, and diagnostics.
- **Editor:** project browser, scene tools, inspectors, visual logic tools, code editing, asset workflows, debugger, and profiler.
- **Build and export:** asset processing, dependency resolution, runtime selection, platform packaging, and reproducible build metadata.
- **SDK and extensions:** public APIs, templates, examples, plugin interfaces, and developer documentation.

---

## 3. Dependency rules

The following rules are more important than the eventual folder names.

1. **Core must remain independent of the editor.** A game build must not need editor UI code.
2. **Core must not directly call a specific operating-system API.** It should request platform services through interfaces.
3. **Gameplay systems must not depend on a particular graphics backend.** Rendering should consume engine-level scene/render data through defined interfaces.
4. **Language adapters must depend on the common scripting contract.** They must not create separate, incompatible engine APIs.
5. **The editor may depend on runtime-facing APIs, but the runtime must not depend on the editor.**
6. **Export tooling may assemble platform-specific components, but platform-specific code must not leak into unrelated modules.**
7. **Plugins must use documented extension points.** They must not rely on private implementation details.
8. **Cross-module communication should use defined interfaces, messages, or events rather than uncontrolled access to internal state.**

A dependency that violates these rules should be treated as an architecture change and reviewed deliberately.

---

## 4. Core runtime

The core runtime owns shared engine behavior and foundational data models.

### Responsibilities

- Application and game lifecycle.
- Frame/tick scheduling and time-step policy.
- Entity/component or equivalent scene-object model.
- Scene loading, activation, and transitions.
- Event and messaging facilities.
- Resource handles, lookup, lifetime, and dependency tracking.
- Project/runtime configuration.
- Serialization contracts and versioned project data.
- Common diagnostics and error types.

### Design constraints

- Avoid making the core depend on a specific renderer, scripting language, editor UI, or operating system.
- Prefer stable identifiers and explicit resource handles over uncontrolled raw references across subsystem boundaries.
- Define ownership and lifetime rules before introducing asynchronous loading or plugin unload.
- Keep deterministic behavior where practical, especially for serialization, tests, and reproducible builds.

The exact entity model (ECS, scene-node hierarchy, or a hybrid) is an open design decision. It should be evaluated against editor usability, runtime performance, 2D/3D needs, and scripting ergonomics.

---

## 5. Platform abstraction

A host platform is the machine running the editor or build tools. A target platform is the machine or environment that runs the exported game. They are not necessarily the same.

For example, a developer may use the editor on Windows and export a game for Android or the Web.

### Platform services

The platform boundary may include:

- Window and display/surface creation.
- Input devices and event translation.
- File-system access and virtual project paths.
- High-resolution clocks and timers.
- Threading and synchronization primitives.
- Audio device access.
- Networking primitives.
- Application lifecycle, suspend/resume, and shutdown.
- Platform-specific permissions and integration.

Each service needs a documented capability model. A target that lacks a capability must report that limitation rather than silently pretending the service exists.

### Target profiles

Each export target should define:

- Supported CPU architectures and graphics capabilities.
- Available platform services.
- Runtime and scripting constraints.
- Packaging, signing, and permission requirements.
- Asset formats and size constraints.
- Debugging and profiling capabilities.
- Known limitations and validated versions.

Windows, Linux, Android, and Web are intended targets, not yet verified support claims.

---

## 6. Rendering architecture

Rendering should expose engine-level concepts while allowing different graphics backends beneath them.

### Proposed separation

- **Render API:** backend-neutral commands, resources, capabilities, and synchronization contracts.
- **Render resources:** textures, buffers, shaders/material data, meshes, and render targets.
- **2D renderer:** sprites, layers, tilemaps, text, cameras, batching, and 2D lighting as supported.
- **3D renderer:** meshes, materials, cameras, lights, depth, visibility, and 3D-specific passes.
- **Backend implementations:** translate the render API to a selected graphics API and target environment.

The 2D and 3D renderers should share resource management and backend infrastructure where appropriate, without forcing identical rendering techniques.

### Constraints to resolve through prototypes

- Desktop graphics API choice and fallback strategy.
- Android graphics API and device capability handling.
- Browser graphics path and WebAssembly integration.
- Shader/material portability and asset compilation.
- Render-thread model and resource lifetime.
- Headless rendering and automated test strategy.

No graphics API is selected by this document.

---

## 7. Scripting and language integration

Veyra's scripting architecture must support multiple languages without implementing a different engine for each language.

### Shared engine-facing API

The engine should define one language-neutral API contract for concepts such as:

- Scene and entity access.
- Component/property access.
- Transform and spatial operations.
- Input queries and events.
- Resource loading and references.
- Physics queries and signals.
- Audio and animation commands.
- Logging, diagnostics, and controlled engine services.

Each language adapter maps the contract into idiomatic language features. A language adapter must not silently expose capabilities that another adapter cannot implement consistently unless those capabilities are clearly documented as language-specific extensions.

### Language adapter responsibilities

Each adapter is responsible for:

- Initializing and shutting down its runtime.
- Loading and unloading scripts.
- Mapping engine types and values.
- Exposing engine APIs and callbacks.
- Converting errors and stack traces into engine diagnostics.
- Managing object references, memory ownership, and garbage-collection boundaries.
- Handling threading and callback rules.
- Supporting debugging hooks where the language runtime permits.
- Declaring supported target platforms and runtime versions.

### Runtime manager

The runtime manager should eventually:

- Identify language runtimes required by a project.
- Check runtime identity, version, architecture, and compatibility.
- Select an installed compatible runtime or report what is missing.
- Support controlled installation of approved runtimes where the platform permits.
- Support offline runtime packages.
- Pin runtime versions per project/build.
- Verify package integrity and signatures before installation.
- Prevent an update from silently breaking an existing project.
- Record runtime provenance in build metadata.

It must not download and execute arbitrary runtime binaries without integrity verification and user-visible trust controls.

### Compiler/interpreter is not the whole integration

A compiler or interpreter alone does not make a language supported. A complete integration also needs engine bindings, type conversion, callbacks, error handling, lifecycle integration, memory ownership rules, debugging, and target-compatible packaging.

The Web target has a distinct execution model: browser sandbox rules and WebAssembly/Web APIs constrain what can run. Desktop or Android native runtime assumptions cannot simply be copied into a browser build.

### Language support matrix

Support should be tracked per language and target, for example:

| Language | Windows | Linux | Android | Web |
|---|---|---|---|---|
| Kotlin | Not evaluated | Not evaluated | Not evaluated | Not evaluated |
| Python | Not evaluated | Not evaluated | Not evaluated | Not evaluated |
| JavaScript | Not evaluated | Not evaluated | Not evaluated | Not evaluated |
| Lua | Not evaluated | Not evaluated | Not evaluated | Not evaluated |
| Java | Not evaluated | Not evaluated | Not evaluated | Not evaluated |

These entries are deliberately marked **Not evaluated**. A language becomes supported only after its adapter and runtime path pass documented tests on that target.

---

## 8. Editor architecture

The editor is a development application built around projects and the engine's public tooling APIs.

### Major editor areas

- Project creation, opening, configuration, and migration.
- Scene hierarchy and visual scene editing.
- Inspector and property editing.
- Asset browser, import settings, and asset dependency views.
- Visual scripting/logic tools, if included in the agreed feature scope.
- Code editor and language-aware tooling.
- Debugger, logs, profiler, and runtime controls.
- Build/export configuration and diagnostics.
- Plugin management and editor preferences.

The visual editor and code editor should be two workflows over shared project, scene, asset, and debugging models. They should not maintain competing representations of the same project data.

The editor UI technology remains open. It should be selected after considering desktop portability, Android needs (if an Android editor is pursued), accessibility, rendering integration, code-editor embedding, and maintenance cost.

---

## 9. Project, asset, and resource pipeline

A project needs stable, documented formats so editor tools, runtime builds, and external tooling agree on the same content.

### Proposed pipeline

1. Import source assets into the project.
2. Validate file type, size, metadata, and import settings.
3. Convert assets into engine-managed intermediate or derived formats as needed.
4. Record dependencies and content identifiers.
5. Reuse cached derived assets when the source and importer settings have not changed.
6. Package only assets required by the selected build.
7. Resolve runtime resource references through stable project paths or identifiers.

### Design constraints

- Source assets should remain distinguishable from generated artifacts.
- Generated data should be reproducible where practical.
- Importers should report errors with actionable file and setting information.
- Serialization formats need schema versions and migration rules.
- Asset loading should eventually support asynchronous work without exposing unsafe lifetime behavior to scripts.
- Export must not include editor-only metadata unless required.

Exact file formats and import libraries remain undecided.

---

## 10. Build and export pipeline

The editor host, build tools, and game target should be modeled separately.

A build should be described by explicit inputs:

- Project and engine version.
- Target platform and CPU architecture.
- Graphics/runtime profile.
- Enabled engine modules.
- Script languages and pinned runtime versions.
- Asset import settings and content.
- Build configuration and signing/packaging settings.

The exporter should validate these inputs before packaging. It should produce a build report that records included modules, runtimes, dependencies, warnings, and known limitations.

A successful package operation is not proof that a game runs correctly. Each target requires launch/runtime tests on an appropriate device, emulator, or browser environment.

---

## 11. Plugins and extension points

Plugins can extend the editor and, eventually, selected runtime capabilities.

The architecture should distinguish:

- **Editor plugins:** tools, panels, importers, inspectors, and workflow extensions.
- **Runtime modules:** optional engine capabilities included in a game build.
- **Native extensions:** platform/ABI-dependent code with explicit compatibility requirements.
- **Script packages:** reusable code using supported scripting APIs.

Every extension type needs a versioned contract, capability declaration, lifecycle, error reporting, and compatibility policy. Native code should be treated as trusted code with the privileges of the host process; a plugin interface is not automatically a security sandbox.

Plugin loading, signing, isolation, and hot-reload behavior remain open design topics.

---

## 12. Diagnostics, testing, and quality gates

Testing should follow module boundaries and target behavior.

### Test layers

1. **Unit tests:** pure logic, serialization, identifiers, math, and data structures.
2. **Contract tests:** platform, renderer, scripting, and plugin interfaces.
3. **Integration tests:** scene loading, resources, script-to-engine calls, and subsystem interaction.
4. **Regression tests:** previously fixed defects and project-format migrations.
5. **Target tests:** launch and exercise builds on each claimed target.
6. **Performance tests:** measured workloads with repeatable hardware/software context.

### Definition of supported

A feature is not considered supported merely because it compiles. Its documentation should state:

- What works.
- Which targets and versions were tested.
- How it was tested.
- Known limitations.
- Any required runtime or external dependency.

Errors should carry enough context to identify the project, subsystem, resource, script, and target involved without exposing secrets.

---

## 13. Security and trust boundaries

Game projects can contain user-authored scripts, plugins, downloaded packages, and complex assets. Treat those inputs as untrusted.

Architectural requirements:

- Validate paths and prevent project/import operations from escaping intended directories.
- Validate asset metadata and impose reasonable resource limits.
- Verify runtime packages before installation.
- Make native plugin trust explicit.
- Keep secrets out of project files, logs, and build artifacts.
- Provide clear permissions and warnings for capabilities such as file access and networking where feasible.
- Do not describe ordinary in-process scripts as sandboxed unless an actual isolation boundary exists.

---

## 14. Open technical decisions

These decisions are intentionally not settled by this document.

| ID | Decision | Evaluation criteria | Status |
|---|---|---|---|
| ADR-001 | Core implementation language | Portability, performance, ecosystem, FFI, maintainability, contributor access | Selected: C++20; prototype validation pending |
| ADR-002 | Editor implementation/UI technology | Desktop portability, tooling, accessibility, embedding, maintenance | Selected: Qt 6 Widgets for desktop editor; prototype pending |
| ADR-003 | Build system and dependency management | Reproducibility, cross-compilation, toolchain support, package management | Selected: CMake + Ninja; dependency acquisition method open |
| ADR-004 | Windowing and platform layer | Target coverage, lifecycle handling, input, licensing, maintenance | Selected: SDL3; target validation pending |
| ADR-005 | Rendering API/backend strategy | Windows/Linux/Android/Web feasibility, capability model, shader workflow | Selected: bgfx; integration and target validation pending |
| ADR-006 | Entity and scene model | 2D/3D needs, performance, editor UX, serialization, scripting access | Open |
| ADR-007 | First scripting language | Integration complexity, target compatibility, debugging, runtime size, developer UX | Selected: Lua 5.5; adapter not implemented |
| ADR-008 | Asset/project file formats | Versioning, migration, diffability, performance, tooling | Open |

Selected baseline decisions are recorded in [TECH_STACK.md](TECH_STACK.md) and [ADR-001](decisions/ADR-001-implementation-stack.md). Commenting requirements are recorded in [CODING_STANDARDS.md](CODING_STANDARDS.md). The entity model and project/asset file formats remain open. Selected technologies must still pass their prototype and target-validation gates. A decision should be revisited when new evidence justifies it, not merely because a different technology becomes fashionable.

---

## 15. Prototype strategy

Architecture must be tested by small, representative slices rather than accepted on diagrams alone.

The first prototype should be selected after the core-language and platform/build evaluations. It should be deliberately small but prove a complete path:

- Build a minimal application for one chosen desktop host.
- Create a window or render surface.
- Initialize the selected rendering path.
- Clear the surface or draw one simple 2D primitive.
- Handle a basic input event.
- Shut down cleanly.
- Run a repeatable build and validation procedure.

A later scripting slice should call one engine API from the first selected language. That slice should test type conversion, callbacks, errors, runtime startup/shutdown, and packaging before additional languages are added.

This prototype strategy is a proposal, not a claim that implementation has begun.

---

## 16. Relationship to other project documents

- `README.md` communicates the public project vision and high-level goals.
- `docs/ROADMAP.md` tracks sequencing and project priorities.
- `docs/PROGRESS.md` records only validated accomplishments.
- `docs/MANAGER_NOTES.md` records management roles and decisions.
- This file defines proposed technical boundaries and open architecture questions.

When these documents differ, do not silently rewrite one to match another. Record the conflict, identify the owner of the decision, and resolve it explicitly.

---

## 17. Current implementation status

**Architecture document created; initial technology baseline selected; architecture and integrations still require prototype validation.**

- Working engine code: none verified by this document.
- Core language: C++20 selected; not yet implemented.
- Rendering backend: bgfx selected; not yet integrated.
- Editor technology: Qt 6 Widgets selected for desktop; not yet integrated.
- First scripting language: Lua 5.5 selected; adapter not implemented.
- Platform support: not verified.
- Prototype: not implemented.

The next technical activity is comparative evaluation of the open decisions, beginning with the core implementation language and the minimum toolchain needed for a reproducible desktop prototype.
