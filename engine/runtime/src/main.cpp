/**
 * @file main.cpp
 * @brief Loads an editor-authored scene and renders its 2D primitives.
 */
#include "runtime_renderer.h"
#include "scene_document.h"
#include "scene_geometry.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <filesystem>
#include <string>

namespace
{
constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;

void printUsage(const char* executable)
{
    SDL_Log("Usage: %s <scene.vscene>", executable);
    SDL_Log("Loads, validates, and renders a Veyra version 1 scene.");
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

    const Veyra::Runtime::SceneGeometry geometry =
        Veyra::Runtime::buildSceneGeometry(scene);
    const auto spriteCount = static_cast<std::size_t>(std::count_if(
        scene.objects().begin(), scene.objects().end(),
        [](const Veyra::Runtime::SceneObject& object)
        {
            return object.visible && object.shape == "sprite";
        }));

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

    Veyra::Runtime::RuntimeRenderer renderer;
    if (!renderer.initialize(window, geometry, &error))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer initialization failed: %s", error.c_str());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 5;
    }

    SDL_Log("Loaded scene: %s", scenePath.string().c_str());
    SDL_Log("Validated scene objects: %zu", scene.objects().size());
    SDL_Log("Rendered primitive vertices: %zu", geometry.vertices.size());
    if (spriteCount > 0U)
        SDL_Log("Sprite textures are not supported yet; skipped visible sprites: %zu", spriteCount);
    SDL_Log("Runtime renderer initialized. Press Escape or close the window to exit.");

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

        int pixelWidth = 0;
        int pixelHeight = 0;
        if (SDL_GetWindowSizeInPixels(window, &pixelWidth, &pixelHeight))
            renderer.render(pixelWidth, pixelHeight);
        else
            SDL_Delay(8);
    }

    renderer.shutdown();
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
