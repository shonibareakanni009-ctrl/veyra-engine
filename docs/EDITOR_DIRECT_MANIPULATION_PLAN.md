# Editor Direct Manipulation and Inspector Layout — Implementation Plan

**Status:** Plan committed before implementation.
**Branch:** `main`
**Scope:** Qt editor only; preserve the version-1 `.vscene` format and existing document ownership model.

## Requested behavior

1. Select a scene object and move it by dragging inside its visible shape.
2. Show resize handles around the selected object's transformed bounds.
3. Drag an edge or corner handle to resize the object directly in the viewport.
4. Keep the Inspector synchronized with viewport edits and allow numeric edits to update the viewport.
5. Move the Inspector from the right dock to a horizontal dock directly below the File/Edit/View menu and toolbar area, keeping it dockable/floatable.
6. Preserve hierarchy selection synchronization and scene dirty-state behavior.

## Implementation sequence

1. Extend `SceneViewport` with screen-space handle hit testing, local-space resize calculations, cursor feedback, and selection-bound/handle painting.
2. Keep object movement behavior intact; ensure resize gestures take priority over move gestures.
3. Change the Inspector form to a compact horizontal layout and dock it in the top dock area beneath the menu/toolbar.
4. Add/adjust automated tests where the current test architecture permits; at minimum compile and run the existing Windows Editor Build and scene tests.
5. Update `docs/PROGRESS.md` with exact verification results and commit/push the implementation.

## Interaction rules

- Eight handles: four corners and four edge midpoints.
- Resize changes the object's authored width/height, not its scale fields.
- Minimum authored dimension: 10 world units.
- The resize anchor is the opposite side/edge; rotation and non-uniform scale are respected by converting pointer coordinates into the object's local space.
- Dragging inside the object continues to move it.
- Clicking empty space clears selection.
- Inspector edits remain the precise alternative to dragging.

## Acceptance

- A selected rectangle, circle, triangle, and sprite show resize handles.
- Dragging the center moves the object.
- Dragging any handle changes width/height and the Inspector values update live.
- Numeric Inspector edits update the scene immediately.
- Inspector appears in the top workspace row under the menus/toolbars and can still be moved/floated.
- Existing scene files remain compatible.
- Windows Editor Build and relevant tests pass. GUI visual acceptance is recorded separately from CI.

## Out of scope

- Multi-object selection, snapping, aspect-ratio lock, rotation gizmo, undo/redo, and runtime rendering changes.
