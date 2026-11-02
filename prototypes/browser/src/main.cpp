/**
 * @file main.cpp
 * @brief First runnable application prototype for Veyra Engine.
 *
 * @details
 * This file is a small technical experiment, not the finished Veyra runtime.
 * It checks that the selected C++20 and SDL3 foundation can create a window,
 * receive input, update a little application state, and render a frame.
 *
 * The same source is intended to compile for:
 * - A desktop host, for example Windows 10/11 or Linux.
 * - A browser through Emscripten and WebAssembly.
 *
 * Why the SDL application callbacks are used:
 * A browser must regain control between frames so it can process page events,
 * input, layout, and drawing. SDL's callback model lets SDL drive the frame
 * lifecycle instead of keeping the browser trapped inside a permanent while
 * loop. This also gives Veyra one application-lifecycle shape to build on.
 *
 * Rendering status:
 * SDL_Renderer is used only to make the first browser smoke test small.
 * It is not a change to the selected long-term renderer. Veyra's planned
 * rendering backend remains bgfx, which will be integrated and validated in
 * a later rendering prototype.
 *
 * Ownership:
 * SDL creates and owns the window and renderer objects. This file stores
 * their pointers, uses them while the application is alive, and destroys
 * them in SDL_AppQuit. The static application state itself is not allocated
 * dynamically and remains alive for the full process lifetime.
 *
 * Threading:
 * This first prototype starts no worker threads. All application state is
 * intended to be used by the SDL application callbacks only. Any future
 * worker-thread access must introduce an explicit synchronization contract.
 *
 * Platform notes:
 * Browser builds run inside the browser sandbox. They are not native
 * executables and must be served over HTTP or HTTPS so the browser can load
 * the generated JavaScript and WebAssembly files.
 */

#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <cmath>

namespace
{
    // The first prototype uses a fixed logical coordinate system.
    //
    // SDL scales this 960x540 scene into the actual window or browser canvas.
    // This gives the game code one stable coordinate system even when the
    // browser window changes size or has a different aspect ratio.
    constexpr int kLogicalWidth = 960;
    constexpr int kLogicalHeight = 540;

    // The player is represented by a square because this prototype does not
    // yet include Veyra's texture, sprite, font, or asset-loading systems.
    constexpr float kPlayerSize = 48.0F;

    // Movement speed is expressed in logical pixels per second, not pixels
    // per frame. Using elapsed time makes movement less dependent on frame
    // rate and browser refresh rate.
    constexpr float kPlayerSpeed = 300.0F;

    // If a browser tab is inactive and later resumes, the elapsed time can be
    // unusually large. Clamping the frame delta prevents one long pause from
    // moving the player across the whole scene in a single update.
    constexpr float kMaximumFrameDeltaSeconds = 0.05F;

    /**
     * @brief Holds the state needed by this first prototype.
     *
     * @details
     * SDL passes the pointer supplied by SDL_AppInit back to the other
     * application callbacks. Keeping the state in one structure makes the
     * lifetime and ownership of the prototype's data easy to inspect.
     *
     * The window and renderer are SDL-owned resources represented here by
     * non-owning pointers. This application is responsible for calling the
     * matching SDL destroy functions before the application exits.
     */
    struct AppState
    {
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        float player_x = (static_cast<float>(kLogicalWidth) - kPlayerSize) / 2.0F;
        float player_y = (static_cast<float>(kLogicalHeight) - kPlayerSize) / 2.0F;

        Uint64 previous_frame_ticks = 0;
    };

    // Static storage gives the application state a lifetime that safely
    // covers every SDL callback. SDL receives its address; SDL does not own it.
    AppState g_app_state{};

    /**
     * @brief Move the player so its centre is at a logical scene position.
     *
     * @param state Application state whose player position will be changed.
     * @param logical_x Horizontal position in Veyra's 960-pixel logical space.
     * @param logical_y Vertical position in Veyra's 540-pixel logical space.
     *
     * @details
     * Mouse and touch input are converted to the same logical coordinate
     * system before reaching this helper. Keeping the conversion in one
     * function avoids separate movement rules for different input devices.
     */
    void move_player_to(AppState& state, float logical_x, float logical_y)
    {
        const float half_player_size = kPlayerSize / 2.0F;

        state.player_x = logical_x - half_player_size;
        state.player_y = logical_y - half_player_size;

        // Keep the complete square inside the visible logical scene.
        const float maximum_x = static_cast<float>(kLogicalWidth) - kPlayerSize;
        const float maximum_y = static_cast<float>(kLogicalHeight) - kPlayerSize;

        state.player_x = std::clamp(state.player_x, 0.0F, maximum_x);
        state.player_y = std::clamp(state.player_y, 0.0F, maximum_y);
    }

