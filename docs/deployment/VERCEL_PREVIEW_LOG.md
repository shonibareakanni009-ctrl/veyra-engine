# Veyra Engine — Vercel Deployment Log

## 2026-11-02 — First browser build deployed

**Project:** veyra-engine-preview  
**Repository:** shonibareakanni009-ctrl/veyra-engine  
**Branch:** main  
**Source commit:** e04ec894736c99ee2558d750e5a02aff25abe864  
**Deployment ID:** dpl_sBGbazoS3iZoBsUjr9rYXWQ15Tzy  
**Deployment state:** READY (confirmed through the Vercel deployment API)  
**Vercel-reported target:** production  
**Primary deployment URL:** https://veyra-engine-preview.vercel.app

### What was built

Vercel cloned the GitHub repository and ran scripts/vercel-build-web.sh.

The script installed CMake and Ninja, installed the pinned Emscripten SDK 6.0.10, configured the existing CMake browser prototype, and compiled the SDL3 dependency and Veyra C++ prototype to browser deliverables.

The Vercel build log confirms:
- SDL3 static library linked.
- prototypes/browser/src/main.cpp compiled.
- veyra_browser_prototype.html linked.
- The build script checks for HTML, JavaScript, and WASM output passed.
- Vercel reported Build Completed and Deployment completed.
- The deployment API subsequently reported state READY.

### What this proves

- The browser prototype can be built from the GitHub source in Vercel's cloud build environment.
- The C++/SDL3 source compiles with the pinned Emscripten toolchain in that environment.
- Vercel accepted and completed the static asset deployment.

### Follow-up redeployment and cache correction

The deployment-log commit triggered an automatic Git-based redeployment. That second deployment failed because Vercel restored a cached .vercel-tools/emsdk directory that existed but was not a valid Git checkout. The build script attempted to clone into the non-empty directory and exited with code 128.

Correction committed as c706a60eb116f52906525c649a797baa491deb78:
- Detect an Emscripten SDK directory without its .git metadata.
- Remove only that incomplete SDK directory.
- Clone the pinned 6.0.10 SDK cleanly.
- Reuse a valid cached checkout when its Git metadata exists.

A new automatic deployment is expected from this correction commit. Its final status must be checked and recorded separately.

### What remains unverified

- A real browser interaction test has not yet been performed. READY means Vercel completed deployment; it does not prove keyboard movement, pointer/touch input, resizing, and runtime behavior in a browser.
- Vercel reports the deployment target as production, despite the project being named veyra-engine-preview. Confirm deployment protection and alias intent before treating this as a public production release.
- This is only the small browser prototype, not the complete Veyra Engine. The final bgfx renderer, editor, scene system, physics, audio, scripting runtime, and native export targets are not implemented by this milestone.

### Manager follow-up

1. Check the deployment created from the cache-correction commit.
2. Open the deployment URL and perform a browser smoke test on desktop and mobile.
3. Record runtime errors or input/rendering defects as issues.
4. Decide whether the Vercel project should remain a production-target deployment or be configured for preview-only deployments.
5. Keep this log updated as new builds are made.

**Evidence:** Vercel deployment dpl_sBGbazoS3iZoBsUjr9rYXWQ15Tzy; source commit e04ec894736c99ee2558d750e5a02aff25abe864; cache-correction commit c706a60eb116f52906525c649a797baa491deb78.
