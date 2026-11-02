# Veyra Engine — Coding and Commenting Standards

**Status:** Required project standard  
**Last updated:** 2026-11-02  
**Applies to:** Engine code, editor code, platform adapters, scripts, build files, tests, and examples

## 1. Commenting philosophy

Veyra will use a deliberately comment-heavy coding style during early development.

The goal is to make the project understandable to its owner, future contributors, and future maintainers—even when they encounter a file months after it was written.

Comments are part of the engineering deliverable. A feature is not considered well documented merely because the code compiles.

**Default expectation:** explain purpose, context, assumptions, ownership, lifecycle, side effects, limitations, and non-obvious reasoning.

Do not add meaningless comments that only repeat what a line visibly does. When a line is already obvious, explain the surrounding rule or reason instead.

## 2. Required comments by file type

### C++ source files

Every C++ source file must begin with a file-level documentation block that explains:
- What the file implements.
- Which subsystem owns it.
- Its main responsibilities.
- Important dependencies.
- Initialization/shutdown involvement, if any.
- Threading expectations.
- Memory/resource ownership expectations.
- Known platform-specific behavior.

Example:

    /**
     * @file engine_clock.cpp
     * @brief Implements the engine's monotonic time service.
     *
     * @details
     * This file provides the implementation behind the engine clock interface.
     * The clock is used for frame timing and elapsed-time measurements; it must
     * not be used as a source of wall-clock calendar time.
     *
     * Ownership:
     * - The core runtime owns the clock service.
     * - Callers receive elapsed-time values and do not own the clock.
     *
     * Threading:
     * - The implementation must document whether reads are safe from worker
     *   threads and whether state mutation is restricted to the main thread.
     *
     * Platform notes:
     * - Platform-specific timer selection belongs behind the platform adapter.
     */

### C++ header files

Every public header must explain:
- The public purpose of the interface.
- The module that owns it.
- What callers may rely on.
- What callers must not assume.
- Ownership and lifetime rules.
- Thread-safety guarantees or restrictions.
- Error behavior.
- Platform limitations, if any.

Header comments are the contract other modules read before using an implementation.

## 3. Required documentation for declarations

Use Doxygen-style comments for public C++ APIs.

Document every public:
- Class, struct, enum, and enum value.
- Function and method.
- Constructor and destructor when they have meaningful behavior or ownership implications.
- Public data member that is not purely obvious.
- Constant or configuration value whose meaning, unit, range, or compatibility effect is not obvious.
- Callback and event contract.
- C ABI function and ABI-visible structure.

For a public function, document:
- What it does.
- Preconditions.
- Parameters and units.
- Return value.
- Error behavior.
- Side effects.
- Ownership/lifetime effects.
- Thread-safety.
- Whether it may block or allocate.
- Platform-specific limitations.

Example:

    /**
     * @brief Loads a scene resource and makes it available to the runtime.
     *
     * @param scene_path Project-relative path of the scene resource.
     * @return A valid scene handle on success; an invalid handle on failure.
     *
     * @details
     * Loading may perform file I/O and resource creation. Callers must not
     * assume the returned handle remains valid after the owning project closes.
     *
     * @note This function does not automatically activate the scene.
     * @warning Do not call from a real-time audio callback.
     * @thread_safety Main-thread only until asynchronous loading is implemented.
     */
    SceneHandle load_scene(const ProjectPath& scene_path);

The example is illustrative. Do not add a declaration that the engine has not implemented.

## 4. Comments inside implementations

Use comments to explain:
- Why an algorithm or data structure was selected.
- Invariants that must remain true.
- Ordering requirements.
- Resource ownership and release requirements.
- Coordinate systems, units, color spaces, and matrix conventions.
- Thread-affinity or synchronization requirements.
- Callback re-entrancy assumptions.
- Cache invalidation rules.
- Numerical precision or tolerance decisions.
- Platform workarounds and the reason they exist.
- Security validation and trust boundaries.
- Important failure and recovery paths.
- Performance trade-offs that are not obvious from the code.

Add comments before complex blocks, not just after them. Explain the reasoning before a maintainer has to infer it.

Example:

    // Keep resource destruction on the render thread. The graphics backend may
    // still have queued commands that reference this resource, so releasing it
    // from an arbitrary worker thread could race with command submission.
    enqueue_render_thread_release(resource_handle);

## 5. Ownership and lifetime

Whenever code handles a resource, pointer, handle, view, callback, task, or reference crossing a module boundary, its documentation must explain:
- Who creates it.
- Who owns it.
- Who is allowed to use it.
- How long it remains valid.
- Who releases it.
- What happens if the owner is destroyed.
- Whether use is allowed from multiple threads.

Prefer RAII and clear ownership types in C++. Comments must describe the ownership contract even when RAII makes the normal case safe.

## 6. Platform and graphics code

