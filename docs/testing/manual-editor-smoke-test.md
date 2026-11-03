# Veyra Editor — Manual Windows Smoke Test

**Purpose:** Verify the packaged Windows editor outside CI before treating the Phase 1 editor milestone as fully verified.

**Target build:** `5fd5f224633cb60c3ac9272aed3e8b12c9c4dcf0`  
**Short commit:** `5fd5f22`  
**Workflow:** [Windows Editor Build #41657800158](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41657800158)  
**Artifact:** [veyra-editor-windows-x64-folder](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/artifacts/13240043601)

**Status:** **COMPLETED AND PASSED** on 2026-11-03

---

## Test Result Summary

| Section | Result | Notes |
|---------|--------|-------|
| A. Launch and create a scene | **PASS** | |
| B. Primitive objects | **PASS** | Rectangle, circle, triangle |
| C. Transform objects | **PASS** | Position, rotation, scale, width, height, drag |
| D. Sprite import & render | **PASS** | Image imported and visible |
| E. Save and reopen | **PASS** | Objects and sprite preserved |
| F. Unsaved-change protection | **PASS** | Warning shown correctly |
| G. Invalid asset path rejection | **PASS** | Scene rejected with “invalid identity or transform values” error when path `../outside.png` was used |

**Overall:** Phase 1 manual smoke test **PASSED**.

---

## Result Record

```text
Tester: Shonibare
Date/time: 2026-11-03
Windows version: (as tested by Shonibare)
Package commit: 5fd5f22
Artifact: veyra-editor-windows-x64-folder

Launch and create a scene: PASS
Primitive objects: PASS
Selection and hierarchy: PASS
Rename and delete: PASS
Transform inspector: PASS
Viewport dragging: PASS
Sprite import: PASS
Sprite rendering: PASS
Save and reopen: PASS
Unsaved-change protection: PASS
Invalid asset path rejection: PASS

Crashes or blocking defects:
none

Other observations:
- Section G produced a generic error message (“Scene contains an object with invalid identity or transform values”) rather than a specific path error, but the scene was correctly rejected and the editor remained stable.
```

---

## Completion rule (met)

The Phase 1 editor milestone is marked **Verified** because:

- The tested package launched successfully.
- Scene editing, sprite import, save/reopen, and unsaved-change behavior passed.
- Invalid asset paths were rejected safely.
- Results were recorded with sufficient detail.
