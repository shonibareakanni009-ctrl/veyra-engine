# Veyra Engine — 2D Test Projects & Feature Validation Plan

**Status:** Test plan  
**Scope:** Validate the 2D editor, scene format, runtime, and platform pipeline before expanding into advanced 2D and 3D workflows.

## 1. Core test games

These are the first two practical validation projects.

### 1.1 Veyra Adventure — existing browser game

- **Source:** website/game/index.html, with behavior in website/game/game.js and styling in website/game/game.css.
- **Purpose:** Use the existing browser game as a known baseline for the intended 2D gameplay workflow.
- **Important limitation:** Veyra Adventure is currently a standalone HTML Canvas game. It is not built by the Veyra C++ engine and is not an export from the desktop editor.
- **Validation:** Inventory its confirmed mechanics, reproduce them in the engine/runtime when those systems exist, compare behavior, and record gaps. Do not claim engine compatibility merely because the standalone browser game runs.

### 1.2 Space Flight Simulator

- **Browser prototype:** `website/sfs/index.html` (entry), `website/sfs/sfs.js` (simulation), and `website/sfs/sfs.css` (interface).
- **Purpose:** A new test project focused on movement and simulation in a 2D space environment.
- **Feature coverage:** Player/ship controls, acceleration and deceleration, rotation, camera following, object spawning, collision handling, projectiles or interaction objects where required, HUD elements, and saving/loading.
- **Current browser prototype:** 2D spacecraft rotation, thrust, braking, planetary gravity, fuel, speed/altitude HUD, camera follow, orbital beacon objective, pause/restart, and touch controls. It is a standalone browser prototype, not an engine export.
- **Validation:** Test keyboard and touch input, consistent movement across frame rates, gravity/orbit tuning, mission completion/failure, scene editing, native runtime behavior, and persistence.

## 2. Additional test projects

### 2.1 Top-Down Adventure

Tests character movement, collision, interaction prompts, simple AI/pathfinding, combat, inventory, dialogue, and scene transitions.

### 2.2 Puzzle Game

Tests triggers, switches, doors, object interactions, logic conditions, timed events, level transitions, and save states.

### 2.3 Racing Game

Tests vehicle movement, acceleration, friction, steering, collision response, checkpoints, lap timing, and camera behavior.

### 2.4 Simulation Game

Tests time progression, resource management, inventory, object placement, complex UI, and persistence of a larger set of world objects.

### 2.5 Multiplayer Arena

A later-stage project for testing networking, player synchronization, latency handling, and server communication. This is not an initial release gate.

## 3. Feature validation matrix

| Engine area | What must be tested |
|---|---|
| Scene editor | Create, open, edit, save, close, and reopen scenes; create, select, move, rotate, scale, resize, rename, and delete objects |
| Rendering | Sprites, textures, layers, draw order, visibility, and camera |
| Physics | Gravity, forces, collision detection, and collision response |
| Input | Keyboard, mouse, touch, and controller support where implemented |
| Audio | Music, sound effects, playback controls, and volume |
| Animation | Sprite/frame animation, timing, and transitions |
| AI | Basic state behavior, navigation/pathfinding, and responsiveness |
| UI | HUD, menus, buttons, text, and layout |
| Asset management | Import, organize, reference, and reload assets |
| Scripting | Lua integration, script execution, and safe error reporting |
| Camera | Follow behavior, movement, zoom, and scene bounds |
| Particles | Emitters, lifetime, movement, and rendering |
| Save system | Save/load game state and preserve project data |
| Networking | Synchronization and latency behavior when multiplayer support is implemented |
| Debugging/profiling | Logs, error reporting, frame timing, and performance inspection |
| Exporting | Windows, Linux, Android, and Web builds, as each target becomes available |

A feature is not complete merely because its UI exists. It must work in a real test project and pass regression checks.

## 4. Test levels and release gates

### Level 1 — Core validation

- Veyra Adventure as a standalone browser reference project.
- Space Flight Simulator as the first new engine-authored test project.
- Functional 2D scene editing.
- Create, select, move, rotate, scale, rename, and delete objects.
- Inspector updates and scene hierarchy synchronization.
- Save and reopen scenes without losing edits.
- Load and run an edited scene in the engine runtime.

### Level 2 — Broader 2D validation

- Top-Down Adventure.
- Puzzle Game.
- Racing Game.
- Validate animation, physics, UI, audio, assets, and scripting in combination.

### Level 3 — Advanced validation

- Simulation Game.
- Multiplayer Arena.
- Future 3D projects and workflows.

## 5. Required update and verification process

Every engine update should follow this sequence:

1. **Plan:** Define the feature, expected behavior, dependencies, and acceptance criteria.
2. **Implement:** Make the actual code and editor changes; avoid placeholder-only implementations.
3. **Integrate:** Connect the feature to the editor, scene format, runtime, and relevant systems.
4. **Automated tests:** Build and run unit, integration, and regression tests where available.
5. **Game testing:** Exercise the feature in the relevant test project(s).
6. **Regression testing:** Confirm existing features and the core test projects still work.
7. **Documentation:** Update user and developer documentation, including known limitations.
8. **Completion review:** Mark the feature complete only after the implementation and verification evidence are available.

For each feature, track its status as **Planned**, **In progress**, **Implemented**, or **Verified**. Do not label a feature Verified based only on a successful compile.

## 6. Initial implementation priorities

1. Make the Windows editor folder build and package reproducibly.
2. Test scene serialization and protect unsaved document changes.
3. Manually verify the editor can create, transform, rename, delete, save, and reopen objects.
4. Connect saved scenes to a separate runtime so they can be played.
5. Keep the browser SFS prototype as a separate early gameplay test while the native scene/runtime workflow is developed.

## 7. Accuracy and scope rules

- Veyra Adventure is identified from the repository README and website/game sources.
- Veyra Adventure remains a standalone browser game until it is genuinely integrated with the engine.
- Do not invent test-game titles, mechanics, screenshots, or test results.
- Clearly distinguish planned, implemented, and verified functionality.
- Do not ask the user to download an editor build until the scene editor can genuinely edit and save scenes and the build has been tested.
