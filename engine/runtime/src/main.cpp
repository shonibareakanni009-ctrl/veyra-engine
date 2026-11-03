/**
 * @file main.cpp
 * @brief Starts the standalone Veyra runtime with an editor-authored scene.
 *
 * This first checkpoint proves the independent executable, SDL3 window/event
 * lifecycle, and scene loading. Scene rendering is intentionally the next
 * renderer-integration checkpoint; this executable does not claim to draw
 * scene objects yet.
 */
#include "scene_document.h"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <filesystem>
#include <string>

namespace
{
constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;

void printUsage(const char* executable)
{
    SDL_Log("Usage: %s <scene.vscene>", executable);
    SDL_Log("Loads and validates a Veyra scene, then opens the native runtime window.");
}

int runRuntime(const std::filesystem::path& scenePath)
{
    Veyra::Runtime::SceneDocument scene;
    std::string error;
    if (!Veyra::Runtime::SceneDocument::loadFromFile(scenePath, scene, &error))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Scene load failed: %s", error.c_str());
        return 2;
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s", SDL_GetError());
        return 3;
    }

    const std::string windowTitle =
        "Veyra Runtime - " + std::to_string(scene.objects().size()) + " scene objects";
    SDL_Window* window = SDL_CreateWindow(
        windowTitle.c_str(), WindowWidth, WindowHeight, SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 4;
    }

    SDL_Log("Loaded scene: %s", scenePath.string().c_str());
    SDL_Log("Validated scene objects: %zu", scene.objects().size());
    SDL_Log("Runtime window opened. Press Escape or close the window to exit.");

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                     event.window.windowID == SDL_GetWindowID(window))
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN &&
                     event.key.key == SDLK_ESCAPE && !event.key.repeat)
            {
                running = false;
            }
        }

        // Avoid a busy loop while the renderer is being integrated.
        SDL_Delay(8);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Log("Veyra Runtime shut down cleanly.");
    return 0;
}
} // namespace

int main(int argc, char* argv[])
{
    if (argc == 2 && std::string(argv[1]) == "--help")
    {
        printUsage(argv[0]);
        return 0;
    }

    if (argc != 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    return runRuntime(std::filesystem::path(argv[1]));
}
