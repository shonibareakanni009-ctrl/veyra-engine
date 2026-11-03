# ADR-002: Separate the Public Website from the Engine Runtime

- **Status:** Accepted
- **Date:** 2026-11-02
- **Decision owner:** Akanni

## Context

The earlier browser-first SDL3/WebAssembly experiment was a small application-lifecycle smoke test. It was not the Veyra editor, did not integrate the engine's scene workflow, and had unresolved browser runtime validation. Maintaining it as the deployed root site mixed a temporary technical prototype with the public identity of the project.

## Decision

1. The repository's Vercel deployment serves the official Veyra Engine website from the website/ directory.
2. The website is a static HTML/CSS/JavaScript site with a clear project overview and honest implementation status.
3. Veyra Adventure is a separate standalone HTML Canvas browser game under website/game/.
4. The browser game is a demonstration project, not yet a game built or exported by the Veyra engine.
5. Remove the old SDL3/Emscripten browser prototype, its dedicated GitHub Actions workflow, and its Emscripten-based Vercel build pipeline.
6. Browser games and web export remain part of Veyra's long-term platform vision. Removing the temporary prototype does not remove the Web target.
7. The native Qt desktop editor remains a separate application and continues to be built through its Windows workflow.

## Consequences

- Vercel builds the static site without installing Emscripten or compiling C++.
- The website output directory remains build/vercel-browser/bin to match the existing Vercel project configuration.
- The standalone game can be tested independently in modern browsers and on touch devices.
- Future browser export from Veyra must be implemented as a genuine engine target and must not be confused with this website demo.
- Build success alone does not establish browser compatibility; the game still requires manual testing across desktop and mobile browsers.

## Validation

- The website build script checks that the homepage and game files exist and copies them into the configured deployment output directory.
- The game should be manually tested for keyboard controls, touch controls, jumping, collectibles, enemy collisions, pause/restart, and the win condition.
