/**
 * @file scene_geometry.h
 * @brief Renderer-independent triangle mesh generation for validated scenes.
 */
#pragma once

#include "scene_document.h"

#include <cstdint>
#include <vector>

namespace Veyra::Runtime
{
struct GeometryVertex
{
    float x = 0.0F;
    float y = 0.0F;
    std::uint32_t rgba = 0xFFFFFFFFU;
};

struct SceneGeometry
{
    std::vector<GeometryVertex> vertices;
    std::vector<std::uint32_t> indices;
};

/**
 * Build a CPU mesh in editor world coordinates (origin at scene center,
 * positive X right, positive Y up). Invisible objects are omitted. Sprite
 * objects are deliberately omitted until texture loading is implemented.
 */
[[nodiscard]] SceneGeometry buildSceneGeometry(
    const SceneDocument& scene,
    std::uint32_t circleSegments = 32U);
} // namespace Veyra::Runtime