Platform-specific code must state:
- Which operating systems or targets it applies to.
- Which SDK/API requirement it relies on.
- Why a platform branch is necessary.
- What behavior the generic interface promises.
- What behavior cannot be provided on that target.

Rendering code must document:
- Coordinate handedness.
- Coordinate and screen-space conventions.
- Matrix multiplication order.
- Depth range and clip-space assumptions.
- Texture origin and sampling assumptions.
- Color-space expectations.
- Resource state and synchronization rules.

Do not make other modules infer these conventions from a shader or graphics API call.

## 7. Scripting and ABI code

The language bridge is a high-risk boundary. Its comments must explain:
- Which runtime owns each value.
- Whether a value is copied, borrowed, pinned, or referenced.
- How engine objects are represented to the language.
- How stale handles are detected.
- How exceptions/errors are translated.
- Whether callbacks can re-enter the engine.
- Which thread is allowed to invoke a runtime.
- What happens during script reload or runtime shutdown.

C-compatible API comments must document ABI versioning, structure size, ownership, calling convention, and error values. Never expose an undocumented C++ layout as a cross-language interface.

## 8. Build and configuration files

Every CMake file and build configuration file must explain:
- What targets it creates.
- Why dependencies are included.
- What each non-obvious option changes.
- Why platform-specific branches exist.
- Which files are generated and where they are placed.
- How build configuration affects runtime behavior.

Do not leave unexplained compiler flags, linker flags, macros, environment variables, toolchain overrides, or custom build steps.

## 9. Tests, scripts, and examples

Each test file must describe the behavior area being tested. Each non-trivial test must explain:
- The scenario being reproduced.
- The expected behavior.
- Setup assumptions.
- Whether it protects against a previously fixed regression.

Each script and example must include a short purpose comment and document any engine services, resources, or lifecycle events it depends on.

A descriptive test name is still required. Comments do not excuse vague test names.

## 10. Comment format and tags

Use these tags consistently:
- TODO: work intentionally not completed.
- FIXME: known defect requiring correction.
- NOTE: important context or a subtle rule.
- WARNING: likely misuse or dangerous consequence.
- IMPORTANT: invariant or compatibility requirement.
- PORTABILITY: platform-specific constraint.
- PERFORMANCE: measured or reasoned performance consideration.
- SECURITY: trust-boundary or validation requirement.

When a task tracker or issue exists, include its identifier:

    // TODO [SCRIPTING-014]: Add cancellation support after runtime ownership
    // rules are finalized. Until then, the operation is synchronous.

Do not invent issue numbers. If no issue exists, use a clear descriptive TODO without an ID.

## 11. Comments must stay accurate

A stale comment can be more harmful than no comment.

When changing code:
1. Read the comments around the code being changed.
2. Update any comment whose promise or explanation is no longer accurate.
3. Remove obsolete workarounds and outdated TODOs.
4. Update examples and documentation affected by the change.
5. Keep comments synchronized with actual behavior and tests.

Do not write comments that claim a feature is implemented when it is only planned.

## 12. How much commenting is expected?

During the early project stage, use more documentation than a typical mature codebase:
- Every source/header file: file-level explanation.
- Every public declaration: API documentation.
- Every non-trivial private declaration: purpose and contract.
- Every complex function: a short overview of its steps and important invariants.
- Every non-obvious block: explanation of why it exists.
- Every platform branch: explanation of platform context and limitation.
- Every ownership/threading boundary: explicit contract.
- Every build configuration section: reason for its settings.
- Every test scenario: expected behavior and purpose.

Do not mechanically comment every brace, semicolon, trivial getter, or obvious assignment. The target is maximum useful explanation, not maximum comment characters.

## 13. Style and maintainability

- Prefer descriptive names over abbreviations.
- Keep functions focused and avoid deeply nested control flow.
- Separate interface declarations from implementation where useful.
- Avoid unexplained magic numbers; name and document meaningful constants.
- Use explicit units in names or documentation (for example, delta_seconds).
- Avoid comments that apologize for confusing code; simplify the code where possible.
- Do not hide a design flaw behind a long comment.
- Never copy third-party comments and claim them as Veyra documentation.
- Preserve third-party license and notice files as required.

Formatting and static-analysis tools will be configured when the first compilable C++ skeleton is added. Tool configurations themselves must be documented.

## 14. Definition of done for code

A code change is not ready merely because source files were created.

Before marking a code task complete:
- The implementation matches the documented behavior.
- Public APIs are commented.
- Ownership and lifetime rules are clear.
- Threading assumptions are stated.
- Platform-specific behavior is documented.
- Relevant tests pass or the exact testing limitation is recorded.
- Comments reflect the current implementation.
- No planned feature is described as working without evidence.

**Project rule:** Veyra code should teach its reader how and why it works, not force the reader to reverse-engineer every decision.
