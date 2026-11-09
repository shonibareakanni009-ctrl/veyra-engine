/**
 * @file runtime_renderer.h
 * @brief SDL3-window-backed bgfx renderer for validated scene geometry.
 */
#pragma once

#include "scene_geometry.h"

#include <memory>
#include <string>

struct SDL_Window;

namespace Veyra::Runtime
{
class RuntimeRenderer final
{
public:
    RuntimeRenderer();
    ~RuntimeRenderer();
    RuntimeRenderer(const RuntimeRenderer&) = delete;
    RuntimeRenderer& operator=(const RuntimeRenderer&) = delete;

    bool initialize(SDL_Window* window, const SceneGeometry& geometry,
                    std::string* error = nullptr);
    void render(int pixelWidth, int pixelHeight);
    void shutdown() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace Veyra::Runtime
