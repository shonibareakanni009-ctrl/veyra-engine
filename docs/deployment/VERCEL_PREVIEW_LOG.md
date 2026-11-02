# Veyra Engine — Vercel Deployment Log

## 2026-11-02 — First browser build deployed

**Project:** veyra-engine-preview  
**Repository:** shonibareakanni009-ctrl/veyra-engine  
**Branch:** main  
**Initial source commit:** e04ec894736c99ee2558d750e5a02aff25abe864  
**Initial deployment ID:** dpl_sBGbazoS3iZoBsUjr9rYXWQ15Tzy  
**Initial deployment state:** READY  
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

### Follow-up redeployment and cache correction

The deployment-log commit triggered an automatic Git-based redeployment. That deployment failed because Vercel restored a cached .vercel-tools/emsdk directory that existed but was not a valid Git checkout. The build script attempted to clone into the non-empty directory and exited with code 128.

Correction committed as c706a60eb116f52906525c649a797baa491deb78:
- Detect an Emscripten SDK directory without its .git metadata.
- Remove only that incomplete SDK directory.
- Clone the pinned 6.0.10 SDK cleanly.
- Reuse a valid cached checkout when its Git metadata exists.

**Recovery deployment:** dpl_F5gzJN5tPLYRDCk1cU3QXPyQuwPP  
**Source commit:** c706a60eb116f52906525c649a797baa491deb78  
**Deployment state:** READY (confirmed through Vercel deployment API)  
**Vercel-reported target:** production  
**Deployment URL:** https://veyra-engine-preview-nrvnzt286.vercel.app  
**Production alias:** https://veyra-engine-preview.vercel.app

Recovery build evidence:
- The incomplete cached SDK folder was detected and removed.
- Emscripten 6.0.10 was freshly installed.
- SDL3 static library linked.
- Veyra C++ source main.cpp compiled.
- HTML target linked at build step 216/217.
- The build script's HTML, JavaScript, and WASM output checks passed.
- Vercel reported Build Completed and Deployment completed.
- Vercel deployment API reported READY and assigned the production alias.

### What this proves

- The browser prototype can be built from GitHub source in Vercel's cloud build environment.
- The C++/SDL3 source compiles with the pinned Emscripten toolchain in that environment.
- Vercel accepted and completed the static asset deployment, and the recovery deployment is READY.

### What remains unverified

- A real browser interaction test has not yet been performed. READY means Vercel completed deployment; it does not prove keyboard movement, pointer/touch input, resizing, or runtime behavior. An automated page-open attempt from the available verification browser was inaccessible, so HTTP/browser rendering is not independently confirmed here.
- Vercel reports the deployment target as production. Confirm deployment protection and alias intent before treating this as a public production release.
- This is only the small browser prototype, not the complete Veyra Engine. The final bgfx renderer, editor, scene system, physics, audio, scripting runtime, and native export targets are not implemented by this milestone.

### Manager follow-up

1. Open https://veyra-engine-preview.vercel.app in a normal browser and perform a smoke test.
2. Test keyboard movement, pointer/touch repositioning, resize/letterboxing, and browser console errors.
3. Record runtime errors or input/rendering defects as issues.
4. Decide whether the Vercel project should remain a production-target deployment or be configured for preview-only deployments.
5. Keep this log updated as new builds are made.

**Evidence:** Initial deployment dpl_sBGbazoS3iZoBsUjr9rYXWQ15Tzy; recovery deployment dpl_F5gzJN5tPLYRDCk1cU3QXPyQuwPP; recovery source commit c706a60eb116f52906525c649a797baa491deb78.
