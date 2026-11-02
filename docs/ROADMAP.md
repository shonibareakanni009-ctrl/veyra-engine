# Veyra Engine – Roadmap (Manager-Controlled)

**Last updated:** 2026-11-02  
**Status:** Provisional – will be refined as technical constraints appear

This is the single source of truth for priorities and sequencing.  
Only the manager updates this file.

---

## Guiding Rule

We do **not** start the next stage until the current stage has working code, basic validation, and documented limitations.

---

## Phase 0 – Foundation (Current Focus)

**Goal:** Make the project real and decision-ready.

| ID   | Item                                      | Priority | Status      | Notes |
|------|-------------------------------------------|----------|-------------|-------|
| 0.1  | Lock manager roles & documentation policy | Critical | Done        | Completed 2026-11-02 |
| 0.2  | Create living status & progress tracking  | Critical | In Progress | This file + PROGRESS.md |
| 0.3  | Decide core implementation language       | Critical | Not Started | Highest technical decision |
| 0.4  | Decide initial rendering approach         | High     | Not Started | Depends on 0.3 |
| 0.5  | Create minimal project skeleton           | High     | Not Started | After 0.3 |
| 0.6  | Define first vertical slice (prototype)   | High     | Not Started | Window + clear screen or simple 2D |

**Exit criteria for Phase 0:**  
- Core language chosen and documented  
- Minimal compilable project exists  
- First tiny prototype goal is clearly defined

---

## Phase 1 – Core Runtime & Platform Abstraction

(Only starts after Phase 0 exit criteria are met)

- Core lifecycle
- Basic platform layer (window, input, files)
- Scene / Entity skeleton
- Resource system skeleton

---

## Phase 2 – First Rendering Prototype

- Desktop window
- Clear / draw something simple
- 2D foundations begin here

---

## Later Phases (Parked)

These remain in the original vision but are **not active**:

- Full 3D
- Physics, Audio, Animation
- Multi-language scripting
- Visual Editor
- Android / Web targets
- Plugin system
- Export pipeline

They will be pulled forward only when the foundation is solid.

---

## Timeline Honesty Rule

No dates will be invented.  
We track sequence and readiness, not calendar promises, until real velocity is known.

---

*Manager owns this document.*