    /**
     * @brief Update player movement from the currently held keyboard keys.
     *
     * @param state Application state to update.
     * @param delta_seconds Time elapsed since the previous frame, in seconds.
     *
     * @details
     * SDL exposes the current keyboard state as an array indexed by physical
     * key scancodes. This is useful for continuous movement: a key can remain
     * held across many frames without requiring us to store every key event.
     *
     * Diagonal movement is normalized so holding two directions at once does
     * not make the player move faster than moving in one direction.
     */
    void update_player_from_keyboard(AppState& state, float delta_seconds)
    {
        const bool* keyboard_state = SDL_GetKeyboardState(nullptr);

        if (keyboard_state == nullptr)
        {
            return;
        }

        float direction_x = 0.0F;
        float direction_y = 0.0F;

        if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT])
        {
            direction_x -= 1.0F;
        }

        if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT])
        {
            direction_x += 1.0F;
        }

        if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP])
        {
            direction_y -= 1.0F;
        }

        if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN])
        {
            direction_y += 1.0F;
        }

        const float direction_length = std::sqrt(
            (direction_x * direction_x) + (direction_y * direction_y)
        );

        if (direction_length > 0.0F)
        {
            direction_x /= direction_length;
            direction_y /= direction_length;
        }

        state.player_x += direction_x * kPlayerSpeed * delta_seconds;
        state.player_y += direction_y * kPlayerSpeed * delta_seconds;

        // Clamp after keyboard movement as well as pointer movement. The
        // player should never disappear outside the logical scene.
        const float maximum_x = static_cast<float>(kLogicalWidth) - kPlayerSize;
        const float maximum_y = static_cast<float>(kLogicalHeight) - kPlayerSize;

        state.player_x = std::clamp(state.player_x, 0.0F, maximum_x);
        state.player_y = std::clamp(state.player_y, 0.0F, maximum_y);
    }

    /**
     * @brief Draw one frame of the temporary bootstrap scene.
     *
     * @param state Application state containing a valid SDL renderer.
     *
     * @details
     * The background, play area, and player square deliberately use solid
     * colours. This avoids introducing image loading, font rendering, shaders,
     * materials, or asset management before the browser build path is proven.
     *
     * IMPORTANT:
     * This function is not Veyra's final renderer. It is a visual smoke test
     * for the SDL3 platform layer and browser presentation path.
     */
    void draw_frame(const AppState& state)
    {
        // Clear the old frame first so objects from an earlier position do not
        // remain visible after the player moves.
        SDL_SetRenderDrawColor(state.renderer, 12, 17, 30, 255);
        SDL_RenderClear(state.renderer);

        // Draw a subtle inner play area so the logical scene boundaries are
        // visible when the browser letterboxes the canvas.
        const SDL_FRect play_area{
            12.0F,
            12.0F,
            static_cast<float>(kLogicalWidth) - 24.0F,
            static_cast<float>(kLogicalHeight) - 24.0F
        };

        SDL_SetRenderDrawColor(state.renderer, 22, 31, 50, 255);
        SDL_RenderFillRect(state.renderer, &play_area);

        SDL_SetRenderDrawColor(state.renderer, 78, 103, 139, 255);
        SDL_RenderRect(state.renderer, &play_area);

        // The blue square is the temporary player object. Later prototypes
        // will replace this drawing with Veyra scene and rendering components.
        const SDL_FRect player{
            state.player_x,
            state.player_y,
            kPlayerSize,
            kPlayerSize
        };

        SDL_SetRenderDrawColor(state.renderer, 65, 190, 255, 255);
        SDL_RenderFillRect(state.renderer, &player);

        // Present submits the completed frame to the window or browser canvas.
        SDL_RenderPresent(state.renderer);
    }
} // namespace

/**
 * @brief Perform one-time application initialization.
 *
 * @param appstate Output location where SDL stores our application-state pointer.
 * @param argc Standard application argument count (unused in this prototype).
 * @param argv Standard application argument list (unused in this prototype).
 *
 * @return SDL_APP_CONTINUE when startup succeeds, otherwise SDL_APP_FAILURE.
 *
 * @details
 * SDL calls this function once. It must not contain a permanent while loop.
 * SDL will call SDL_AppIterate for subsequent frames and SDL_AppEvent when
 * input or window events arrive.
 */
