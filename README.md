# Veyra Engine

**An independent cross-platform 2D and 3D game engine project.**

Veyra is being built around creative freedom, a visual desktop editor, a code-oriented workflow, and platform choice. The project is in active early development. This repository contains the engine/editor source, project documentation, the official website, and a separate browser game demo.

## Explore Veyra

- **Official website:** deployed through the connected Vercel project.
- **Veyra Adventure:** website/game/, a small playable HTML Canvas platformer used as a browser-facing demonstration.
- **Veyra SFS:** website/sfs/, a standalone 2D browser flight simulator prototype used to explore thrust, momentum, planetary gravity, fuel, and camera-follow behavior.
- **Desktop editor:** editor/, a Qt 6 Widgets application with an early editable 2D scene workflow.
- **Engine foundation:** C++20, CMake, and the architecture documented below.

The Adventure demo is currently a standalone browser game. It is not yet built by the Veyra engine or exported from the desktop editor. It demonstrates the kind of 2D experience Veyra ultimately aims to help creators make.

## Current implementation status

### Desktop editor (Phase 1 — Verified)
The editor includes a 2D scene document, rectangle/circle/triangle primitives, image sprite import (PNG, JPG, BMP, GIF, WebP), scene-local Assets storage, viewport sprite rendering, selection and dragging, a hierarchy, transform inspector, and .vscene save/load.  

**Status as of 2026-11-03:**  
- Windows CI is green (compile + tests + packaging)  
- Full manual smoke test passed (launch, editing, sprites, save/reopen, unsaved-change protection, invalid path rejection)  
- Phase 1 is now marked **Verified**

Arbitrary custom polygons and a searchable/thumbnail asset browser are not implemented yet. The native runtime is not yet implemented.

### Browser website and game
The website is a static HTML/CSS site. Veyra SFS is a separate HTML Canvas 2D flight prototype with keyboard/touch controls, thrust, braking, gravity, fuel, a following camera, and an orbital beacon objective. It is not an engine export. Veyra Adventure is an HTML Canvas game with keyboard and touch controls, platforms, collectibles, patrolling enemies, lives, a timer, a portal goal, pause, and restart. It is separate from the native engine runtime.

### Planned engine capabilities
2D/3D rendering, asset management, physics, collision, animation, audio, scripting, project management, debugging, exporting, plugins, and native platform support remain staged engineering goals unless explicitly marked implemented and tested.

## Technology direction

- C++20 engine core
- CMake build system
- SDL3 platform/lifecycle layer
- bgfx long-term rendering backend
- Qt 6 Widgets desktop editor
- Lua 5.5 first planned scripting integration
- Web delivery for the public website and browser demonstrations

See docs/TECH_STACK.md, docs/ARCHITECTURE.md, docs/ROADMAP.md, and [docs/INSTALLER_UPDATER.md](docs/INSTALLER_UPDATER.md) for the Phase 2 distribution plan.

## Repository layout

```text
veyra-engine/
├── editor/                 # Native Qt desktop editor
├── website/                # Official website and browser demo
│   ├── game/               # Standalone Veyra Adventure game
│   └── sfs/                # Standalone 2D Space Flight Simulator browser prototype
├── scripts/                # Website deployment build
├── docs/                   # Architecture, decisions, and project notes
└── .github/workflows/      # Native editor CI
```

## Website deployment

The connected Vercel project should use:

- Build command: bash scripts/vercel-build-web.sh
- Output directory: build/vercel-browser/bin
- Root document: index.html

The build script copies the static website into the configured output folder and validates the required game files. No Emscripten or C++ WebAssembly compilation is required for the website deployment.

## Development principles

1. Keep user-created projects separate from engine installation and updates.
2. Document ownership, data flow, and important design decisions.
3. Verify builds and runtime behavior before claiming platform support.
4. Keep the browser demo distinct from the actual engine until it is genuinely integrated.
5. Preserve the long-term cross-platform vision while implementing one stable milestone at a time.

## License

A project license has not been selected yet. All rights are reserved by default; do not assume the code is open for reuse or redistribution.
