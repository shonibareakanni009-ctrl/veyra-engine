/**
 * @file scene_geometry.cpp
 * @brief Converts validated 2D scene primitives into transformed CPU triangles.
 */
#include "scene_geometry.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace Veyra::Runtime
{
namespace
{
constexpr double Pi = 3.14159265358979323846;
constexpr std::uint32_t RectangleColor = 0xFF8A9BB5U;
constexpr std::uint32_t CircleColor = 0xFF73A6D8U;
constexpr std::uint32_t TriangleColor = 0xFF91B58AU;

struct Point
{
    double x;
    double y;
};

Point transformPoint(const SceneObject& object, Point point)
{
    const double scaledX = point.x * object.scaleX;
    const double scaledY = point.y * object.scaleY;
    const double radians = object.rotation * Pi / 180.0;
    const double cosine = std::cos(radians);
    const double sine = std::sin(radians);
    return {
        object.x + scaledX * cosine - scaledY * sine,
        object.y + scaledX * sine + scaledY * cosine
    };
}

void appendVertex(SceneGeometry& geometry, const SceneObject& object,
                  Point point, std::uint32_t color)
{
    const Point world = transformPoint(object, point);
    geometry.vertices.push_back({
        static_cast<float>(world.x),
        static_cast<float>(world.y),
        color
    });
}

void appendTriangle(SceneGeometry& geometry, std::uint32_t a,
                    std::uint32_t b, std::uint32_t c)
{
    geometry.indices.push_back(a);
    geometry.indices.push_back(b);
    geometry.indices.push_back(c);
}
} // namespace

SceneGeometry buildSceneGeometry(const SceneDocument& scene,
                                 std::uint32_t circleSegments)
{
    SceneGeometry geometry;
    circleSegments = std::clamp(circleSegments, 3U, 4096U);

    for (const SceneObject& object : scene.objects())
    {
        if (!object.visible || object.shape == "sprite")
            continue;

        const std::uint32_t base = static_cast<std::uint32_t>(geometry.vertices.size());
        const double halfWidth = object.width / 2.0;
        const double halfHeight = object.height / 2.0;

        if (object.shape == "rectangle")
        {
            appendVertex(geometry, object, {-halfWidth, -halfHeight}, RectangleColor);
            appendVertex(geometry, object, { halfWidth, -halfHeight}, RectangleColor);
            appendVertex(geometry, object, { halfWidth,  halfHeight}, RectangleColor);
            appendVertex(geometry, object, {-halfWidth,  halfHeight}, RectangleColor);
            appendTriangle(geometry, base, base + 1U, base + 2U);
            appendTriangle(geometry, base, base + 2U, base + 3U);
        }
        else if (object.shape == "circle")
        {
            appendVertex(geometry, object, {0.0, 0.0}, CircleColor);
            for (std::uint32_t segment = 0; segment < circleSegments; ++segment)
            {
                const double angle = (2.0 * Pi * static_cast<double>(segment)) /
                                     static_cast<double>(circleSegments);
                appendVertex(geometry, object,
                             {halfWidth * std::cos(angle), halfHeight * std::sin(angle)},
                             CircleColor);
            }
            for (std::uint32_t segment = 0; segment < circleSegments; ++segment)
            {
                const std::uint32_t current = base + 1U + segment;
                const std::uint32_t next = base + 1U + ((segment + 1U) % circleSegments);
                appendTriangle(geometry, base, current, next);
            }
        }
        else if (object.shape == "triangle")
        {
            // Same local points as the editor viewport. Reverse the last two
            // indices so the triangle has consistent counter-clockwise winding.
            appendVertex(geometry, object, {0.0, halfHeight}, TriangleColor);
            appendVertex(geometry, object, {halfWidth, -halfHeight}, TriangleColor);
            appendVertex(geometry, object, {-halfWidth, -halfHeight}, TriangleColor);
            appendTriangle(geometry, base, base + 2U, base + 1U);
        }
    }

    return geometry;
}
} // namespace Veyra::Runtime
