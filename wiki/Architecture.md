# Architecture (summary)

## High-level layout

```text
veyra-engine/
  engine/
    editor/     Qt 6 desktop editor
    runtime/    Qt-free SDL3 runtime
  docs/         Project documentation
  website/      Marketing site + browser prototypes
  installer/    Windows installer scripts
  updater/      Update manifest / trust design
  wiki/         Project wiki pages
```

## Core ideas

- **Separate editor and runtime** — the runtime must not depend on Qt.
- **Versioned scene documents** — `.vscene` JSON with stable object IDs and validation.
- **Windows x64 first** — other platforms only after their own build/install path is proven.
- **Honest verification** — CI + manual tests; compile success alone is not enough.

## Scene contract (v1)

Top-level fields:
- `format`: `"veyra-scene"`
- `formatVersion`: `1`
- `objects`: array of objects

Object fields include: `id`, `name`, `shape`, `assetPath`, `x`, `y`, `width`, `height`, `rotation`, `scaleX`, `scaleY`, `visible`.

Unsafe paths, duplicate IDs, invalid transforms, and bad JSON must be rejected.

## Rendering direction

- Runtime window: SDL3
- Planned renderer backend: **bgfx**
- First goal: draw rectangles, circles, and triangles from editor-authored scenes

Full detail: `docs/ARCHITECTURE.md`
