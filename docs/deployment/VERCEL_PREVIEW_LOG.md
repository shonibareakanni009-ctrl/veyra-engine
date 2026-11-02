# Veyra Engine — Vercel Deployment Log

## 2026-11-02 — First browser build deployed

**Project:** \`veyra-engine-preview\`  
**Repository:** \`shonibareakanni009-ctrl/veyra-engine\`  
**Branch:** \`main\`  
**Commit:** \`e04ec894736c99ee2558d750e5a02aff25abe864\`  
**Deployment ID:** \`dpl_sBGbazoS3iZoBsUjr9rYXWQ15Tzy\`  
**Deployment state:** \`READY\` (confirmed through the Vercel deployment API)  
**Vercel-reported target:** \`production\`  
**Primary deployment URL:** https://veyra-engine-preview.vercel.app  
**Project preview name:** \`veyra-engine-preview\`

### What was built

Vercel cloned the GitHub repository and ran \`scripts/vercel-build-web.sh\`.

The script installed CMake and Ninja, installed the pinned Emscripten SDK 6.0.10, configured the existing CMake browser prototype, and compiled the SDL3 dependency and Veyra C++ prototype to browser deliverables.

The Vercel build log confirms:
- SDL3 static library linked.
- \`src/main.cpp\` compiled.
- \`veyra_browser_prototype.html\` linked.
- The build script's checks for HTML, JavaScript, and WASM output passed.
- Vercel reported \`Build Completed\` and \`Deployment completed\`.
- The deployment API subsequently reported state \`READY\`.

### What this proves

- The browser prototype can be built from the GitHub source in Vercel's cloud build environment.
- The C++/SDL3 source compiles with the pinned Emscripten toolchain in that environment.
- Vercel accepted and completed the static asset deployment.

### What remains unverified

- A real browser interaction test has not yet been performed. The deployment API reports READY, but that is not the same as manually verifying keyboard movement, pointer/touch input, resizing, and runtime behavior in a browser.
- The deployment API reports target \`production\`, despite the project being named \`veyra-engine-preview\`. Confirm deployment protection/alias intent before treating this as a public production release.
- This is only the small browser prototype, not the complete Veyra Engine. The final bgfx renderer, editor, scene system, physics, audio, scripting runtime, and native export targets are not implemented by this milestone.

### Manager follow-up

1. Open the deployment URL and perform a browser smoke test on desktop and mobile.
2. Record any runtime errors or input/rendering defects as issues.
3. Decide whether the Vercel project should remain a production-target deployment or be configured for preview-only deployments.
4. Keep the deployment log updated as new builds are made.

**Evidence:** Vercel deployment \`dpl_sBGbazoS3iZoBsUjr9rYXWQ15Tzy\`; GitHub source commit \`e04ec894736c99ee2558d750e5a02aff25abe864\`.
