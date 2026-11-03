# Veyra Editor — Manual Windows Smoke Test

**Purpose:** Verify the packaged Windows editor outside CI before treating the Phase 1 editor milestone as fully verified.

**Target build:** `5fd5f224633cb60c3ac9272aed3e8b12c9c4dcf0`  
**Short commit:** `5fd5f22`  
**Workflow:** [Windows Editor Build #41657800158](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/runs/41657800158)  
**Artifact:** [veyra-editor-windows-x64-folder](https://github.com/shonibareakanni009-ctrl/veyra-engine/actions/artifacts/13240043601)

This test is a manual runtime check of the packaged application. It complements, but does not replace, the automated CTest and packaging checks in GitHub Actions.

## Before testing

1. Sign in to GitHub and download the artifact linked above.
2. Extract the complete folder to a writable location.
3. Do not move `VeyraEditor.exe` away from its supporting files.
4. Confirm that the extracted folder contains the Qt `platforms` directory and the packaged MSVC runtime DLLs.
5. Record the Windows version, test date, tester, and package commit.

## Test procedure

### A. Launch and create a scene

1. Launch `VeyraEditor.exe`.
2. Confirm that the application opens without a missing-DLL, Qt-platform, or immediate-crash error.
3. Create a new scene with `Ctrl+N` or the corresponding menu action.
4. Confirm that the scene viewport, hierarchy, and inspector are visible.

### B. Create and inspect primitive objects

1. Add a rectangle.
2. Add a circle.
3. Add a triangle.
4. Confirm that each object appears in the viewport and hierarchy.
5. Select each object and confirm that the selected object is synchronized between the viewport and hierarchy.
6. Rename at least one object in the hierarchy.
7. Delete one object and confirm that it disappears from both the viewport and hierarchy.

### C. Transform objects

For a selected object, verify each operation independently:

1. Move its position.
2. Change its rotation.
3. Change its scale.
4. Change its width.
5. Change its height.
6. Confirm that the viewport reflects each inspector change.
7. Drag an object in the viewport and confirm that its position changes.

### D. Import and render a sprite

1. Use the image-import action to import a PNG, JPG, BMP, GIF, or WebP image.
2. Confirm that the image is copied into the scene-local `Assets` folder.
3. Confirm that a sprite object is created in the scene and hierarchy.
4. Confirm that the sprite is visible in the viewport.
5. Move and transform the sprite.
6. Rename the sprite object.

### E. Save and reopen

1. Save the scene as a `.vscene` file.
2. Note the file location and confirm that the scene-local `Assets` folder exists beside it as expected.
3. Make an additional visible edit and save again.
4. Close the editor.
5. Reopen the editor and open the saved `.vscene` file.
6. Confirm that all remaining objects, names, transforms, primitive shapes, and sprite references are preserved.
7. Confirm that the imported sprite is still rendered after reopening.

### F. Unsaved-change protection

1. Open or create a scene.
2. Make a visible change without saving.
3. Attempt to create a new scene, open another scene, or close the editor.
4. Confirm that the editor warns about unsaved changes.
5. Confirm that choosing cancel keeps the modified scene open.
6. If the dialog offers save/discard choices, verify that save preserves the change and discard does not silently write it.

### G. Invalid and unsafe asset paths

Use a disposable test scene or test copy for this section.

1. Attempt to load a sprite reference with a path that leaves the scene asset directory, such as `../outside.png`.
2. Attempt a Windows-style traversal path, such as `..\\outside.png`.
3. Attempt an absolute path outside the project or scene asset directory.
4. Confirm that invalid references are rejected safely.
5. Confirm that no file outside the intended asset directory is copied, overwritten, or loaded as a valid scene asset.
6. Confirm that the editor remains usable after rejection.

## Result record

Copy this section into the result issue or test report and replace each placeholder.

```text
Tester: <name>
Date/time: <local date and time>
Windows version: <version>
Package commit: 5fd5f22
Artifact: veyra-editor-windows-x64-folder

Launch and create a scene: PASS / FAIL
Primitive objects: PASS / FAIL
Selection and hierarchy: PASS / FAIL
Rename and delete: PASS / FAIL
Transform inspector: PASS / FAIL
Viewport dragging: PASS / FAIL
Sprite import: PASS / FAIL
Sprite rendering: PASS / FAIL
Save and reopen: PASS / FAIL
Unsaved-change protection: PASS / FAIL
Invalid asset path rejection: PASS / FAIL

Crashes or blocking defects:
<none or detailed description>

Other observations:
<notes>
```

## Completion rule

The Phase 1 editor milestone can be marked **Verified** only when:

- The tested package launches successfully.
- Scene editing, sprite import, save/reopen, and unsaved-change behavior pass.
- Invalid asset paths are rejected safely.
- Any failure is recorded with enough detail to reproduce it.

A successful automated build alone is not sufficient evidence for this milestone.
