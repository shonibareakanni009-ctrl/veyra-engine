/**
 * @file scene_geometry_tests.cpp
 * @brief Tests renderer-independent primitive mesh generation and transforms.
 */
#include "scene_document.h"
#include "scene_geometry.h"

#include <cmath>
#include <iostream>
#include <string_view>

namespace
{
int failures = 0;

void check(bool condition, std::string_view description)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << description << '\n';
        ++failures;
    }
}

constexpr std::string_view SceneJson = R"json(
{
  "format": "veyra-scene",
  "formatVersion": 1,
  "objects": [
    {"id":"rect","name":"Rect","shape":"rectangle","x":10,"y":20,"width":8,"height":4},
    {"id":"circle","name":"Circle","shape":"circle","x":30,"y":40,"width":10,"height":6},
    {"id":"triangle","name":"Triangle","shape":"triangle","x":50,"y":60,"width":12,"height":8,"rotation":90},
    {"id":"hidden","name":"Hidden","shape":"rectangle","visible":false},
    {"id":"sprite","name":"Sprite","shape":"sprite","assetPath":"Assets/sprite.png"}
  ]
}
)json";

bool near(float actual, float expected)
{
    return std::abs(actual - expected) < 0.001F;
}

void testPrimitiveCountsAndUnsupportedObjects()
{
    Veyra::Runtime::SceneDocument scene;
    std::string error;
    check(Veyra::Runtime::SceneDocument::loadFromString(SceneJson, scene, &error),
          "test scene loads");
    const auto geometry = Veyra::Runtime::buildSceneGeometry(scene, 12U);
    check(geometry.vertices.size() == 20U, "rectangle, 12-segment circle, and triangle vertices generated");
    check(geometry.indices.size() == 45U, "primitive triangle indices generated");
    check(geometry.vertices.size() > 0U, "visible primitives produce geometry");
}

void testWorldCoordinatesAndRotation()
{
    Veyra::Runtime::SceneDocument scene;
    std::string error;
    check(Veyra::Runtime::SceneDocument::loadFromString(SceneJson, scene, &error),
          "transform scene loads");
    const auto geometry = Veyra::Runtime::buildSceneGeometry(scene, 12U);
    if (geometry.vertices.size() >= 20U)
    {
        // Rectangle's first local bottom-left point (-4,-2), translated by (10,20).
        check(near(geometry.vertices[0].x, 6.0F), "rectangle X uses centered local origin");
        check(near(geometry.vertices[0].y, 18.0F), "rectangle Y uses positive-up world coordinates");

        // Triangle begins after 4 rectangle vertices and 13 circle vertices.
        // Its top-center (0,4) rotated +90 degrees around (50,60) becomes (46,60).
        check(near(geometry.vertices[17].x, 46.0F), "rotation is applied to local X");
        check(near(geometry.vertices[17].y, 60.0F), "rotation is applied in editor world convention");
    }
}

void testCircleSegmentClamp()
{
    Veyra::Runtime::SceneDocument scene;
    std::string error;
    check(Veyra::Runtime::SceneDocument::loadFromString(
        R"json({"format":"veyra-scene","formatVersion":1,"objects":[{"id":"c","name":"C","shape":"circle"}]})json",
        scene, &error), "circle scene loads");
    const auto minimum = Veyra::Runtime::buildSceneGeometry(scene, 1U);
    const auto maximum = Veyra::Runtime::buildSceneGeometry(scene, 5000U);
    check(minimum.vertices.size() == 4U, "circle segment count clamps to minimum three");
    check(maximum.vertices.size() == 4097U, "circle segment count clamps to safe maximum");
}

} // namespace

int main()
{
    testPrimitiveCountsAndUnsupportedObjects();
    testWorldCoordinatesAndRotation();
    testCircleSegmentClamp();

    if (failures != 0)
    {
        std::cerr << failures << " geometry test(s) failed.\n";
        return 1;
    }
    std::cout << "All runtime geometry tests passed.\n";
    return 0;
}