SDL_AppResult SDLCALL SDL_AppInit(void** appstate, int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    g_app_state = {};
    *appstate = &g_app_state;

    SDL_SetAppMetadata(
        "Veyra Engine Browser Prototype",
        "0.1.0",
        "dev.veyra.engine.browser-prototype"
    );

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Veyra startup failed while initializing SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer(
            "Veyra Engine - Browser Prototype 001",
            kLogicalWidth,
            kLogicalHeight,
            SDL_WINDOW_RESIZABLE,
            &g_app_state.window,
            &g_app_state.renderer
        ))
    {
        SDL_Log("Veyra startup failed while creating the window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Logical presentation keeps the game coordinate system constant and
    // letterboxes the scene rather than stretching it when the window shape
    // does not match 16:9.
    if (!SDL_SetRenderLogicalPresentation(
            g_app_state.renderer,
            kLogicalWidth,
            kLogicalHeight,
            SDL_LOGICAL_PRESENTATION_LETTERBOX
        ))
    {
        SDL_Log("Veyra could not configure logical presentation: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Browser builds should normally synchronize frames with the display.
    // This asks SDL to use the platform's appropriate presentation timing.
    if (!SDL_SetRenderVSync(g_app_state.renderer, 1))
    {
        SDL_Log("Veyra could not enable renderer VSync: %s", SDL_GetError());
    }

    g_app_state.previous_frame_ticks = SDL_GetTicks();

    SDL_Log("Veyra Browser Prototype 001 started successfully.");
    return SDL_APP_CONTINUE;
}

/**
 * @brief Handle one SDL event.
 *
 * @param appstate Application state supplied by SDL_AppInit.
 * @param event Event currently being delivered by SDL.
 *
 * @return SDL_APP_SUCCESS to request a normal exit, otherwise continue.
 *
 * @details
 * This prototype handles:
 * - Quit requests from the windowing system.
 * - Escape on a keyboard.
 * - Mouse clicks to reposition the player.
 * - Touch presses to reposition the player on a touchscreen.
 *
 * Touch positions arrive as normalized values between zero and one. They are
 * converted into the prototype's logical 960x540 coordinate system.
 */
SDL_AppResult SDLCALL SDL_AppEvent(void* appstate, SDL_Event* event)
{
    auto* state = static_cast<AppState*>(appstate);

    if (state == nullptr || event == nullptr)
    {
        return SDL_APP_FAILURE;
    }

    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;

        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE)
            {
                return SDL_APP_SUCCESS;
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            move_player_to(*state, event->button.x, event->button.y);
            break;

        case SDL_EVENT_FINGER_DOWN:
            move_player_to(
                *state,
                event->tfinger.x * static_cast<float>(kLogicalWidth),
                event->tfinger.y * static_cast<float>(kLogicalHeight)
            );
            break;

        default:
            // Events not needed by this small prototype are intentionally
            // ignored. Later input systems will translate more SDL events
            // into Veyra's own platform-independent input representation.
            break;
    }

    return SDL_APP_CONTINUE;
}

/**
 * @brief Update and draw one frame.
 *
 * @param appstate Application state supplied by SDL_AppInit.
 *
 * @return SDL_APP_CONTINUE while the application should keep running.
 *
 * @details
 * SDL calls this repeatedly. On a browser, SDL's callback integration gives
 * control back to the browser between frames, which allows the page to remain
 * responsive and the browser to schedule drawing efficiently.
 */
SDL_AppResult SDLCALL SDL_AppIterate(void* appstate)
{
    auto* state = static_cast<AppState*>(appstate);

    if (state == nullptr || state->renderer == nullptr)
    {
        return SDL_APP_FAILURE;
    }

    const Uint64 current_ticks = SDL_GetTicks();

    float delta_seconds = static_cast<float>(
        current_ticks - state->previous_frame_ticks
    ) / 1000.0F;

    state->previous_frame_ticks = current_ticks;

    delta_seconds = std::min(delta_seconds, kMaximumFrameDeltaSeconds);

    update_player_from_keyboard(*state, delta_seconds);
    draw_frame(*state);

    return SDL_APP_CONTINUE;
}

/**
 * @brief Release resources when SDL is shutting the application down.
 *
 * @param appstate Application state supplied by SDL_AppInit.
 * @param result The reason the application stopped (not needed here).
 *
 * @details
 * Destroy the renderer before its window because the renderer depends on the
 * window. SDL handles its own global shutdown after this callback returns.
 */
void SDLCALL SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    (void)result;

    auto* state = static_cast<AppState*>(appstate);

    if (state == nullptr)
    {
        return;
    }

    if (state->renderer != nullptr)
    {
        SDL_DestroyRenderer(state->renderer);
        state->renderer = nullptr;
    }

    if (state->window != nullptr)
    {
        SDL_DestroyWindow(state->window);
        state->window = nullptr;
    }

    SDL_Log("Veyra Browser Prototype 001 shut down cleanly.");
}
