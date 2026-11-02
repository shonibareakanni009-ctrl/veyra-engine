# Veyra Engine

**A universal 2D and 3D game development engine.**

Veyra Engine is an ambitious, cross-platform game-engine project focused on giving developers the freedom to build games with the tools and programming languages that suit them.

The project is in its early planning and development stage. Its architecture will be developed incrementally, with an emphasis on portability, modularity, clear interfaces, and practical testing.

## Vision

Build one engine that can support:

- **2D and 3D game development** — from small 2D projects to fully three-dimensional worlds.
- **Visual and code-based workflows** — a visual editor alongside a code editor and debugging tools.
- **Multiple scripting languages** — a shared engine API with language adapters, planned for languages such as Kotlin, Python, JavaScript, Lua, and Java.
- **Cross-platform development** — tools and export workflows targeting Windows, Linux, Android, and the Web.
- **An extensible ecosystem** — plugins, reusable tools, asset workflows, and documented APIs.

## Guiding principles

1. **Modular by design:** rendering, physics, audio, scripting, editor tools, and platform support should be separable systems with well-defined interfaces.
2. **One engine API:** supported scripting languages should interact with consistent engine functionality rather than each receiving unrelated APIs.
3. **Portability with honesty:** a feature is only considered supported on a target platform when its runtime, dependencies, and behavior have been implemented and tested there.
4. **Developer choice:** language and workflow support should grow without compromising stability or maintainability.
5. **Open development:** document decisions, track limitations, test changes, and make the project understandable to contributors.

## Planned capabilities

- 2D and 3D rendering
- Scene and entity management
- Resource and asset management
- Physics, collision, and animation systems
- Audio
- Input and event handling
- Visual editor and code editor
- Debugging and profiling tools
- Scripting runtime manager and language integrations
- Plugin and extension system
- Build and export pipeline

These are project goals, not claims that the features are already implemented.

## Initial architecture direction

The engine is expected to be organized into distinct layers:

- **Core:** lifecycle, scenes, entities, resources, events, and serialization.
- **Platform layer:** operating-system services, windows, input, files, and target-specific integrations.
- **Rendering:** graphics abstraction, 2D/3D rendering, and backend implementations.
- **Engine systems:** physics, audio, animation, networking, and asset handling.
- **Scripting:** a stable engine-facing API, runtime management, and per-language adapters.
- **Editor:** visual editing, code editing, asset management, debugging, and project tools.
- **Exporter and SDK:** build pipelines, platform packaging, templates, and developer documentation.

The implementation language and low-level runtime choices are not locked yet. They will be selected after evaluating portability, performance, tooling, interoperability, and the requirements of the target platforms.

### Important scripting constraint

Supporting a language is more than detecting or installing its compiler or interpreter. Each integration needs engine API bindings, data conversion, error handling, debugging support, and a runtime that works on the intended target. Some targets—especially browsers and mobile devices—have additional sandboxing and packaging constraints. Veyra will document support per language and platform rather than promise that every combination works automatically.

## Planned project structure

The following is an initial organizational proposal and may change as implementation decisions are made.

```text
veyra-engine/
├── core/
├── platform/
├── native/
├── rendering/
│   ├── api/
│   ├── 2d/
│   ├── 3d/
│   └── backends/
├── physics/
├── audio/
├── animation/
├── networking/
├── scripting/
│   ├── api/
│   ├── runtime-manager/
│   └── languages/
├── editor/
│   ├── visual-editor/
│   ├── code-editor/
│   ├── asset-manager/
│   └── debugger/
├── platforms/
│   ├── windows/
│   ├── linux/
│   ├── android/
│   └── web/
├── exporter/
├── plugins/
├── sdk/
├── tests/
└── docs/
```

## Development roadmap

The roadmap is provisional. Work will be reordered as prototypes reveal technical constraints.

1. Architecture research and foundational project setup
2. Core runtime and platform abstraction
3. Initial desktop window, input, and rendering prototype
4. Scene, entity, and resource systems
5. 2D foundations, followed by 3D foundations
6. Physics, audio, animation, and asset workflows
7. Stable scripting API and first language integration
8. Editor foundations and debugging tools
9. Additional language integrations and plugin system
10. Windows and Linux workflows, followed by Android and Web targets
11. Export pipeline, documentation, testing, and optimization

## Project status

<sub>Early-stage project — planning and foundational implementation.</sub>

No feature should be considered complete until it has working code, tests or reproducible validation, and documented platform limitations.

## Contributing

The project is being developed incrementally. Until contribution guidelines are established, useful contributions include focused prototypes, architecture discussions, documentation improvements, reproducible bug reports, and tests.

Please discuss substantial architectural changes before building on assumptions that have not yet been accepted.

## License

A project license has not been selected yet. Until one is added, all rights are reserved by default; do not assume the code is open for reuse or redistribution.
